#include <stdio.h>
#include <time.h>
#include <sys/netmgr.h>
#include <sys/neutrino.h>
#include <signal.h>


void catch_sig(int signo) {
    printf("action_%d\n", signo - 47);
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

    int signal1 = 48;
    int signal2 = 49;
    int signal3 = 50;

    sigset_t set;
    sigemptyset(&set);
    sigaddset(&set, signal1);
    sigaddset(&set, signal2);
    sigaddset(&set, signal3);

    struct sigaction act;
    act.sa_handler = &catch_sig;
    act.sa_flags = 0;
    act.sa_mask = set;
    sigaction(signal1, &act, NULL);
    sigaction(signal2, &act, NULL);
    sigaction(signal3, &act, NULL);

    int signo1 = signal1;
    int signo2 = signal2;
    int signo3 = signal3;
    SIGEV_SIGNAL_INIT(&event1, signo1);
    SIGEV_SIGNAL_INIT(&event2, signo2);
    SIGEV_SIGNAL_INIT(&event3, signo3);

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