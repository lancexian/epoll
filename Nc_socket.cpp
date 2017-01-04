#include "../include/socket.h"

namespace WJ
{
    Socket::Socket()
    {
    }
    Socket::~Socket()
    {
        if( _bOwner )
        {
            close();
        }
    }
    void Socket::init( int fd, bool bOwner, int iDomain )
    {
        if( bOwner )
        {
            close();
        }

        _sock = fd;
        _bOwner = bOwner;
        _iDomain = iDomain;
    }
    void Socket::createSocket( int iSocketType, int iDomain )
    {
        assert( iSocketType==SOCK_STREAM || iSocketType==SOCK_DGRAM );
        close();

        _iDomain = iDomain;
        _sock = socket( iDomain, iSocketType , 0 );

        if( _sock<0 )
        {
            _sock = INVALID_SOCKET;
        }
    }
    void Socket::close()
    {
        if( _sock != INVALID_SOCKET )
        {
            ::close( _sock );
            _sock = INVALID_SOCKET;
        }
    }
    int Socket::setSockOpt( int opt, const void *pvOptVal, socklen_t optLen, int leve )
    {
        return setsockopt( _sock, leve, opt, pvOptVal, optLen );
    }
    int Socket::getSockOpt( int opt, void *pvOptVal, socklen_t &optLen, int leve )
    {
        return getsockopt( _sock, leve, opt, pvOptVal, &optLen );
    }
    int Socket::accept( Socket &tSocket, struct sockaddr *pstServerAddr, socklen_t &iSocketLen )
    {
        assert( tSocket._sock == INVALID_SOCKET );

        int fd;
        while( (fd=::accept(_sock, pstServerAddr, &iSocketLen) ) < 0 && ( errno==EINTR));

        tSocket._sock = fd;
        tSocket._iDomain = _iDomain;

        return tSocket._sock;
    }
    void Socket::parseAddr( const string &sAddr, struct in_addr &stSinAddr )
    {
        int iRet = inet_pton( AF_INET, sAddr.c_str(), &stSinAddr);
        if( iRet < 0 )
        {
            throw Socket_Exception( "[Socket::parseAddr] inet_pton error", errno );
        }
        else if( iRet==0 )
        {
            struct hostent stHostent;
            struct hostent *pstHostent;
            char buff[1024] = "\0";
            int iError;

            gethostbyname_r( sAddr.c_str(), &stHostent, buff, sizeof(buff), &pstHostent, &iError );
            if( pstHostent==NULL )
            {
                throw Socket_Exception( "[Socke::parseAddr] gethostbyname_r error :" + string( hstrerror(iError)) );
            }
            else
            {
                stSinAddr = *( struct in_addr *)pstHostent->h_addr;
            }
        }
    }
    void Socket::bind( const string &sServerAddr, int iPort )
    {
        assert( _iDomain== AF_INET );

        struct sockaddr sBindAddr;
        bzero( &sBindAddr, sizeof( sBindAddr ) );

        struct sockaddr_in *p = (struct sockaddr_in *)&sBindAddr;

        p->sin_family = _iDomain;
        p->sin_port = htons( iPort );

        if( sServerAddr == "")
        {
            p->sin_addr.s_addr = htonl( INADDR_ANY );
        }
        else
        {
            parseAddr( sServerAddr, p->sin_addr );
        }

        try
        {
            bind( &sBindAddr, sizeof( sBindAddr ) );
        }
        catch ( ... )
        {
            throw Socket_Exception( "[Socke::bind] bind " + sServerAddr + ":" + " error.", errno );
        }
    }
    void Socket::bind( struct sockaddr *pstBindAddr, socklen_t iAddrLen )
    {
        int iResumeAddr = 1;

        setSockOpt( SO_REUSEADDR, (const void*)&iResumeAddr, sizeof(int), SOL_SOCKET );

        if( ::bind( _sock, pstBindAddr, iAddrLen ) < 0 )
        {
            throw Socket_Exception( "[Socket::bind] bind error" , errno );
        }
    }
    int Socket::connect( struct sockaddr *pstServerAddr, socklen_t serverLen )
    {
        return ::connect( _sock, pstServerAddr, serverLen );
    }
    void  Socket::connect( const char *sPathName )
    {
        int iRet = connectNoThrow( sPathName );

        if( iRet < 0 )
        {
            throw Socket_Exception( "[Socket::connect] connect error" , errno );
        }
    }
    int Socket::connectNoThrow( const char *sPathName )
    {
        assert( _iDomain==AF_LOCAL );

        struct sockaddr_un stServerAddr;
        bzero( &stServerAddr, sizeof( struct sockaddr_un ) );

        stServerAddr.sun_family = _iDomain;
        strncpy( stServerAddr.sun_path, sPathName, sizeof( stServerAddr.sun_path ) );

        return connect( (struct sockaddr *)&stServerAddr, sizeof( stServerAddr ) );
    }
    void Socket::listen( int iConnBackLog )
    {
        if( ::listen( _sock, iConnBackLog ) )
        {
            throw Socket_Exception( "[Socket::listen] listen error" , errno );
        }
    }
    int Socket::recv( void *pBuff, size_t iLen, int iFlag )
    {
        return ::recv( _sock, pBuff, iLen, iFlag );
    }
    void Socket::send( const void *pBuff, size_t iLen, int iFlag )
    {
        ::send( _sock, pBuff, iLen, iFlag );
    }

    int Socket::recvfrom( void *pBuff, size_t iLen, string &sFromAddr, unsigned short &iFromPort, int iFlags )
    {
        struct sockaddr stFromAddr;
        socklen_t iFromLen = sizeof( struct sockaddr );
        struct sockaddr_in *p = (struct sockaddr_in *)&stFromAddr;

        bzero( &stFromAddr, sizeof(struct sockaddr) );

        int iByte = recvfrom( pBuff, iLen, &stFromAddr, iFromLen, iFlags );

        if( iByte >= 0 )
        {
            char sAddr[INET_ADDRSTRLEN] = "\0";
            inet_ntop( _iDomain, &p->sin_addr, sAddr, sizeof(sAddr) );

            sFromAddr = sAddr;
            iFromPort = ntohs( p->sin_port );
        }
        return iByte;
    }
    int Socket::recvfrom( void *pBuff, size_t iLen, struct sockaddr *pstFromAddr, socklen_t &iFromLen, int iFlags )
    {
        return ::recvfrom( _sock, pBuff, iLen, iFlags, pstFromAddr, &iFromLen );
    }
    int Socket::sendto( const void *pvBuf, size_t iLen, const string &sToAddr, unsigned short &iToPort, int iFlags )
    {
        struct sockaddr stToAddr;
        struct sockaddr_in *p = (struct sockaddr_in *)&stToAddr;

        bzero( &stToAddr, sizeof( struct sockaddr ) );

        p->sin_port = _iDomain;

        if( sToAddr == "" )
        {
            p->sin_addr.s_addr = htonl(INADDR_BROADCAST);
        }
        else
        {
            parseAddr( sToAddr, p->sin_addr );
        }

        p->sin_port = htons( iToPort );

        return sendto( pvBuf, iLen, &stToAddr, sizeof(stToAddr), iFlags );
    }
    int Socket::sendto( const void *pvBuf, size_t iLen, struct sockaddr *pstToAddr, socklen_t iToLen, int iFlags )
    {
        return ::sendto( _sock, pvBuf, iLen, iFlags, pstToAddr, iToLen );
    }
    void Socket::shutdown( int iHow )
    {
        if( ::shutdown( _sock, iHow ) < 0 )
        {
            throw Socket_Exception( "[Socket::shutdown] shutdown error", errno );
        }
    }
    void Socket::setblock( bool bBlcok )
    {
        assert( _sock == INVALID_SOCKET );
        setblock( _sock, bBlcok );
    }
    void Socket::setblock( int fd, bool bBlcok )
    {
        int iVal = 0;

        if( ( iVal = fcntl( fd, F_GETFL ) ) == -1 )
        {
            throw Socket_Exception( "[Socket::setblock] fcntl [F_GETEL] error", errno );
        }

        if( !bBlcok )
        {
            iVal |= O_NONBLOCK;
        }
        else
        {
            iVal &= ~O_NONBLOCK;
        }

        if( fcntl( fd, F_SETFL , iVal ) == -1 )
        {
            throw Socket_Exception( "[Socket::setblock] fcntl [F_SETEL] error", errno );
        }
    }
    int Socket::getRecvBufferSize()
    {
        int sz;
        socklen_t len = sizeof(sz);

        if( ( getSockOpt( SO_RCVBUF, (void*)&sz, len, SOL_SOCKET ) == -1 ) && (len != sizeof(int)))
        {
            throw Socket_Exception("[Socket::getRecvBufferSize] error", errno);
        }

        return sz;
    }
    void Socket::setRecvBufferSize( int sz )
    {
        if( ( setSockOpt( SO_RCVBUF, (char*)&sz, int(sizeof(int)), SOL_SOCKET ) == -1 ) )
        {
            throw Socket_Exception("[Socket::setRecvBufferSize] error", errno);
        }
    }
    int Socket::getSendBufferSize()
    {
        int sz;
        socklen_t len = sizeof(sz);

        if( ( getSockOpt( SO_SNDBUF, (void*)&sz, len, SOL_SOCKET ) ) == -1 && (len != sizeof(int)))
        {
            throw Socket_Exception("[Socket::getSendBufferSize] error", errno);
        }
        return sz;
    }
    void Socket::setSendBufferSize( int sz )
    {
        if( setSockOpt( SO_SNDBUF, (char*)&sz, int(sizeof(int)), SOL_SOCKET ) == -1 )
        {
            throw Socket_Exception("[Socket::setSendBufferSize] error", errno);
        }
    }
}
