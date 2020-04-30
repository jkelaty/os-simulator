/// @author Jonathan Kelaty
/// @file CS OS Home Project - Process.h
/// @date 2020-04-14
/// @brief Process class implementation. Contains PID, process type,
/// and memory location of an individual process. Cannot be modified
/// directly after initialization unless using copy or move assignment,
/// which is used only to overwrite an invalid process.

#ifndef PROCESS_H_
#define PROCESS_H_

#include "DataTypes.h"

/****************
 * 
 * Process Class
 * 
 ****************/

class Process {

    public:
        Process() = default;
        Process & operator=(const Process &) = default;
        Process & operator=(Process &&) = default;

        Process(PID id, ProcessType type, const MemoryBlock & location) :
            process_id{ id },
            process_type{ type },
            memory_location{ location } { /* Intentionally empty */ }

        PID getPID() const {
            return process_id;
        }

        ProcessType getProcessType() const {
            return process_type;
        }

        const MemoryBlock & getMemoryBlock() const {
            return memory_location;
        }

    private:
        PID process_id{ 0 };
        ProcessType process_type{ ProcessType::Invalid };
        MemoryBlock memory_location{ 1,0 };

};

#endif // PROCESS_H_
