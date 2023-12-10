#ifndef HTTP_PROXY_H
#define HTTP_PROXY_H

int proxy_run(int port);
int proxy_stop(int listening_socket_fd);

#endif // HTTP_PROXY_H