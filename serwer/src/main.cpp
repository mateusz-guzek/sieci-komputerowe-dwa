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
#include <atomic>
#include <thread>
#include <chrono>
#include <iostream>
#include <signal.h>

struct ClientThreadParameters
{
    int client;
    std::vector<char> *buf;
    std::chrono::steady_clock::time_point stream_start;
    int bps;
};

void *handle_client(void *arg);
int main()
{

    signal(SIGPIPE, SIG_IGN);

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
    const auto stream_start = std::chrono::steady_clock::now();
    const int bps = 80;

    while (1)
    {
        int client_socket = accept(server_socket, NULL, NULL);

        pthread_t thread;
        ClientThreadParameters *params = new ClientThreadParameters;

        params->client = client_socket;
        params->buf = &buffer;
        params->stream_start = stream_start;
        params->bps = bps;

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
    const auto stream_start = params->stream_start;
    const int bps = params->bps;
    const double bytes_per_second = params->bps / 8.0;

    delete params;

    const std::chrono::duration<float> elapsed = std::chrono::steady_clock::now() - stream_start;
    int start = static_cast<int>(elapsed.count() * bytes_per_second);

    while (start < buffer->size())
    {
        const auto target_time = stream_start +
                                 std::chrono::duration_cast<std::chrono::steady_clock::duration>(
                                     std::chrono::duration<double>(
                                         static_cast<double>(start) / bytes_per_second));

        std::this_thread::sleep_until(target_time);
        
        if (write(
                client_socket,
                buffer->data() + start,
                1) == -1)
        {
            close(client_socket);
            return nullptr;
        };
        start++;
    }

    close(client_socket);

    return nullptr;
}
