#ifndef KBAN
#define KBAN

typedef struct proxytype {
  char** proxies;
  int len;
} ProxyList;

typedef struct argtype {
  char* url;
  char* proxy;
  int count;
  int id;
} ThreadArgs;

ProxyList* readProxies(char* fileName);
void* runThread(void* arg);

#endif
