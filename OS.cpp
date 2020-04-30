/// @author Jonathan Kelaty
/// @file CS OS Home Project - OS.cpp
/// @date 2020-04-14
/// @brief OS class implementation. Implementation details
/// can be found in function documentation below.

#include <iostream>
#include <string>
#include <unordered_map>

#include "DataTypes.h"
#include "OS.h"

using std::cout;
using std::cin;
using std::endl;
using std::string;
using std::unordered_map;

// Operations that can be performed by OS
enum class Operation { A, AR, Q, t, d, D, S };

// Snapshot commands that can be performed by OS
enum class Snapshot { r, i, m };

// Operation lookup hash table
const unordered_map<string, Operation> OPERATIONS {
    {"A",  Operation::A},
    {"AR", Operation::AR},
    {"Q",  Operation::Q},
    {"t",  Operation::t},
    {"d",  Operation::d},
    {"D",  Operation::D},
    {"S",  Operation::S}
};

// Snapshot lookup hash table
const unordered_map<string, Snapshot> SNAPSHOTS {
    {"r", Snapshot::r},
    {"i", Snapshot::i},
    {"m", Snapshot::m}
};

/**
 * Reads in argument for a given operation and determines if the input
 * is valid (is an unsigned integer). If not, std::cin is cleared and
 * input stream is ignored. Error is then output to indicate this was
 * an invalid argument.
 * 
 * @param arg Unsigned integer argument to read in
 * 
 * @return False if invalid argument, else true
 */
bool isValidOperationArgument(uint & arg) {
    if ( ! (cin >> arg) ) {
        cout << "\n\tError - Invalid argument\n" << endl;
        cin.clear();
        cin.ignore(256, '\n');
        return false;
    }
    else {
        return true;
    }
}

/**
 * Simulates the operating system by prompting the user to enter commands
 * to create or interact with processes used by the CPU and IO devices.
 * Input is sanitized to make sure we don't encounter errors during execution.
 * Operations that can be performed by the user:
 * 
 *  A <#>  - Creates new common process of size #
 *  AR <#> - Creates new real-time process of size #
 *  Q      - Ends time slice of currently executing process
 *  t      - Terminates currently executing process
 *  d <#>  - Send currently running process to hard disk #
 *  D <#>  - Send process being served by hard disk # to ready-queue
 *  S r    - Snapshot of CPU and ready-queues
 *  S i    - Snapshot of IO devices and their IO-queues
 *  S m    - Snapshot of RAM
 */
void OS::run() {
    string operation{ "" };
    string snapshot{ "" };
    uint uint_arg{ 0 };

    while ( true ) {
        
        cout << ">> ";
        cin >> operation;

        // Check if valid operation
        if ( ! OPERATIONS.count(operation) ) {
            cout << "\n\tError - Invalid operation: " << operation << '\n' << endl;
            cin.clear();
            cin.ignore(256, '\n');
            continue;
        }

        switch ( OPERATIONS.at(operation) ) {
            case Operation::A: // Create new common process

                if ( isValidOperationArgument(uint_arg) ) {
                    createNewProcess(ProcessType::Common, uint_arg);
                }
                break;

            case Operation::AR: // Create new RT process

                if ( isValidOperationArgument(uint_arg) ) {
                    createNewProcess(ProcessType::RealTime, uint_arg);
                }
                break;

            case Operation::Q: // End time slice for currently running process

                executeNextProcess();
                break;

            case Operation::t: // Terminate currently running process

                terminateCurrentProcess();
                break;

            case Operation::d: // Send currently running process to IO Queue

                if ( isValidOperationArgument(uint_arg) ) {
                    sendCurrentProcesstoIOQueue(uint_arg);
                }
                break;

            case Operation::D: // Send process back form IO to ready-queue

                if ( isValidOperationArgument(uint_arg) ) {
                    sendIOProcessToReadyQueue(uint_arg);
                }
                break;

            case Operation::S: // Snapshot command

                cin >> snapshot;

                // Check if valid snapshot
                if ( ! SNAPSHOTS.count(snapshot) ) {
                    cout << "\n\tError - Invalid snapshot: " << snapshot << '\n' << endl;
                    cin.clear();
                    cin.ignore(256, '\n');
                    continue;
                }
                
                switch( SNAPSHOTS.at(snapshot) ) {
                    case Snapshot::r: // Print CPU ready-queue data
                        printCPUData();
                        break;
                    case Snapshot::i: // Print IO-queue data
                        printIOData();
                        break;
                    case Snapshot::m: // Print RAM data
                        printRAMData();
                        break;
                }
                break;
        }

    }
}

/**
 * Creates new process and sends to ready queue. Will first check
 * if there is a valid memory block that the process can fit into,
 * and then create the process, else output an error message.
 * 
 * @param type Process type
 * @param size Size of process
 */
void OS::createNewProcess(ProcessType type, uint size) {
    if ( ! size ) {
        cout << "\n\tError - Invalid process size of 0\n" << endl;
        return;
    }

    // Find memory block to fit new process
    MemoryBlock address{ memory.findAvailableMemoryBlock(size) };

    // Check if valid memory block and create the process
    if ( address.first <= address.second ) {
        PID process_ID{ ++PID_counter };
        processes[process_ID] = Process(process_ID, type, address);
        sendProcessToReadyQueue(process_ID);
    }
    else {
        cout << "\n\tError - Could not fit new process into memory\n" << endl;
    }
}

/**
 * Sends a given process to the appropriate ready-queue by
 * checking the process' type.
 * 
 * @param process_ID Process to send to ready-queue
 */
void OS::sendProcessToReadyQueue(PID process_ID) {
    if ( getProcessType( process_ID ) == ProcessType::Common ) {
        common_queue.push_back( process_ID );
    }
    else if ( getProcessType( process_ID ) == ProcessType::RealTime ) {
        RT_queue.push_back( process_ID );
    }
    else {
        /* Error... Invalid process send to ready-queue.
            This block should never be reached. */
    }

    updateCPU();
}

/**
 * Terminates currently running process. Deletes process from OS's set
 * of processes and frees the memory it was occupying.
 */
void OS::terminateCurrentProcess() {
    if ( processor.isRunning() ) {
        PID prev_process{ processor.currentProcessPID() };
        const MemoryBlock & prev_memory{ processes.at(prev_process).getMemoryBlock() };

        processor.finishRunningCurrentProcess();
        processes.erase( prev_process );
        memory.freeMemoryBlock( prev_memory );

        updateCPU();
    }
    else {
        cout << "\n\tError - No processes currently being executed\n" << endl;
    }
}

/**
 * Stops execution of current process and sends it to the back of the
 * ready-queue, and the next process is executed.
 */
void OS::executeNextProcess() {
    if ( processor.isRunning() ) {
        PID prev_process{ processor.currentProcessPID() };
        processor.finishRunningCurrentProcess();
        sendProcessToReadyQueue( prev_process );
    }
    else if ( ! RT_queue.empty() || ! common_queue.empty() ) {
        /* Potential error... should never reach this block
            unless manipulating ready-queues and CPU */
        updateCPU();
    }
    else {
        cout << "\n\tError - No processes to execute\n" << endl;
    }
}

/**
 * Updates the CPU to ensure it's running a process if the ready-queues
 * are not empty. This function will preempt a common process if there
 * is a real-time process to be executed. Otherwise it will first send
 * the available real-time processes to the CPU, and if there are none,
 * then it will send the common processes.
 */
void OS::updateCPU() {
    if ( processor.isRunning() ) {
        // Preempt common process if applicable
        if ( currentlyRunningProcessType() == ProcessType::Common && ! RT_queue.empty() ) {
            PID old_process{ processor.currentProcessPID() };
            common_queue.push_front( old_process );
            processor.runNewProcess( RT_queue.front() );
            RT_queue.pop_front();
        }
    }
    else {
        if ( ! RT_queue.empty() ) {
            processor.runNewProcess( RT_queue.front() );
            RT_queue.pop_front();
        }
        else if ( ! common_queue.empty() ) {
            processor.runNewProcess( common_queue.front() );
            common_queue.pop_front();
        }
    }
}

/**
 * Sends the CPU's currently running process to the corresponding
 * IO-queue. Check if the HDD # is valid and that the CPU is
 * running before manipulating any ready-queues or PIDs.
 * 
 * @param HDD_ID Hard drive # to send currently running process to
 */
void OS::sendCurrentProcesstoIOQueue(uint HDD_ID) {
    // Check if valid HDD #
    if ( hard_drives.size() > HDD_ID ) {
        if ( processor.isRunning() ) {
            PID IO_process{ processor.currentProcessPID() };

            processor.finishRunningCurrentProcess();
            hard_drives[HDD_ID].sentProcessToIOQueue( IO_process );

            updateCPU();
        }
        else {
            cout << "\n\tError - No processes currently being executed\n" << endl;
        }
    }
    else {
        cout << "\n\tError - Invalid hard drive ID #\n" << endl;
    }
}

/**
 * Sends a process from a HDD's IO-queue back to the ready-queue. Checks
 * that the HDD # is valid and that the HDD is currently serving a process.
 * 
 * @param HDD_ID Hard drive # to stop serving current process
 */
void OS::sendIOProcessToReadyQueue(uint HDD_ID) {
    // Check if valid HDD #
    if ( hard_drives.size() > HDD_ID ) {
        if ( hard_drives[HDD_ID].isServing() ) {
            PID IO_process{ hard_drives[HDD_ID].currentProcessPID() };
            hard_drives[HDD_ID].finishServingCurrentProcess();
            sendProcessToReadyQueue( IO_process );
        }
        else {
            cout << "\n\tError - No processes currently being served\n" << endl;
        }
    }
    else {
        cout << "\n\tError - Invalid hard drive ID #\n" << endl;
    }
}

void OS::printCPUData() const {
    cout << "\n\tPID\tTYPE\tSTATUS" << endl;

    // Output CPU's currently running process first if it's running
    if ( processor.isRunning() ) {
        PID process_ID{ processor.currentProcessPID() };
        string process_type{ getProcessType( process_ID ) == ProcessType::Common ? "Common" : "RT" };
        cout << '\t' << process_ID << '\t' << process_type << '\t' << "Running" << endl; 
    }

    // Output RT ready-queue
    for (PID process : RT_queue) {
        cout << '\t' << process << '\t' << "RT" << '\t' << "Waiting" << endl; 
    }
    
    // Output common ready-queue
    for (PID process : common_queue) {
        cout << '\t' << process << '\t' << "Common" << '\t' << "Waiting" << endl; 
    }

    cout << endl;
}

void OS::printIOData() const {
    cout << "\n\tPID\tHDD\tSTATUS" << endl;

    for (size_t i{0}; i < hard_drives.size(); ++i) {
        // Output HDD's current process first if it's serving
        if ( hard_drives[i].isServing() ) {
            PID process_ID{ hard_drives[i].currentProcessPID() };
            cout << '\t' << process_ID << '\t' << i << '\t' << "Serving" << endl;
        }

        // Output HDD's IO-queue
        for (PID process : hard_drives[i].getIOQueue()) {
            cout << '\t' << process << '\t' << i << '\t' << "Waiting" << endl;
        }
    }

    cout << endl;
}

void OS::printRAMData() const {
    cout << "\n\tPID\tM_START\tM_END" << endl;

    /* Output processes (in order of PID) and their
        starting and ending memory address blocks */
    for (const auto & process : processes) {
        const MemoryBlock & memory{ process.second.getMemoryBlock() };
        cout << '\t' << process.first << '\t' << memory.first << '\t' << memory.second << endl; 
    }

    cout << endl;
}

ProcessType OS::getProcessType(PID process_ID) const {
    if ( processes.count( process_ID ) ) {
        return processes.at( process_ID ).getProcessType();
    }
    else {
        return ProcessType::Invalid;
    }
}

ProcessType OS::currentlyRunningProcessType() const {
    return getProcessType( processor.currentProcessPID() );
}
