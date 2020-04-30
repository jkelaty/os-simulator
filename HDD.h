/// @author Jonathan Kelaty
/// @file CS OS Home Project - HDD.h
/// @date 2020-04-14
/// @brief HDD class implementation. Manages its IO-queue, which is
/// implemented as first come, first served (FCFS). The next process
/// is served (if available) whenever a process is finished being
/// served. Enumeration of HDD number is managed by OS class.

#ifndef HDD_H_
#define HDD_H_

#include "DataTypes.h"

/******************************
 * 
 * Hard Disk Drive (HDD) Class
 * 
 ******************************/

class HDD {

    public:
        bool isServing() const {
            return current_process;
        }

        void updateIOQueue() {
            /* Only serve new process when not serving 
                and we have processes to serve */
            if ( ! isServing() && ! IO_queue.empty() ) {
                current_process = IO_queue.front();
                IO_queue.pop_front();
            }
        }

        void sentProcessToIOQueue(PID process) {
            IO_queue.push_back( process );
            updateIOQueue();
        }

        void finishServingCurrentProcess() {
            current_process = 0;
            updateIOQueue();
        }

        PID currentProcessPID() const {
            return current_process;
        }

        const ReadyQueue & getIOQueue() const {
            return IO_queue;
        }

    private:
        PID current_process{ 0 };
        ReadyQueue IO_queue;

};

#endif // HDD_H_
