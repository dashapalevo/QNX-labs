#include <stdlib.h>
#include <semaphore.h>

#include <pthread.h>
#include <stdio.h>
#include <iostream>
#include <unistd.h>

int MAX_SLOTS = 10;

using namespace std;

struct Attr
{
  int *flag;
  pthread_mutex_t *mutex; 
  sem_t *empty;
  sem_t *full;
};

void* producerThread(void* data){
  cout << "Producer start" << endl;

  Attr *data1 = (Attr*) data;
  int *flag = data1->flag;
  pthread_mutex_t *mutex = data1->mutex;
  sem_t *empty = data1->empty;
  sem_t *full = data1->full;

  while (*flag == 1){

    sem_wait(empty); //
    pthread_mutex_lock(mutex); //Закрываем мьютекс
    
    //Запись в буфер
    int value;
    sem_getvalue(empty,&value);
    cout << "Producer add: count = "<< MAX_SLOTS - value << "\n"<<flush;
    
    pthread_mutex_unlock(mutex); //
    sem_post(full); //
    
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
  sem_t *empty = data1->empty;
  sem_t *full = data1->full;

  while (*flag == 1){


    sem_wait(full); //
    pthread_mutex_lock(mutex); //Закрываем мьютекс
    
    //Считывание из буфера
    int value;
    sem_getvalue(full,&value);
    cout << "Consumer pick: count = "<<  value << "\n"<<flush;
    
    pthread_mutex_unlock(mutex); //
    sem_post(empty); //
    
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
  sem_t empty, full;


  pthread_mutex_init(&mutex , NULL);
  sem_init(&empty, NULL, MAX_SLOTS);
  sem_init(&full, NULL, 0);

  Attr attributs; //структура с параметрами для потока
  attributs.flag = &flag1;
  attributs.mutex = &mutex;
  attributs.empty = &empty;
  attributs.full = &full;
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
  sem_destroy(&empty);
  sem_destroy(&full);

  cout << "Program end" << endl;
  return 0;
}