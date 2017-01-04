#ifndef _SOCKET_H_
#define _SOCKET_H_
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h> 
#include <sys/socket.h>
#include <sys/un.h>
#include <errno.h>
#include <strings.h>
#include <arpa/inet.h>
#include <assert.h>
#include <string>
#include <netdb.h>
#include <unistd.h>
#include <fcntl.h>
#include <arpa/inet.h>

#include "ex.h"

using namespace std;

namespace WJ
{
    struct Socket_Exception : public Exception
    {
        Socket_Exception( const string &buffer ):Exception( buffer ){};
        Socket_Exception( const string &buffer, int err ):Exception( buffer, err ){};
        ~Socket_Exception() throw() {};
    };
    class Socket
    {
        public:
            Socket();
            ~Socket();
            /**
             * 初始化
             * */
            void init( int fd, bool bOwner, int iDomain=AF_INET );
            /**
             * 设置是否拥有fd
             * */
            void setOwner( bool bOwner );
            /**
             * 设置套接字类型
             * */
            void setDomain( int iDomain )
            {
                _iDomain = iDomain;
            }
            /**
             *
             * */
            void parseAddr( const string &sAddr, struct in_addr &stSinAddr );
            /**
             * 生成socket，如果已经存在以前的socket，则释放掉，生成新的
             * */
            void createSocket( int iSocketType=SOCK_STREAM, int iDomain=AF_INET );
            /**
             * 获取socket句柄
             * */
            int getfd() const
            {
                return _sock;
            }
            /**
             * socket是否有效
             * */
            bool isValid() const
            {
                return _sock != INVALID_SOCKET;
            }
            /**
             * 关闭socket
             * */
            void close();
            /**
             * 修改socket选项
             * */
            int setSockOpt( int opt, const void *pvOptVal, socklen_t optLen, int leve=SOL_SOCKET);
            /**
             * 获知socket选项
             * */
             int getSockOpt( int opt, void *pvOptVal, socklen_t &optLen, int leve = SOL_SOCKET );
             /**
              * accept
              * */
             int accept( Socket &tSocket, struct sockaddr *pstServerAddr, socklen_t &iSocketLen );
             /**
              * 绑定,对AF_INET的socket有效
              * */
             void bind( const string &sServerAddr, int iPort );
             /**
              *
              * */
             void bind( const char *sPathName );
             /**
              * 绑定域套接字，对AF_INET的socket有效(同步连接)
              * */
             void connect( const string &sServerAddr, unsigned short iPort );
             /**
              * 绑定域套接字，对AF_INET的socket有效(同步连接)
              * */
             void connect( const char *sPathName );
             /**
              *
              * */
             int connectNoThrow( const string &sServerAddr, unsigned short iPort );
             /**
              *
              *
              * */
             int connectNoThrow( struct sockaddr *addr );
             /**
              *
              *
              * */
             int connectNoThrow( const char *sPathName );
             /**
              *
              * */
             void listen( int connBackLog );
             /**
              *
              * */
             int recv( void *pvBuf, size_t iLen, int iFlag=0 );
             /**
              *
              * */
             void send( const void *pvBuf, size_t iLen, int iFlag=0 );
             /**
              *
              * */
             int recvfrom( void *pvBuf, size_t iLen, string &sFromAddr, unsigned short &iFromPort, int iFlag=0 );
             /**
              *
              * */
             int recvfrom( void *pvBuf, size_t iLen, struct sockaddr *pstFromAddr, socklen_t &iFromLen, int iFlag=0 );
             /**
              * pvBuf      发送buffer
              * iLen       buffer长度
              * sToAddr    服务端ip地址，如果sToAddr为空，则UDP广播
              * iToPort    服务端端口
              * */
             int sendto( const void* pvBuf, size_t iLen, const string &sToAddr, unsigned short &iToPort, int iFlag=0 );
             /**
              * pvBuf      发送buffer
              * iLen       buffer长度
              * pstToAddr  服务端地址
              * iToLen     pstToAddr长度
              * iFlags     标示
              * */
             int sendto( const void* pvBuf, size_t iLen, struct sockaddr *pstToAddr, socklen_t iToLen, int iFlag=0 );
             /**
              *
              * */
             void shutdown( int iHow );
            /**
             * 获取接收buffer大小
             * */
            int getRecvBufferSize();
            /**
             * 设置接收buffer大小
             * */
            void setRecvBufferSize( int sz );
            /**
             * 获取发送buffer大小
             * */
            int getSendBufferSize();
            /**
             * 设置发送buffer大小
             * */
            void setSendBufferSize( int sz );
            /**
             * 设置socket方式
             * */
            void setblock( bool bBlcok );
            static void setblock( int fd, bool bBlcok );
            /**
             * 绑定
             * */
            void bind( struct sockaddr *pstBindAddr, socklen_t iAddrLen );
        protected:
            /**
             * 连接其他服务
             * */
            int connect( struct sockaddr *pstServerAddr, socklen_t serverLen );
        protected:
            static const int INVALID_SOCKET = -1;
            /**
             * Socket句柄
             * */
            int _sock;
            bool _bOwner;
            /**
             * Socket的类型 
             * */
            int _iDomain;
        private:
    };
}
#endif
