//Joel Andrews Assign 3
//Dr. Won
//Due Nov 30
#include <iostream>
#include <vector>
#include <string>
#include <cstdlib>

void memoryManager(int, int);
int allocate(int, int);
int deallocate(int);
int write(int, int);
int read(int, int);
void printMemory();

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

std::vector<pidStruct> processes;
int totalmemory;





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
		case 'P': printMemory();
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

	void memoryManager(int memSize, int frameSize)
	{
		totalmemory = memSize;
		for (int i = 0; i < memSize; i++) {
			physicalMemorySpace.push_back(0);
			freeFrameList.push_back(true);
		}
	}

	int allocate(int allocationSize, int pId)
	{
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

			}
		}
		process.pageTable = pageTable;
		processes.push_back(process);


		return 1;
	}

	int deallocate(int pid)
	{
		int deallocated, index = 0;
		for (int i = 0; i < processes.size(); i++) {
			if (processes[i].pid == pid) {
				for (int x = 0; x < processes[i].pageTable.size(); x++) {
					physicalMemorySpace[processes[i].pageTable[x]] = 0;
					freeFrameList[processes[i].pageTable[x]] = true;
					totalmemory = totalmemory + 1;
				}
				processes.erase(processes.begin() + i);
				break;
			}
		}
		

		return 1;
	}

	int write(int pid, int logical_address)
	{
		//find the right pid
		for (int i = 0; i < processes.size(); i++) {
			if (processes[i].pid == pid) {
				if (logical_address > processes[i].totalFrames || logical_address == 0)
					return -1;
				else {
					
					physicalMemorySpace[processes[i].pageTable[logical_address-1]] = 1;
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
					std::cout << "The phsyical memory at this location is " << (int)physicalMemorySpace[processes[i].pageTable[logical_address-1]] << std::endl;
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

		std::cout << std::endl;

	}