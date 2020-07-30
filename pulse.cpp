#include <stdlib.h>

#include <pthread.h>
#include <stdio.h>
#include <iostream>
#include <unistd.h>
#include <math.h>


#include <sys/neutrino.h> 


const int NUMBER_OF_CLIENTS = 3;

int chid;
int chid_pulse;

int chid_clients[NUMBER_OF_CLIENTS];
int chid_pulse_clients[NUMBER_OF_CLIENTS];

pthread_barrier_t barrier;

typedef struct {
  int from_id;
  int type;
  int to_id;
  char data[255];
} message;


using namespace std;

struct Attr
{
  int *flag;
  int id;
  
};

void* serverPulse(void* data){

  Attr *data1 = (Attr*) data;
  int *flag = data1->flag;

  chid_pulse = ChannelCreate(0); //Создание канала для взаимодействия с клиентами
  if(chid_pulse == -1){
    printf("[Server] Error create channel\n");
    pthread_exit ((void *) 1);
  }
  pthread_barrier_wait(&barrier);

  struct _pulse msg;
	int rcvid;
  while (*flag == 1){
      
      rcvid = MsgReceivePulse(chid_pulse, &msg, sizeof(msg), NULL); //Блокировка сервера пока не пришло сообщение от клиента
      	//cout << "==Debug server pulse recive   to " << msg.value.sival_int << endl;
	
      if(rcvid == -1){
        printf("[Server] Error receive message\n");
        continue;
      }
      int coid = ConnectAttach(0, getpid(), chid_pulse_clients[msg.value.sival_int], 0, 0);
      rcvid = MsgSendPulse(coid, 1, 1, 1);
      cout << "==Debug server pulse send   to " << msg.value.sival_int << endl;
      

  }
}


void* server(void* data){
  cout << "--Server start" << endl;

  int rcvid, to_id;
  message rcmsg;

  Attr *data1 = (Attr*) data;
  int *flag = data1->flag;

  chid = ChannelCreate(0); //Создание канала для взаимодействия с клиентами
  if(chid == -1){
    printf("[Server] Error create channel\n");
    pthread_exit ((void *) 1);
  }


  pthread_t serverPulse_pid;
  pthread_create(&serverPulse_pid, NULL, &serverPulse, data1);

  pthread_barrier_wait(&barrier);

  while (*flag == 1){
      
      rcvid = MsgReceive(chid, &rcmsg, sizeof(rcmsg), NULL); //Блокировка сервера пока не пришло сообщение от клиента
      if(rcvid == -1){
        printf("[Server] Error receive message\n");
        continue;
      }
      
      //cout << "==Debug server recive  id "<< rcmsg.from_id<<" client send to " << rcmsg.to_id << endl;
	
      message noup;
      MsgReply(rcvid, 0, &noup, sizeof(noup));
     
     
      message rmsg;
      int coid = ConnectAttach(0, getpid(), chid_clients[rcmsg.to_id], 0, 0);
      
 
      int msg_result = MsgSend(coid, &rcmsg, sizeof(rcmsg), &rmsg,    sizeof(rmsg));
      
      //cout << "==Debug server res " << msg_result <<endl;
      
      	//cout << "==Debug server send from id "<< rcmsg.from_id<<"  to " << rcmsg.to_id << endl;


  }
  void *res1;
  pthread_join(serverPulse_pid, (void**) &res1); 

  cout << "--Server end" << endl;

  pthread_exit ((void *) 1);
}

void* client_listener(void* data){
  
  
  message msg, rmsg;
  int rcvid;
  
  Attr *data1 = (Attr*) data;
  int *flag = data1->flag;
  int id = data1->id;
  
  cout << "++Client listen start # " << id << endl << flush;

  
  chid_clients[id] = ChannelCreate(0);
  int channelid = chid_clients[id];

  

  int coid;
  coid = ConnectAttach(0, getpid(), chid_pulse, 0, 0); //Client runs

  pthread_barrier_wait(&barrier);

  while (*flag == 1){
      
      
      rcvid = MsgReceive(chid_clients[id], &msg, sizeof(msg), NULL); //прослушиваем соеденение с сервером
      if(rcvid == -1){
        printf("++Client Error receive message\n");
        continue;
      }
      
      message noup;
      MsgReply(rcvid, 0, &noup, sizeof(noup));

      cout << "++Client #" << id << "recive message from #" << msg.from_id << "text:"
      << msg.data << endl  << flush;;


      rcvid = MsgSendPulse(coid, 1, 1, msg.from_id);

  }

  

  return EXIT_SUCCESS;
}

void* client(void* data){
 
	
  Attr *data1 = (Attr*) data;
  int *flag = data1->flag;
  int id = data1->id;
  
   cout << "++Client start # " << id << endl << flush;

  message smsg, rmsg;  
  int msg_result;
  int rcvid;
  struct _pulse msg;

  int coid;
  coid = ConnectAttach(0, getpid(), chid, 0, 0);//Создаем соединение к каналу сервера
  if(coid == -1){
    printf("[Client %d] Connection error\n", id);
  }

  pthread_t listener_pid;
  pthread_create(&listener_pid, NULL, &client_listener, data1);

  chid_pulse_clients[id] = ChannelCreate(0);
  int channelid = chid_pulse_clients[id];
 
  pthread_barrier_wait(&barrier);

  while (*flag == 1){
    
    
    smsg.from_id = id;
    smsg.to_id = (id+1) % NUMBER_OF_CLIENTS;
    smsg.type = 1;
    char data[255];

    usleep(id *1500000 + 4000000);

	//cout << "==Debug id "<< id<<" client send to " << smsg.to_id << endl;
		cout << "++Client #" << id << "send message to #" << smsg.to_id << "text:"
      <<smsg.data << endl  << flush;	
    msg_result = MsgSend(coid, &smsg, sizeof(smsg), &rmsg,    sizeof(rmsg)); //отправляем сообщение

    rcvid = MsgReceive(channelid, &msg, sizeof(msg), NULL); //ожидаем импульс с ответом
    
    cout << "++Client #" << id << "recive pulse" << endl  << flush;;

  }
  void *res1;
  pthread_join(listener_pid, (void**) &res1); 

  cout << "++Client end" << endl;

  pthread_exit ((void *) 1);
}


int main(int argc, char const *argv[]){
  pthread_t thread1 ; //объявляем нить(поток)

  pthread_t threads[NUMBER_OF_CLIENTS];

  int flag = 1; //иницилизируем флаги завершения цикла в функции потока

// NUMBER_OF_CLIENTS*2 + 2
  pthread_barrier_init(&barrier, NULL, NUMBER_OF_CLIENTS*2 + 2);


  Attr attributs[NUMBER_OF_CLIENTS + 1]; //структура с параметрами для потока
 for (int i = 0; i < NUMBER_OF_CLIENTS +1; ++i)
  {
    attributs[i].flag = &flag;
  }
  attributs[NUMBER_OF_CLIENTS].id = 999999;

  
  pthread_create( &thread1, NULL, server, &attributs);

  for (int i = 0; i < NUMBER_OF_CLIENTS; ++i)
  {
    attributs[i].id = i;
    pthread_create( &(threads[i]), NULL, client, &attributs[i]);
  }

  cout << "Push any key" << endl;
  getchar(); //ожидание нажатия клавиши

  flag = 0;

  void *res1 ; //объявляем ресурсы для получения значений возвращаемых нитями
  pthread_join(thread1, (void**) &res1);  //блокируем главную нить до завершения работы нитей во 2 арг записывается возвращаемое нитью знач

  for (int i = 0; i < NUMBER_OF_CLIENTS; ++i)
  {
    pthread_join(threads[i], (void**) &res1); 
  }

  cout << "Program end" << endl;
  return 0;
}