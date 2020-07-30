#include <stdlib.h>
#include <semaphore.h>

#include <pthread.h>
#include <stdio.h>
#include <iostream>
#include <unistd.h>

int MAX_SLOTS = 10;
int storage = 0;

using namespace std;

struct Attr
{
  int *flag;
  pthread_mutex_t *mutex; 
  pthread_cond_t *request_consumer, *request_producer;
};

void* producerThread(void* data){
  cout << "Producer start" << endl;

  Attr *data1 = (Attr*) data;
  int *flag = data1->flag;
  pthread_mutex_t *mutex = data1->mutex;
  pthread_cond_t *request_consumer = data1->request_consumer;
  pthread_cond_t *request_producer = data1->request_producer;

  while (*flag == 1){

    pthread_mutex_lock(mutex);
    while (storage >= MAX_SLOTS)
      pthread_cond_wait(request_producer, mutex);
    
    storage++;
    cout << "Producer add: count = "<< storage << "\n"<<flush;
    
    pthread_mutex_unlock(mutex);
    pthread_cond_signal(request_consumer);

    usleep(1000000);
    
  }
  cout << "Producer end" << endl;

  pthread_exit ((void *) 1);
}


void* consumerThread(void* data){
  cout << "Consumer start" << endl;

  Attr *data1 = (Attr*) data;
  int *flag = data1->flag;
  pthread_mutex_t *mutex = data1->mutex;
  pthread_cond_t *request_consumer = data1->request_consumer;
  pthread_cond_t *request_producer = data1->request_producer;

  while (*flag == 1){

    pthread_mutex_lock(mutex);
    while (storage == 0)
      pthread_cond_wait(request_consumer, mutex);

    storage--;
    cout << "Consumer pick: count = "<<  storage << "\n"<<flush;

    pthread_mutex_unlock(mutex);
    pthread_cond_signal(request_producer);

    usleep(2000000);

  }
  cout << "Consumer end" << endl;

  pthread_exit ((void *) 1);
}


int main(int argc, char const *argv[]){
  pthread_t thread1, thread2; //объявляем нить(поток)

  int flag1 = 1; //иницилизируем флаги завершения цикла в функции потока
  int flag2 = 1;

  pthread_mutex_t mutex; 
  pthread_cond_t request_consumer, request_producer;


  pthread_mutex_init(&mutex , NULL);
  pthread_cond_init(&request_consumer, NULL);
  pthread_cond_init(&request_producer, NULL);

  Attr attributs; //структура с параметрами для потока
  attributs.flag = &flag1;
  attributs.mutex = &mutex;
  attributs.request_consumer = &request_consumer;
  attributs.request_producer = &request_producer;

  pthread_create( &thread1, NULL, producerThread, &attributs); //создаем нити и передаём адрес структуры в последнем аргументе
  attributs.flag = &flag2;
  pthread_create( &thread2, NULL, consumerThread, &attributs);

  

  cout << "Push any key" << endl;
  getchar(); //ожидание нажатия клавиши

  flag1 = 0;
  flag2 = 0;

  void *res1 , *res2; //объявляем ресурсы для получения значений возвращаемых нитями
  pthread_join(thread1, (void**) &res1);  //блокируем главную нить до завершения работы нитей во 2 арг записывается возвращаемое нитью знач
  pthread_join(thread2, (void**) &res2);

  cout << (long)res1 << endl;
  cout << (long)res2 << endl;

  pthread_mutex_destroy(&mutex); //уничтожаем мьютекс
  pthread_cond_destroy(&request_consumer);
  pthread_cond_destroy(&request_producer);

  cout << "Program end" << endl;
  return 0;
}
