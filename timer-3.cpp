#include <stdio.h>
#include <time.h>
#include <sys/netmgr.h>
#include <sys/neutrino.h>
#include <signal.h>


void thread_func(union sigval sig) {
   int value = sig.sival_int;
   printf("action_%d\n", value);
}

int main() {
   struct sigevent         event1;
    struct sigevent         event2;
    struct sigevent         event3;
    struct itimerspec       itime1;
    struct itimerspec       itime2;
    struct itimerspec       itime3;
    timer_t                 timer_id1;
    timer_t                 timer_id2;
    timer_t                 timer_id3;

     int value = 1;
    SIGEV_THREAD_INIT(&event1, thread_func, value, NULL);
    value = 2;
    SIGEV_THREAD_INIT(&event2, thread_func, value, NULL);
    value = 3;
    SIGEV_THREAD_INIT(&event3, thread_func, value, NULL);

   timer_create(CLOCK_REALTIME, &event1, &timer_id1);
    timer_create(CLOCK_REALTIME, &event2, &timer_id2);
    timer_create(CLOCK_REALTIME, &event3, &timer_id3);

   itime1.it_value.tv_sec = 0;
    itime1.it_value.tv_nsec = 500000000;
    itime1.it_interval.tv_sec = 0;
    itime1.it_interval.tv_nsec = 500000000;
    timer_settime(timer_id1, 0, &itime1, NULL);

    itime2.it_value.tv_sec = 0;
    itime2.it_value.tv_nsec = 700000000;
    itime2.it_interval.tv_sec = 0;
    itime2.it_interval.tv_nsec = 700000000;
    timer_settime(timer_id2, 0, &itime2, NULL);

    itime3.it_value.tv_sec = 1;
    itime3.it_value.tv_nsec = 0;
    itime3.it_interval.tv_sec = 1;
    itime3.it_interval.tv_nsec = 0;
    timer_settime(timer_id3, 0, &itime3, NULL);

    while(getchar() != '\n'){}
}