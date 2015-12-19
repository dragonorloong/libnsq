/*************************************************************************
	> File Name: test.cpp
	> Author: 
	> Mail: 
	> Created Time: 2015年10月28日 星期三 22时27分59秒
 ************************************************************************/

#include<iostream>
using namespace std;
#include "event2/bufferevent.h"
#include "event2/event.h"

void timeout_cb(int fd, short event, void *params)
{
        puts("111");

}

int main()
{
        struct event_base *base = event_base_new();
        struct event *timeout = NULL;
        struct timeval tv = {1, 0};
        timeout = event_new(base, -1, EV_PERSIST, timeout_cb, NULL);
        evtimer_add(timeout, &tv);
        event_base_dispatch(base);
        evtimer_del(timeout);

        return 0;

}
