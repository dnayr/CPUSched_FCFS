// CPU Scheduler Simulator - FCFS
// COP4610 - 3/22/17 - Richard Finkel

#include <iostream>
#include <string>
#include <list>
#include "Process.h"

using namespace std;

// ----- Global Vars ----------
//   |------------- Processes and their jobs + I/O --------------
const int PROC_AMT = 8;

// ------ Function Prototypes -----------

void start_processes(Process[]);
void init_jobQueue(Process[], list<Process>&);
void load_readyQueue(list<Process>&, list<Process>&);
void updateProcesses(int);
bool processes_done(list<Process>&, list<Process>&);

void PrintFinalReport(list<Process>&, int, int);
void PrintContextSwitch(list<Process>&, list<Process>&, list<Process>&, int );

int main() 
{
	///FCFS algorithm for CPU Scheduler

	Process proc[PROC_AMT];
	list<Process> jobQ;
	list<Process> readyQ;
	list<Process> ioQ;
	list<Process> doneQ;
	list<Process>::iterator itrIOQ = ioQ.begin();

	start_processes(proc);
	init_jobQueue(proc, jobQ);
	load_readyQueue(jobQ, readyQ);					// State: Start processes

	int time = 0;
	int cpuTime = 1;
	int ioTime = 1;

	// >>> CPU loop <<<
	// CPU loop stops when all CPU bursts and I/O bursts are finished

	PrintContextSwitch(readyQ, ioQ, doneQ, time);

	while (!processes_done(readyQ, ioQ))	/*(time < 649)*/		//Important cycles : 139, 600, 650
	{

		// State: Start New Process
		if (!readyQ.empty() && readyQ.front().newState == true)
		{
			readyQ.front().newState = false;
			readyQ.front().startTime = time;

			cout << "Process P" << readyQ.front().procID << " starting, ";
			PrintContextSwitch(readyQ, ioQ, doneQ, time);
		}
		
		// State: Run Process
		if (!readyQ.empty())		// Something is in the Ready Queue
		{
			
			if (readyQ.front().bursts.front())
			{
				--readyQ.front().bursts.front();			// CPU cycle

				if (readyQ.front().bursts.front() == 0
					&& readyQ.front().bursts.size() == 1)	// Last CPU burst, send to done queue
				{
					cout << "Process P" << readyQ.front().procID << " is done, ";
					readyQ.front().endTime = time;
					readyQ.front().bursts.pop();
					readyQ.front().processMode = 3;

					doneQ.push_back(readyQ.front());

					readyQ.pop_front();	

					if (readyQ.empty())
						readyQ.clear();

					PrintContextSwitch(readyQ, ioQ, doneQ, time);
				}
				else if (readyQ.front().bursts.front() == 0)	// Process needs I/O device data, send to I/O queue
				{
					cout << "Process P" << readyQ.front().procID << " needs I/O, ";
					readyQ.front().bursts.pop();				// Delete CPU burst from process' burst queue
					readyQ.front().processMode = 1;				// Switch process mode to I/O


					ioQ.push_back(readyQ.front());		// Send to I/O queue
					readyQ.pop_front();

					PrintContextSwitch(readyQ, ioQ, doneQ, time);
				}
				if (!readyQ.empty())
				{
					readyQ.front().runTime++;

					for (list<Process>::iterator iReady = ++readyQ.begin(); iReady != readyQ.end(); iReady++)
						iReady->waitTime++;
				}
				cpuTime++;
			}
		}

		// State: I/O wait/receive
		if (!ioQ.empty())			// State: Run I/O
		{
			////cout << "burst IO?:" << readyQ.front().bursts.front() << "<__>" << time << endl;

			//if (ioQ.front().bursts.front())
			//{
				itrIOQ = ioQ.begin();

				while (itrIOQ != ioQ.end())
				////for (list<Process>::iterator itrIOQ = ioQ.begin(); itrIOQ != ioQ.end(); itrIOQ++)
				{
					--itrIOQ->bursts.front();

					if (itrIOQ->bursts.front() == 0)	// I/O done, send back to ready queue
					{
						cout << "Process P" << itrIOQ->procID << " returning to cpu ready state, ";
						itrIOQ->bursts.pop();
						itrIOQ->processMode = 0;

						//if (!readyQ.empty() && itrIOQ != ioQ.end()) {
						readyQ.push_back(*itrIOQ);
						itrIOQ = ioQ.erase(itrIOQ);
					//}
						//else
							//cout << "EMPTY Ready Queue\n\n";

						PrintContextSwitch(readyQ, ioQ, doneQ, time);
					}
					else
						++itrIOQ;
				}

				for (list<Process>::iterator iIO = ioQ.begin(); iIO != ioQ.end(); iIO++)
					iIO->ioTime++;
				ioTime++;
			//}
		}
		time++;
		
	}

	PrintFinalReport(doneQ, time, cpuTime);
	system("color 70");

	return 0;
}
////cout << "burst RL?:" << readyQ.front().bursts.front()<< "<__>" << time << endl;
////PrintContextSwitch(readyQ, ioQ, time);	// Per cycle data per process

// ------ Functions -----------
/// Put processes into process array
void start_processes(Process p[]) {
	int P1[] = { 4,24, 5,73, 3,31, 5,27, 4,33, 6,43, 4,64, 5,19, 2 };		//total activities: 17
	p[0].setProcTimes(P1, 17, 1);
	int P2[] = { 18,31,19,35,11,42,18,43,19,47,18,43,17,51,19,32,10 };		//17
	p[1].setProcTimes(P2, 17, 2);
	int P3[] = { 6,18, 4,21, 7,19, 4,16, 5,29, 7,21, 8,22, 6,24, 5 };		//17
	p[2].setProcTimes(P3, 17, 3);
	int P4[] = { 17,42,19,55,20,54,17,52,15,67,12,72,15,66,14 };			//15
	p[3].setProcTimes(P4, 15, 4);
	int P5[] = { 5,81, 4,82, 5,71, 3,61, 5,62, 4,51, 3,77, 4,61, 3,42, 5 }; //19
	p[4].setProcTimes(P5, 19, 5);
	int P6[] = { 10,35,12,41,14,33,11,32,15,41,13,29,11 };					//13
	p[5].setProcTimes(P6, 13, 6);
	int P7[] = { 21,51,23,53,24,61,22,31,21,43,20 };						//11
	p[6].setProcTimes(P7, 11, 7);
	int P8[] = { 11,52,14,42,15,31,17,21,16,43,12,31,13,32,15 };			//15
	p[7].setProcTimes(P8, 15, 8);
}

///Put all the needy processes in their sling baskets
void init_jobQueue(Process p[], list<Process>& jobQueue) {

	for (int i = 0; i < PROC_AMT; i++)
		jobQueue.push_back(p[i]);
}

///Fire! Put jobs into the Ready Queue to be called into the CPU office later
void load_readyQueue(list<Process>& jobQueue, list<Process>& readyQueue) {

	if (!jobQueue.empty()) {

		for (list<Process>::iterator iter = jobQueue.begin(); iter != jobQueue.end(); iter++) {
			readyQueue.push_back(*iter);
		}
	}
}

void updateProcesses(int cpuTime)
{
	// procs in ready queue have wait time increased
	// I/O queue procs get time subtracted


}

/// Check that there are no more active processes in the Ready and I/O queues
bool processes_done(list<Process>& readyQueue, list<Process>& ioQueue) {
	if (readyQueue.empty() && ioQueue.empty())	// Burger is ready
		{return true;}
	else										// Buns still in the oven
		{return false;}

	return true;
}

//****** Calculation & Printer Functions **************

void PrintFinalReport(list<Process>& lp, int totalTime, int cpuTime)
{
	cout << "\n\nFinal Report for CPU Scheduler\n\n";

	cout << "Total Run Time: " << totalTime << "\n\n";


	float cpuUtil = 0.0;
	cpuUtil = cpuTime / totalTime;

	// CPU utilization
	cout << "CPU Utilization: " << cpuUtil << "\n\n";
		// sum of actual work time / total time		




	// waiting time for each process + avg wait
	cout << "Wait Times: \n" ;		// time spent waiting in ready state

	float waitTotal = 0.0;
	for (list<Process>::iterator i = lp.begin(); i != lp.end(); i++)
	{
		cout << "P" << i->procID << " waittime: " << i->waitTime << endl;
		waitTotal += i->waitTime;
	}

	cout << "Wait Time Avg (Tw): " << waitTotal / 8 << "\n\n";




	// turnaround time each + avg
	cout << "Turnaround Times: \n";

	int turnTotal = 0;
	for (list<Process>::iterator i = lp.begin(); i != lp.end(); i++)
	{
		//cout << "P" << i->procID << ": " << i-> <<endl;
		//turnTotal += i->;
	}
	cout << "Turnaround Avg: " << turnTotal / 8 << "\n\n";

		// time of admit to time of completion (endTime - startTime)
		// sum of periods spent waiting, in ready queue, I/O, CPU




	cout << "Response Times: \n";

	float respTotal = 0;
	for (list<Process>::iterator i = lp.begin(); i != lp.end(); i++)
	{
		cout << "P" << i->procID << ": " << i->startTime <<endl;
		respTotal += i->startTime;
	}
	cout << "Response Avg: " << respTotal / 8 << "\n\n";




}


void PrintContextSwitch(list<Process> &readyL, list<Process> &ioL, list<Process> &dne, int cpuRunTime) {
	cout << "Execution Time: " << cpuRunTime << "                                 >>>>>>>>>>" << endl;
	cout << "-------------------------------\n"
		<< "Process\tCPU Burst\tI/O Burst\tState\n";

	if (!readyL.empty())
	{
		cout << "<<Ready List>>\n";
		for (list<Process>::iterator iRL = readyL.begin(); iRL != readyL.end(); iRL++)
		{
			cout << "P" << iRL->procID << "\t" << iRL->bursts.front() << "\t\t\t\t";

			if (iRL->processMode == 0)
				cout << "Processing";
			else if (iRL->processMode == 1 || iRL->processMode == 2 )
				cout << "Waiting";

			cout << endl;
		}
	}

	if (!ioL.empty())
	{
		cout << "\n<<I/O List>>\n";
		for (list<Process>::iterator iIOL = ioL.begin(); iIOL != ioL.end(); iIOL++)
		{
			cout << "P" << iIOL->procID << "\t\t\t" << iIOL->bursts.front();

			if (iIOL->processMode == 1)
				cout << "\t\tI/O Wait";

			cout << endl;
		}
	}

	if (!dne.empty())
	cout << "\n<<Done List>>\n";
	for (list<Process>::iterator iDone = dne.begin(); iDone != dne.end(); iDone++)
	{
		cout << "P" << iDone->procID << "\t\t\t\t\t" ;

		if (iDone->processMode == 3)
			cout << "Done";

		cout << endl;
	}
	cout<< endl;
}