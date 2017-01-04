#ifndef _EPOLL_SERVER_H_
#define _EPOLL_SERVER_H_
#include <iostream>
#include "Nc_epoll.h"
using namespace std;

namespace WJ
{
    class EpollServer
    {
        public:
            EpollServer();
            ~EpollServer();
            void createEpoll();
        protected:
        private:
            EPoll _epoller;
            bool _createEpoll;
            bool _Terminate;
    };
}
#endif
