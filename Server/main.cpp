
#include <stdlib.h> 
#include <iostream>
#include <thread>
#include <atomic>

#include "coreserver.cpp"

#define SERVERTIME 15
int main() {

    std::atomic<bool> serverRunning; 
    std::atomic<bool> processingRunning;
    serverRunning.store(true); 
    processingRunning.store(true);
    server udpServer; 
    uint16_t timeLeft = SERVERTIME;

    std::thread packetProcessingThread([&processingRunning, &serverRunning](){
        PacketProcessing::init(processingRunning, serverRunning);
    });

    std::thread serverThread([&udpServer, &serverRunning](){
        udpServer.runServer(serverRunning);
    }); 

    while (timeLeft > 0) {
        sleep(1);
        timeLeft--;
    }
    serverRunning.store(false);

    serverThread.join();
    processingRunning.store(false);
    packetProcessingThread.join();

    return 0; 
}
