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
#include <iostream>
#include <fstream>
#include <vector>

struct ClientThreadParameters
{
    int client;
    std::vector<char> *buf;
};
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

    std::ifstream file("res/plik", std::ios::binary | std::ios::ate);
    std::streamsize size = file.tellg();
    file.seekg(0, std::ios::beg);

    std::vector<char> buffer(size);
    if (!file.read(buffer.data(), size))
    {
        exit(1);
    }

    while (1)
    {
        int client_socket = accept(server_socket, NULL, NULL);

        pthread_t thread;
        ClientThreadParameters *params = new ClientThreadParameters;

        params->client = client_socket;
        params->buf = &buffer;

        pthread_create(&thread, NULL, handle_client, params);
        pthread_detach(thread);
    }

    close(server_socket);

    return 0;
};
void *handle_client(void *arg)
{
    auto *params = static_cast<ClientThreadParameters *>(arg);

    int client_socket = params->client;
    const std::vector<char> *buffer = params->buf;

    delete params;

    write(
        client_socket,
        buffer->data(),
        buffer->size());

    close(client_socket);

    return NULL;
}