/// @author Jonathan Kelaty
/// @file CS OS Home Project - DataTypes.h
/// @date 2020-04-14
/// @brief Declares enumeration and typedefs used across all OS classes.

#ifndef DATA_TYPES_H_
#define DATA_TYPES_H_

#include <deque>
#include <utility>

enum class ProcessType { RealTime, Common, Invalid };

typedef unsigned int uint;
typedef unsigned int PID;

typedef std::deque<PID> ReadyQueue;
typedef std::pair<uint, uint> MemoryBlock;

#endif // DATA_TYPES_H_
