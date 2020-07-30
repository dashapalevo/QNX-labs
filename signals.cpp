#include <stdlib.h>
#include <stdio.h>
#include <signal.h>
#include <unistd.h>


void handler(int signo, siginfo_t* info, void* context) {
   printf("signo=%d, code=%d, info=%d\n",
           signo, info->si_code, info->si_value.sival_int);
}

int main() {
   int signal1 = 48,
       signal2 = 49,
       value = 0;
   
   sigset_t set;
   sigemptyset(&set);
   sigaddset(&set, signal1);
   sigaddset(&set, signal2);

   sigprocmask(SIG_BLOCK, &set, NULL);
   
   pid_t pid;

   if(pid = fork() != 0) {	
   /* процесс – родитель */
      printf("Parent (pid=%d) started\n", pid);

      for (int i = 0; i < 5; ++i)
      {
         siginfo_t info;
         info.si_value.sival_int = value;
         sigqueue(0, signal1, info.si_value);
         printf("signal1 # %d value %d is sent\n",  signal1, info.si_value);


         sigqueue(0, signal2, info.si_value);
         printf("signal2 # %d value %d is sent\n",  signal2, info.si_value);
         
         value++;
         usleep(100000);
      }

      printf("Parent (pid=%d) finished\n", pid);
   }

   /* дочерний процесс */
   else {   
      printf("Child (pid=%d) started\n", pid);
      struct sigaction act;
      act.sa_sigaction = &handler;
      act.sa_flags = SA_SIGINFO;
      act.sa_mask = set;
      
      sigaction(signal1, &act, NULL);

      sigaction(signal2, &act, NULL);

      printf("Child (pid=%d): signal unblocked\n", pid);
      sigprocmask(SIG_UNBLOCK, &set, NULL);

      while(getchar() != 13){

      }
      printf("Child (pid=%d) finished\n", pid);
   }
}
