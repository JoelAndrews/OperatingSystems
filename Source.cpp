#include <iostream>
#include <vector>
#include <string>
#include <cstdlib>
#include <deque>
using namespace std;

void memoryManager(int, int);
int allocate(int, int);
int deallocate(int);
int write(int, int);
int read(int, int);
void printMemory();
void setLRU();
void setFIFO();
void put_back_pages(int);
void printPageFaults();
bool clear_memory_FIFO(int);
int clear_memory_LRU(int);

struct pidStruct
{
	int pid;
	int totalFrames;
	std::vector<int> pageTable;
};




//Physical Memory Space
std::vector<unsigned char> physicalMemorySpace;
//Free Frame List
std::vector<bool> freeFrameList;
bool LRU = false;
bool FIFO = false;
std::vector<pidStruct> processes;
int totalmemory;
std::vector<int> memoryReplacementTable;
std::vector<int> mainMemoryTimer;
int oldestFIFOlocation;
int memoryreplacementindex = 0;
int pageFaults = 0;
std::deque<int> LRUStack;



int main()
{
	std::string userCommand = "";

	std::vector<std::string> userStringArguments;
	std::vector<std::string>userCommandArguments;
	while (true) {
		std::getline(std::cin, userCommand);
		for (size_t i = 0, j = 0; i != userCommand.npos; i = j) {
			userCommandArguments.push_back(userCommand.substr(i + (i != 0), (j = userCommand.find(' ', i + 1)) - i - (i != 0)));
		}



		switch (userCommandArguments[0][0])
		{
		case 'M': memoryManager(std::atoi(userCommandArguments[1].c_str()), std::atoi(userCommandArguments[2].c_str()));
			break;
		case 'A': allocate(std::atoi(userCommandArguments[1].c_str()), std::atoi(userCommandArguments[2].c_str()));
			break;
		case 'W':  write(std::atoi(userCommandArguments[1].c_str()), std::atoi(userCommandArguments[2].c_str()));
			break;
		case 'D': deallocate(std::atoi(userCommandArguments[1].c_str()));
			break;
		case 'P': if (userCommandArguments[1] == "PF") {
			printPageFaults();
		}
				  else {
					  printMemory();
				  }
				  break;
		case 'S':
			if (userCommandArguments[1] == "LRU")
				setLRU();
			else if (userCommandArguments[1] == "FIFO")
				setFIFO();
			break;
		case 'R': read(std::atoi(userCommandArguments[1].c_str()), std::atoi(userCommandArguments[2].c_str()));
			break;
		case 'X': return 0;
			break;

		}
		userStringArguments.clear();
		userCommandArguments.clear();



	}
}


void printPageFaults() {
	cout << "Page Faults: " << pageFaults << std::endl;
}

void setLRU() {



	LRU = true;
	FIFO = false;

	std::cout << "LRU set!" << std::endl;

}

void setFIFO() {
	FIFO = true;
	LRU = false;

	std::cout << "FIFO set!" << std::endl;
}


void memoryManager(int memSize, int frameSize)
{
	totalmemory = memSize;
	for (int i = 0; i < memSize; i++) {
		physicalMemorySpace.push_back(0);
		freeFrameList.push_back(true);

	}



}

bool clear_memory_FIFO(int toClear) {

	int i = 0;

	while (toClear > 0) {

		int thisOne = mainMemoryTimer[0];
		mainMemoryTimer.erase(mainMemoryTimer.begin());

		memoryReplacementTable.push_back(physicalMemorySpace[thisOne]);
		freeFrameList[thisOne] = true;
		physicalMemorySpace[thisOne] = 0;

		totalmemory++;
		toClear--;
		pageFaults++;
	}

	return true;
}





int clear_memory_LRU(int memory_to_clear) {

	int i = 0;

	while (memory_to_clear > 0) {

		int thisOne = LRUStack.back();
		LRUStack.pop_back();



		memoryReplacementTable.push_back(physicalMemorySpace[thisOne]);
		freeFrameList[thisOne] = true;
		physicalMemorySpace[thisOne] = 0;

		totalmemory++;
		memory_to_clear--;
		pageFaults++;


	}
	return true;
}





int allocate(int allocationSize, int pId)
{
	int memoryToClear;

	if (totalmemory < allocationSize) {
		memoryToClear = allocationSize - totalmemory;

		if (FIFO) {
			clear_memory_FIFO(memoryToClear);
		}
		else if (LRU) {
			clear_memory_LRU(memoryToClear);
		}


	}





	if (allocationSize > totalmemory) {
		std::cout << "Not enough space to allocate!" << std::endl;
		return -1;
	}
	else {
		totalmemory -= allocationSize;
	}
	pidStruct process;
	process.pid = pId;
	process.totalFrames = allocationSize;
	std::vector<int> pageTable;
	int totalAllocated = 0;
	int index = 0;
	while (totalAllocated < allocationSize)
	{
		index = rand() % physicalMemorySpace.size();
		if (freeFrameList[index]) {
			pageTable.push_back(index);
			freeFrameList[index] = false;
			totalAllocated++;

			if (FIFO) {
				mainMemoryTimer.push_back(index);
			}
			else if (LRU) {
				LRUStack.push_front(index);
			}

		}
	}
	process.pageTable = pageTable;
	processes.push_back(process);


	return 1;
}

int deallocate(int pid)
{
	int deallocated, index, erased = 0;
	for (int i = 0; i < processes.size(); i++) {
		if (processes[i].pid == pid) {
			for (int x = 0; x < processes[i].pageTable.size(); x++) {
				physicalMemorySpace[processes[i].pageTable[x]] = 0;
				freeFrameList[processes[i].pageTable[x]] = true;
				totalmemory = totalmemory + 1;
				//need to erase from oldestproccesses
				for (int y = 0; y < mainMemoryTimer.size(); y++) {
					//find the one to erase from oldestprocesses
					if (mainMemoryTimer[y] == processes[i].pageTable[x]) {
						mainMemoryTimer.erase(mainMemoryTimer.begin() + y);
						break;
					}
				}
				erased++;
			}
			processes.erase(processes.begin() + i);
			break;
		}
	}

	if (FIFO)
		put_back_pages(erased);


	return 1;
}


void put_back_pages(int erased) {
	int i = 0;
	int totalfree = 0;
	for (int z = 0; z < freeFrameList.size(); z++) {
		if (freeFrameList[z] == true)
			totalfree++;
	}

	while (erased > 0 && totalfree > 0 && memoryReplacementTable.size() > 0) {
		i = rand() % physicalMemorySpace.size();

		if (freeFrameList[i] == true) {
			freeFrameList[i] = false;
			physicalMemorySpace[i] = memoryReplacementTable[0];
			mainMemoryTimer.push_back(i);
			memoryReplacementTable.erase(memoryReplacementTable.begin());

			erased--;
			totalfree--;



		}


	}



}

int write(int pid, int logical_address)
{
	//find the right pid
	for (int i = 0; i < processes.size(); i++) {
		if (processes[i].pid == pid) {
			if (logical_address > processes[i].totalFrames || logical_address == 0)
				return -1;
			else {

				physicalMemorySpace[processes[i].pageTable[logical_address - 1]] = 1;


				if (LRU) {
					for (int k = 0; k < LRUStack.size(); k++) {
						if (LRUStack[k] == processes[i].pageTable[logical_address - 1]) {
							LRUStack.erase(LRUStack.begin() + k);
							LRUStack.push_front(processes[i].pageTable[logical_address - 1]);
							break;
						}
					}
				}


				break;
			}

		}

	}
	return 1;
}

int read(int pid, int logical_address)
{
	//find the right pid
	for (int i = 0; i < processes.size(); i++) {
		if (processes[i].pid == pid) {
			if (logical_address > processes[i].totalFrames)
				return -1;
			else {
				std::cout << "The phsyical memory at this location is " << (int)physicalMemorySpace[processes[i].pageTable[logical_address - 1]] << std::endl;
				break;
			}

		}

	}
}

void printMemory()
{
	std::cout << "Physical memory:";
	for (int i = 0; i < physicalMemorySpace.size(); i++) {
		std::cout << (int)physicalMemorySpace[i];
	}

	std::cout << std::endl;

	std::cout << "Free frame list:";
	for (int i = 0; i < freeFrameList.size(); i++) {
		//std::cout << freeFrameList[i];
		if (freeFrameList[i])
			std::cout << i;
	}

	std::cout << std::endl;

	std::cout << "Process list:" << std::endl;
	for (int i = 0; i < processes.size(); i++) {
		std::cout << processes[i].pid << ": " << processes[i].totalFrames;
		std::cout << std::endl;
	}


	std::cout << "Oldest Process list:" << std::endl;
	for (int i = 0; i < mainMemoryTimer.size(); i++) {
		std::cout << mainMemoryTimer[i] << ",";
	}

	std::cout << "LRU Stack: " << std::endl;
	for (int i = 0; i < LRUStack.size(); i++) {
		std::cout << LRUStack[i] << ",";
	}




	std::cout << std::endl;

}