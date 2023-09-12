#ifndef SERVER_MAIN_H
#define SERVER_MAIN_H

#include <WinSock2.h>
#include <WS2tcpip.h>

#pragma comment(lib, "ws2_32.lib")

#include <iostream>
#include <ctime>
#include <stack>
#include <queue>
#include <vector>
#include <mutex>

using std::cout;
using std::endl;

#include <thread>
#include <chrono>

#include <string>
#include <stdexcept>

#define SERVER_PORT 11235  // server port number
#define BUF_SIZE 4096 // block transfer size  
#define QUEUE_SIZE 10
#define IPAddress "127.0.0.1" // server IP address

#endif // !SERVER_MAIN_H