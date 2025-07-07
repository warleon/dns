#include <iostream>
#include <cstring>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include "dns_message.hpp"
#include "types.hpp"

int main()
{
    // Flush after every std::cout / std::cerr
    std::cout << std::unitbuf;
    std::cerr << std::unitbuf;

    // Disable output buffering
    setbuf(stdout, NULL);

    // You can use print statements as follows for debugging, they'll be visible when running tests.
    std::cout << "Logs from your program will appear here!" << std::endl;

    // Uncomment this block to pass the first stage
    int udpSocket;
    struct sockaddr_in clientAddress;

    udpSocket = socket(AF_INET, SOCK_DGRAM, 0);
    if (udpSocket == -1)
    {
        std::cerr << "Socket creation failed: " << strerror(errno) << "..." << std::endl;
        return 1;
    }

    // Since the tester restarts your program quite often, setting REUSE_PORT
    // ensures that we don't run into 'Address already in use' errors
    int reuse = 1;
    if (setsockopt(udpSocket, SOL_SOCKET, SO_REUSEPORT, &reuse, sizeof(reuse)) < 0)
    {
        std::cerr << "SO_REUSEPORT failed: " << strerror(errno) << std::endl;
        return 1;
    }

    sockaddr_in serv_addr = {
        .sin_family = AF_INET,
        .sin_port = htons(2053),
        .sin_addr = {htonl(INADDR_ANY)},
    };

    if (bind(udpSocket, reinterpret_cast<struct sockaddr *>(&serv_addr), sizeof(serv_addr)) != 0)
    {
        std::cerr << "Bind failed: " << strerror(errno) << std::endl;
        return 1;
    }

    int bytesRead;
    buffer_t buffer = new char[buffer_size];
    socklen_t clientAddrLen = sizeof(clientAddress);

    buffer_t response = new char[buffer_size];
    while (true)
    {
        memset(buffer, 0, buffer_size);
        memset(response, 0, buffer_size);
        // Receive data
        bytesRead = recvfrom(udpSocket, buffer, buffer_size, 0, reinterpret_cast<struct sockaddr *>(&clientAddress), &clientAddrLen);
        if (bytesRead == -1)
        {
            perror("Error receiving data");
            break;
        }

        DNSMessage dns_question(buffer);
        DNSMessage dns_response(dns_question);

        // Create an empty response
        uint16_t response_size = dns_response.serialize(response);

        // Send response
        if (sendto(udpSocket, response, response_size, 0, reinterpret_cast<struct sockaddr *>(&clientAddress), sizeof(clientAddress)) == -1)
        {
            perror("Failed to send response");
        }
    }

    delete[] buffer;
    delete[] response;
    close(udpSocket);

    return 0;
}
