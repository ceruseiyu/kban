#include <stdio.h>
#include <pthread.h>
#include <string.h>
#include <stdlib.h>
#include "kban.h"
#include <curl/curl.h>

ProxyList* readProxies(char* fileName) {
  FILE *file = fopen(fileName, "r");
  if(file != NULL) {
    ProxyList* plist = malloc(sizeof(ProxyList));
    plist->proxies = malloc(sizeof(char*));
    if(plist->proxies == NULL) {
      return NULL;
    }
    plist->len = 0;
    char line[32];
    while(fgets(line, sizeof(line), file) != NULL) {
      if(line[strlen(line) - 1] == '\n')  {
        line[strlen(line) - 1] = '\0';
      }
      plist->len++;
      plist->proxies = realloc(plist->proxies, sizeof(char*) * plist->len);
      if(plist->proxies == NULL) {
         return NULL;
      }
      plist->proxies[plist->len - 1] = malloc(strlen(line) + 1);
      if(plist->proxies[plist->len - 1] == NULL) {
        plist->len--;
        int i;
        for(i = 0; i < plist->len; i++) {
          free(plist->proxies[i]);
        }
        free(plist);
        return NULL;
      }
      strcpy(plist->proxies[plist->len - 1], line);
    }
    return plist;
  }
  return NULL;
}

void* runThread(void* arg) {
  ThreadArgs* arguments = (ThreadArgs*)arg;
  CURL* curl;
  CURLcode res;
  curl = curl_easy_init();
  if(curl) {
    curl_easy_setopt(curl, CURLOPT_URL, arguments->url);
    curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);
    if(arguments->proxy != NULL) {
      curl_easy_setopt(curl, CURLOPT_PROXY, arguments->proxy);
    }
    curl_easy_setopt(curl, CURLOPT_NOBODY, 1);
    int i;
    for(i = 0; i < arguments->count; i++) {
      curl_easy_perform(curl);
      printf("Thread %d Completed Request %d\n", arguments->id, i);
      if(res != CURLE_OK) {
        break;
      }
    }
    curl_easy_cleanup(curl);
  }
  free(arguments->url);
  if(arguments->proxy != NULL) {
    free(arguments->proxy);
  }
  free(arguments);
  pthread_exit(NULL);
}
