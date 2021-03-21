#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <netdb.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>
#include <arpa/inet.h>

#define SERVER_PORT    "8000"
#define MAX_CONNECTION 1000
//https://github.com/sirpolin/Linux1-task/blob/936e83b00ddc9b02e5dbe8ea6156d14228b7ef10/main.c

int create_socket(const char *Port); // создание сокета
void sm200(int sock_f);

/*
To accept connections, the following steps are performed:

         1.  A socket is created with socket(2).

         2.  The socket is bound to a local address using bind(2), so that
             other sockets may be connect(2)ed to it.

         3.  A willingness to accept incoming connections and a queue
             limit for incoming connections are specified with listen().

         4.  Connections are accepted with accept(2).
*/




int main()
{
  int sock;
  printf("Starting server...\n");
  sock = create_socket(SERVER_PORT);
  if (sock<0)
  {
    fprintf(stderr, "ERROR: Create socket (CODE: %d)\n", sock);
    return -1;
  }
  printf ("Server created\n");
  struct sockaddr client_addr; // информация об адресе
  int client_d; // идентефикатор сокета
  while(1)
  {
    unsigned int size = (unsigned int) sizeof(client_addr);
    client_d = accept(sock, &client_addr, &size);
    if (client_d == -1)
    {
      fprintf(stderr, "ERROR: Error accept\n");
      return -1;
    }


    char ip[INET_ADDRSTRLEN]; //Выделяем буфер. INET6_ADDRSTRLEN - 46 байт. Length of the string form for IPv6.
    inet_ntop(AF_INET, &(((struct sockaddr_in*)((struct sockaddri*)&(client_addr))) ->sin_addr),ip, sizeof ip); // преобразование IP из двоичного в точечкодесятичное значение
    printf ("Got connection from %s\n", ip);
    sm200(client_d);
  }
  return 0;
}



void sm200(int sock_f) //send message 200
{
    FILE *fp;
    char message[1];
    char buffer[65536] = {0};
    fp = fopen("index.txt","r");
    strcat(buffer, "HTTP/1.1 200 OK\n\n");
    strcat(buffer, "<html><body><h1>");
    fscanf(fp,"%s",message[0]);
    strcat(buffer, message[0]);
    strcat(buffer, "</h1></body></html>");
    int len = strlen(buffer);
    printf("Sending message to client...\n");
    send(sock_f, buffer, len, 0);
    printf("Done...\n");
    close(sock_f);
    fclose(fp);
}

int create_socket(const char *Port) // создание сокета
{
  int sock;
  int yes;
  struct addrinfo hints;
  struct addrinfo *servinfo;
  struct addrinfo *p;

  memset(&hints, 0, sizeof(hints));

  hints.ai_family =AF_UNSPEC; //неважно какой версии ip (IPv4, IPv6)
  hints.ai_socktype = SOCK_STREAM; //TCP/IP
  hints.ai_flags = AI_PASSIVE; // автоматическое заполнение IP
  int r = getaddrinfo(NULL, Port, &hints, &servinfo);
  
  if(r!=0)
  {
    fprintf((stderr), "ERROR: getaddrifo()\n");
    return -1;
  }
  
  for (p= servinfo; p!= NULL; p = p->ai_next)
  {
    sock = socket(p->ai_family, p->ai_socktype, p->ai_protocol);
    if (sock == -1) continue;
    if(bind(sock,p->ai_addr, p->ai_addrlen) == -1) //присваивание имени сокету
    {
      close(sock);
      continue;
    }
    break;
  }
  
  freeaddrinfo(servinfo);

  if (p==NULL)
  {
    fprintf(stderr, "ERROR: Failed to find address\n");
    return -3;
  
      }
    
    char ip[INET_ADDRSTRLEN]; //Выделяем буфер. INET6_ADDRSTRLEN - 46 байт. Length of the string form for IPv6.

    inet_ntop(AF_INET, &(((struct sockaddr_in*)((struct sockaddri*)&(p->ai_next))) ->sin_addr),ip, sizeof ip); // преобразование IP из двоичного в точечкодесятичное значение
    printf ("IP:  %s\n", ip);
if (listen(sock,MAX_CONNECTION) == -1) // подготовка сокета для входящего соединения
  {
    fprintf(stderr, "ERROR: listen()\n");
    return -4;
  }
  return sock;
}
