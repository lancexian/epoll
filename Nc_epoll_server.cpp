#include "../include/epoll_server.h"

namespace WJ
{
    EpollServer::EpollServer():_Terminate(false),_createEpoll(false)
    {
        
    }
    EpollServer::~EpollServer()
    {
    }
    void EpollServer::createEpoll()
    {
        if( !_createEpoll )
        {
        }
    }
}
