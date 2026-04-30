#include <stdlib.h> 
#include <iostream>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h> 
#include <unistd.h>


#define MAXBUFF 4096
#define PORT 8010


int main() {
    int sockfd; 
    char buffer[MAXBUFF];
    
    if ((sockfd = socket(AF_INET, SOCK_DGRAM,0)) < 0) {
        std::cerr << "ERROR Socket Creation Failure\n" << std::endl; 
        return -1; 
    }

    sockaddr_in server; 
    sockaddr_in client; 
    memset(&server, 0, sizeof(server)); 
    memset(&client, 0, sizeof(client)); 

    server.sin_family = AF_INET; 
    server.sin_addr.s_addr = INADDR_ANY; 
    server.sin_port = htons(PORT); 

    if (bind(sockfd, reinterpret_cast<sockaddr*>(&server), sizeof(server)) < 0) {
        std::cerr << "ERROR Bind Failure\n" << std::endl; 
        close(sockfd);
        return -1;
    }

    
    socklen_t clientSize = sizeof(client);
    const socklen_t clientAddrlen = sizeof(client); 
    int ret; 
    unsigned int errorCount = 0; 
    unsigned long long int totalBytesRec = 0; 

    while (true) {
        clientSize = clientAddrlen; 
        ret = recvfrom(sockfd, buffer, MAXBUFF, 0,reinterpret_cast<sockaddr*>(&client), &clientSize);
        if (ret < 0) {
            errorCount++; 
        } else {
            totalBytesRec += ret;
        }
    }

    return 0; 
}