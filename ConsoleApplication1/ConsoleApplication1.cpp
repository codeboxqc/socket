#include <iostream>
#include <winsock2.h>
#include <Ws2tcpip.h> // Include this header for InetPton

#pragma comment(lib, "ws2_32.lib")
#pragma warning(disable: 4996) 

 
class Socket {
public:
    Socket() {
        if (WSAStartup(MAKEWORD(2, 2), &wsaDATA) != 0) {
            std::cerr << "WSAStartup failed\n";
            std::exit(EXIT_FAILURE);
        }

        socket_result = socket(AF_INET, SOCK_STREAM, 0);
        if (socket_result == INVALID_SOCKET) {
            std::cerr << "Error creating socket\n";
            WSACleanup();
            std::exit(EXIT_FAILURE);
        }
    }

    

    int Connect(char* ip, const int port) {
        sockaddr_in serverAddr;
        serverAddr.sin_family = AF_INET;

        if (InetPton(AF_INET, ip, &serverAddr.sin_addr) != 1) {
            std::cerr << "Invalid IP address\n";
            closesocket(socket_result);
            WSACleanup();
            return SOCKET_ERROR;
        }

        serverAddr.sin_port = htons(port);

        if (connect(socket_result, reinterpret_cast<struct sockaddr*>(&serverAddr), sizeof(serverAddr)) == SOCKET_ERROR) {
            int errorCode = WSAGetLastError();
            std::cerr << "Error connecting. Error code: " << errorCode << "\n";

            if (errorCode == WSAECONNREFUSED) {
                std::cerr << "Connection refused. The server may not be running or is not accepting connections.\n";
            }

            closesocket(socket_result);
            WSACleanup();
            return SOCKET_ERROR;
        }

        return 0;
    }

    int Send(const char* buf)
    {
        return send(socket_result, buf, strlen(buf), 0);
    }
    int Receive(char* buf, const int len)
    {
        return recv(socket_result, buf, len, 0);
    }

    void Close() {
        closesocket(socket_result);
        WSACleanup();
    }

  

    ~Socket() {
        closesocket(socket_result);
        WSACleanup();
    }

private:
    SOCKET socket_result;
    WSADATA wsaDATA;
};
 

char* GetIP(const char* website) {
    struct addrinfo* result = nullptr;
    struct addrinfo hints;

    ZeroMemory(&hints, sizeof(hints));
    hints.ai_family = AF_INET;

    if (getaddrinfo(website, nullptr, &hints, &result) != 0) {
        std::cerr << "Error getting address info for " << website << ". Error code: " << WSAGetLastError() << "\n";
        return nullptr;
    }

    char ip[INET_ADDRSTRLEN];
    inet_ntop(AF_INET, &reinterpret_cast<struct sockaddr_in*>(result->ai_addr)->sin_addr, ip, INET_ADDRSTRLEN);

    freeaddrinfo(result);

    char* resultIP = _strdup(ip);
    return resultIP;
}

 
int main() {
    Socket mySocket;
    const char* serverIP = "142.251.46.206";
    const int serverPort = 80;
     
    const char* website = "www.google.com";


    printf("%s\n", website);

    char* ipAddress = GetIP(website);

    printf("%s\n", ipAddress);

    if (mySocket.Connect(ipAddress, serverPort) == SOCKET_ERROR) {
        std::cerr << "Error connecting to server\n";
        return 1;
    }

    const char* httpRequest = "GET / HTTP/1.1\r\nHost: www.google.com\r\nConnection: close\r\n\r\n";
    mySocket.Send(httpRequest);

    char buf[8096];
    int bytesRead;

    while ((bytesRead = mySocket.Receive(buf, sizeof(buf) - 1)) > 0) {
        buf[bytesRead] = '\0';  // Null-terminate the received data
        printf("%s", buf);
    }

    printf("end\n" );
    mySocket.Close();

    return 0;
}
