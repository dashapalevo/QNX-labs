#include <stdio.h>
#include <time.h>
#include <sys/netmgr.h>
#include <sys/neutrino.h>
#include <signal.h>

#define MY_PULSE_CODE   _PULSE_CODE_MINAVAIL

typedef union {
        struct _pulse   pulse;
        /* your other message structures would go here too */
} my_message_t;

main()
{
   struct sigevent         event1;
    struct sigevent         event2;
    struct sigevent         event3;
    struct itimerspec       itime1;
    struct itimerspec       itime2;
    struct itimerspec       itime3;
    timer_t                 timer_id1;
    timer_t                 timer_id2;
    timer_t                 timer_id3;
   int                     chid;
   int                     rcvid;
   my_message_t            msg;

   chid = ChannelCreate(0);

    int coid = ConnectAttach(ND_LOCAL_NODE, 0, chid, _NTO_SIDE_CHANNEL, 0);
   int value = 1;
   SIGEV_PULSE_INIT(&event1, coid, getprio(0), MY_PULSE_CODE, value);

  value = 2;
   SIGEV_PULSE_INIT(&event2, coid, getprio(0), MY_PULSE_CODE, value);

  value = 3;
   SIGEV_PULSE_INIT(&event3, coid, getprio(0), MY_PULSE_CODE, value);

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

   while(true) {
       rcvid = MsgReceive(chid, &msg, sizeof(msg), NULL);
       if (rcvid == 0) { /* we got a pulse */
            if (msg.pulse.code == MY_PULSE_CODE) {
                printf("action_%d\n",  msg.pulse.value);
            }
       } /* else other messages ... */
   }
}