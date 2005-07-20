#include "PosixThread.hpp"
#include <stdio.h>

int main()
{
  Mutex lock;
  pthread_mutex_t mutex;//  = NULL;
  
  int returnvalue = pthread_mutex_init(&mutex, NULL);
  fprintf(stderr, "returns %d\n", returnvalue);
  returnvalue = pthread_mutex_init(&mutex, NULL);
  fprintf(stderr, "returns %d\n", returnvalue);

  if (pthread_mutex_lock(&mutex) == 0)
    fprintf(stderr, "locked.\n");
  else
    fprintf(stderr, "unlocked.\n");

  if (pthread_mutex_trylock(&mutex) == 0)
    fprintf(stderr, "locked.\n");
  else
    fprintf(stderr, "unlocked.\n");

  if (pthread_mutex_lock(&mutex) == 0)
    fprintf(stderr, "locked.\n");
  else
    fprintf(stderr, "unlocked.\n");

  return 0;
}

  
