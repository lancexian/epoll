#ifndef _EX_H_
#define _EX_H_
#include <stdexcept>
#include <execinfo.h>
#include <string>

using namespace std;

namespace WJ
{
    class Exception : public exception
    {
        public:
            explicit Exception( const string &sBuffer );
            Exception( const string &sBuffer, int err );
            virtual ~Exception() throw();
            virtual const char* what() const throw();
            int getErrCode()
            {
                return _code;
            }
        private:
            void getBacktrace();
        private:
            string _buffer;
            int _code;
    };
}
#endif
