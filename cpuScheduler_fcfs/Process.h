#pragma once
#include <queue>
//#include <>

using namespace std;

class Process {

public:
	Process() {
		procID = -1;
		processMode = 0;
		startTime = 0;
		endTime = -1;
		ioTime = 0;
		runTime = 0;
		waitTime = 0;
		newState = true;
	}

	/// Sets a process' CPU Burst and I/O times, also sets Process ID
	void setProcTimes(int inArr[], int max, int pid) {
		for (int i = 0; i < max; i++)
			bursts.push(inArr[i]);

		procID = pid;
	}


	queue<int> bursts;		// Has CPU burst and I/O bursts. Burst and I/O alternate execution

	int procID;
	bool newState;
	int processMode;		// 0 = CPU Burst (active), 1 = I/O time, 2 = idle, 3 = Done
	int startTime;			// Time at which CPU finally starts process' first job
	int endTime;			// Time at which process has no CPU bursts nor I/O to go through
	int ioTime;				// Keeps track of I/O cycles
	int runTime;			// Keeps track of process running time
	int waitTime;			// Keeps track of time process is in ready queue and I/O
};