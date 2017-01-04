#ifndef _EPOLL_H_
#define _EPOLL_H_
#include <assert.h>
#include <sys/epoll.h>

namespace WJ
{
    class EPoll
    {
        public:
            EPoll( bool bEt = true );
            ~EPoll();
            /**
             * 生成句柄
             * */
            void create( int max_connections );
            /**
             * 添加句柄事件
             */
            void add( int fd, long long data, uint32_t event );
            /**
             * 修改句柄事件
             * */
            void mod( int fd, long long data, uint32_t event );
            /**
             * 删除句柄事件
             * */
            void del( int fd, long long data, uint32_t event );
            /**
             * 等待事件
             * return int返回有事件触发的事件数
             * */
            int  wait( int millsecond );
            /**
             * 获取被触发的事件
             * */
            struct epoll_event& get( int i )
            {
                assert( _pevs != 0 );
                return _pevs[i];
            }
        protected:
            /**
             * 控制函数，设置EPoll边缘触发
             * */
            void ctl( int fd, long long data, uint32_t events, int op );
        protected:
            int _iEpollId;
            int _max_connections;
            struct epoll_event *_pevs;
            bool _et;
    };
}
#endif
