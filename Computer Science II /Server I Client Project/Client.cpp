#include <winsock2.h>
#include <ws2tcpip.h>
#include <iostream>
#include <string>

using namespace std;

#pragma comment(lib, "ws2_32.lib")

#define PORT 54000
#define SERVER_IP "127.0.0.1"

int main() {

    WSADATA wsaData;
    SOCKET clientSocket;
    sockaddr_in serverAddr;

    char buffer[256] = {0};

    // Start Winsock
    if (WSAStartup(MAKEWORD(2,2), &wsaData) != 0) {
        cout << "WSAStartup failed\n";
        return 1;
    }

    // Create socket
    clientSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (clientSocket == INVALID_SOCKET) {
        cout << "Socket creation failed\n";
        WSACleanup();
        return 1;
    }

    // Setup server address
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(PORT);
    inet_pton(AF_INET, SERVER_IP, &serverAddr.sin_addr);

    // Connect
    if (connect(clientSocket, (sockaddr*)&serverAddr, sizeof(serverAddr)) == SOCKET_ERROR) {
        cout << "Connection failed\n";
        closesocket(clientSocket);
        WSACleanup();
        return 1;
    }

    // Get input
    string input;
    cout << "Enter state abbreviation (EX: IN, CA): ";
    cin >> input;

    // Validate
    if (input.length() != 2) {
        cout << "Invalid input format\n";
        closesocket(clientSocket);
        WSACleanup();
        return 1;
    }

    // SEND to server (THIS WAS MISSING BEFORE)
    send(clientSocket, input.c_str(), 2, 0);

    // RECEIVE response
    int bytesReceived = recv(clientSocket, buffer, sizeof(buffer) - 1, 0);

    if (bytesReceived > 0) {
        buffer[bytesReceived] = '\0';
        cout << "Server response: " << buffer << endl;
    } else {
        cout << "No response from server\n";
    }

    // Cleanup
    closesocket(clientSocket);
    WSACleanup();

    return 0;
}