/// @author Jonathan Kelaty
/// @file CS OS Home Project - OS.h
/// @date 2020-04-14
/// @brief OS class declaration. Manages CPU, HDD's, RAM, and
/// processes used in simulated OS. Main driver of the OS is the
/// run() member function, which accepts and sanitizes input to
/// perform actions on the OS. CPU has two levels of its ready-queue,
/// one for common processes and one for real-time processes. RT
/// processes will preempt common processes when they enter their
/// ready-queue. Memory is a contiguous first-fit approach. The HDD's
/// IO-queues are first come, first served. The PIDs start from 1
/// using a counter. This implementation does not reuse previous PIDs.

#ifndef OPERATING_SYSTEM_H_
#define OPERATING_SYSTEM_H_

#include <vector>
#include <map>

#include "DataTypes.h"
#include "CPU.h"
#include "RAM.h"
#include "HDD.h"
#include "Process.h"

using std::vector;
using std::map;

/******************************
 * 
 * Operating System (OS) Class
 * 
 ******************************/

class OS {

    public:
        OS() = delete;

        OS(uint RAM_size, uint HDD_count) :
            memory{ RAM_size }, hard_drives{ HDD_count } { /* Intentionally empty */ }

        void run();
        
        void createNewProcess(ProcessType type, uint size);

        // CPU Ready-Queue functions
        void sendProcessToReadyQueue(PID process_ID);
        void terminateCurrentProcess();
        void executeNextProcess();
        void updateCPU();

        // IO-Queue functions
        void sendCurrentProcesstoIOQueue(uint HDD_ID);
        void sendIOProcessToReadyQueue(uint HDD_ID);

        // Snapshots
        void printCPUData() const;
        void printIOData() const;
        void printRAMData() const;

        // Helpers
        ProcessType getProcessType(PID process_ID) const;
        ProcessType currentlyRunningProcessType() const;

    private:
        CPU processor;
        RAM memory;
        vector<HDD> hard_drives;

        ReadyQueue RT_queue;
        ReadyQueue common_queue;

        map<PID, Process> processes;

        PID PID_counter{ 0 };

};

#endif // OPERATING_SYSTEM_H_
