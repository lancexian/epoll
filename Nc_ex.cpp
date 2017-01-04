#include "../include/ex.h"

namespace WJ
{
    Exception::Exception( const string &sBuffer ):_buffer(sBuffer),_code(0)
    {

    }
    Exception::Exception( const string &sBuffer, int err)
    {
        _buffer = sBuffer;
        _code = err;
    }
    Exception::~Exception() throw()
    {
    }
    const char * Exception::what() const throw()
    {
        return _buffer.c_str();
    }
    void Exception::getBacktrace()
    {
        void * array[64];
        int nSize = backtrace(array, 64);
        char ** symbols = backtrace_symbols(array, nSize);

        for (int i = 0; i < nSize; i++)
        {
            _buffer += symbols[i];
            _buffer += "\n";
        }
        free(symbols);
    }
}
