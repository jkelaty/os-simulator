/// @author Jonathan Kelaty
/// @file CS OS Home Project - RAM.h
/// @date 2020-04-14
/// @brief RAM class implementation. Manages only the available (free)
/// memory in our simulated system. It will find an available memory
/// block (if possible) given a specified size, and will free a memory
/// block previously used by a process when it terminated. Implemented
/// as contiguous first-fit memory allocation.

#ifndef RAM_H_
#define RAM_H_

#include <set>
#include <iterator>

#include "DataTypes.h"

using std::set;
using std::advance;

/*
 * () operator overload for comparing MemoryBlock (std::pair<uint, uint>)
 * objects in std::set<MemoryBlock>. Compares only the first members (beginning
 * address of address block) of each pair to order objects in increasing order.
 */
struct memory_compare {
    bool operator()(const MemoryBlock & a, const MemoryBlock & b) const {
        return a.first < b.first;
    }
};

/***********************************
 * 
 * Random Access Memory (RAM) Class
 * 
 ***********************************/

class RAM {

    public:
        RAM() = delete;

        RAM(uint size) :
            available_memory{ {0, size - 1} } { /* Intentionally empty */ }

        /**
         * Finds an available memory block that fits a process of a desired
         * size. It will scan the free memory blocks in order (contiguous
         * first-fit) until it finds a big enough block. The previous block
         * is then removed and then resized and put back in the set (if new
         * size is non-zero). If there is no space to fit the process, an
         * invalid location is returned (first > second). The calling function
         * is responsible for checking if this memory location is valid.
         * 
         * Notes:
         *  Memory block of size 1 has equal starting and ending addresses.
         *  Size can be found by: second - first + 1
         * 
         * @param size Size of process to fit into RAM (must be > 0)
         * 
         * @return Valid memory location if it can be fit, else {1,0}
         */
        MemoryBlock findAvailableMemoryBlock(uint size) {
            for (const MemoryBlock & memory : available_memory) {
                // Check if memory block is large enough
                if ( (memory.second - memory.first + 1) >= size ) {
                    // Reserve memory block for process and resized free memory
                    MemoryBlock reserved_memory{ memory.first, memory.first + size - 1 };
                    MemoryBlock free_memory{ memory.first + size, memory.second };

                    // Erase previous free memory
                    available_memory.erase( memory );
                    
                    // If resized memory is valid, insert
                    if ( free_memory.first <= free_memory.second ) {
                        available_memory.insert( free_memory );
                    }

                    return reserved_memory;
                }
            }

            return {1,0}; // Invalid memory block (first > second)
        }

        /**
         * Inserts freed memory block into RAM. Then scans all available memory
         * to merge contiguous free memory blocks. There will be a maximum of 3
         * memory blocks to be merged each time this function is called. See
         * the illustration below to see how the iterators move after each iteration:
         * 
         * { ..., M1, M2, M3, M4, ... }  ---> { ..., M1, M2, M3, M4, ... }
         *        ^   ^   ^                              ^   ^   ^
         *        1   2   3                              1   2   3
         * 
         * We use 3 iterators to scan the set of free memory blocks until we find
         * that the first two iterators can be merged. If they can, we also check
         * if the last two can be merged. We then create a resized memory block
         * and delete the previous memory blocks and insert the resized one.
         * 
         * @param free_memory Memory block to be added to available memory
         */
        void freeMemoryBlock(const MemoryBlock & free_memory) {
            // Insert freed memory
            available_memory.insert( free_memory );

            // We can only merge if we have > 1 available memory blocks
            if ( available_memory.size() > 1 ) {
                // Initialize and advance our iterators
                auto iter1{ available_memory.begin() };
                auto iter2{ available_memory.begin() };
                auto iter3{ available_memory.begin() };

                advance(iter2, 1);
                advance(iter3, 2);

                // Merge flags
                bool merge12{ false };
                bool merge23{ false };

                while ( iter1 != available_memory.end() && iter2 != available_memory.end() ) {
                    if ( (iter1->second + 1) == iter2->first ) {
                        merge12 = true;
                    }
                    else {
                        advance(iter1, 1);
                    }

                    if ( iter3 != available_memory.end() ) {
                        if ( merge12 && (iter2->second + 1) == iter3->first ) {
                            merge23 = true;
                        }
                        else {
                            advance(iter3, 1);
                        }
                    }

                    if ( merge12 || merge23 ) {
                        // Initialize resized free memory block
                        uint new_memory_start{ merge12 ? iter1->first  : iter2->first  };
                        uint new_memory_end  { merge23 ? iter3->second : iter2->second };

                        MemoryBlock new_memory{ new_memory_start, new_memory_end };

                        // Delete previous memory if merge flag is set
                        if ( merge12 ) {
                            available_memory.erase( iter1 );
                        }
                        
                        if ( merge23 ) {
                            available_memory.erase( iter3 );
                        }

                        available_memory.erase( iter2 );

                        // Insert resized memory block
                        available_memory.insert( new_memory );

                        break;
                    }
                    else {
                        advance(iter2, 1);
                    }
                }
            }
        }

    private:
        set<MemoryBlock, memory_compare> available_memory;

};

#endif // RAM_H_
