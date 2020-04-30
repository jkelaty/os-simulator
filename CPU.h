/// @author Jonathan Kelaty
/// @file CS OS Home Project - CPU.h
/// @date 2020-04-14
/// @brief CPU class implementation. Only keeps track of the PID
/// of the process currently being executed. CPU ready-queues are
/// managed by OS class.

#ifndef CPU_H_
#define CPU_H_

#include "DataTypes.h"

/**************************************
 * 
 * Central Processing Unit (CPU) Class
 * 
 **************************************/

class CPU {

    public:
        bool isRunning() const {
            return current_process;
        }

        PID currentProcessPID() const {
            return current_process;
        }

        void runNewProcess(PID new_PID) {
            current_process = new_PID;
        }

        void finishRunningCurrentProcess() {
            current_process = 0;
        }

    private:
        PID current_process{ 0 };

};

#endif // CPU_H_
