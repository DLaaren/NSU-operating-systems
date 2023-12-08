#ifndef HTTP_PROXY_H
#define HTTP_PROXY_H

#define MAX_CONNECTIONS 10

int proxy_run(int port);
int proxy_stop();

#endif // HTTP_PROXY_H