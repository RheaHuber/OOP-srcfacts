/*
    refillBuffer.cpp

    Implementation file for refillBuffer function
*/

#include "refillBuffer.hpp"
#include <string>

#if !defined(_MSC_VER)
#include <sys/uio.h>
#include <unistd.h>
#define READ read
#else
#include <BaseTsd.h>
#include <io.h>
typedef SSIZE_T ssize_t;
#define READ _read
#endif

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
int refillBuffer(std::string::const_iterator& cursor, std::string::const_iterator& cursorEnd, std::string& buffer) {

    // number of unprocessed characters [cursor, cursorEnd)
    size_t unprocessed = std::distance(cursor, cursorEnd);

    // move unprocessed characters, [cursor, cursorEnd), to start of the buffer
    std::copy(cursor, cursorEnd, buffer.begin());

    // reset cursors
    cursor = buffer.begin();
    cursorEnd = cursor + unprocessed;

    // read in whole blocks
    ssize_t readBytes = 0;
    while (((readBytes = READ(0, static_cast<void*>(buffer.data() + unprocessed),
        std::distance(cursorEnd, buffer.cend()))) == -1) && (errno == EINTR)) {
    }
    if (readBytes == -1)
        // error in read
        return -1;
    if (readBytes == 0) {
        // EOF
        cursor = buffer.cend();
        cursorEnd = buffer.cend();
        return 0;
    }

    // adjust the end of the cursor to the new bytes
    cursorEnd += readBytes;

    return readBytes;
}
