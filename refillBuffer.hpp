/*
    refillBuffer.hpp

    Include file for the refillBuffer function
*/

#ifndef INCLUDED_REFILLBUFFER_HPP
#define INCLUDED_REFILLBUFFER_HPP

#include <string>

/*
    Refill the buffer preserving the unused data.
    Current content [cursor, cursorEnd) is shifted left and new data
    appended to the rest of the buffer.

    @param[in,out] cursor Iterator to current position in buffer
    @param[in, out] cursorEnd Iterator to end of buffer for this read
    @param[in, out] buffer Container for characters
    @return Number of bytes read
    @retval 0 EOF
    @retval -1 Read error
*/
int refillBuffer(std::string::const_iterator& cursor, std::string::const_iterator& cursorEnd, std::string& buffer);

#endif
