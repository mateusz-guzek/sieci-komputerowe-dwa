#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <time.h>
#include <pthread.h>
#include <string>

void *handle_client(void *arg);
int main()
{
    sockaddr_in sa{};

    sa.sin_family = AF_INET;
    sa.sin_port = htons(5000);
    sa.sin_addr.s_addr = INADDR_ANY;

    int server_socket = socket(AF_INET, SOCK_STREAM, 0);

    bind(server_socket, (struct sockaddr *)&sa, sizeof(sa));

    listen(server_socket, 64);

    while (1)
    {
        int client_socket = accept(server_socket, NULL, NULL);

        int *client = new int;
        *client = client_socket;

        pthread_t thread;

        pthread_create(&thread, NULL, handle_client, client);
        pthread_detach(thread);
    }

    close(server_socket);

    return 0;
};

void *handle_client(void *arg)
{
    int client_socket = *(int *)arg;
    delete (int *)arg;

    char buf[1024];

    while (1)
    {
        ssize_t n = read(client_socket, buf, sizeof(buf));

        std::string data(buf, n);

        std::string response = "Serwer: " + data;

        if (n <= 0)
            break;

        write(client_socket, response.c_str(), response.size());
    }

    close(client_socket);
    return NULL;
}