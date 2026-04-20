/*
Computer Science II 
Textbook Assignment Module 6. 

Challenge: 
Create a client and a server C++ program such that the server computes and sends back to the client the long name of any U.S. state given the two-letter abbreviation. The protocol is binary, and is as follows:

(1) Client connects and sends the two-letter state abbreviation in binary form.

(2) Server reads the character string and sends back a single character string containing the long name of the state, also in binary form. If an invalid abbreviation is sent, the server should respond with an appropriate error response.

(3) The server closes the connection to the client.

The server will be a single-threaded server that serializes service to its clients.

The server will stay in a loop in which it accepts a connection, reads the character data sent through the connection, and sends back the character response through the same connection. After that, the server closes the connection and goes back to wait for another connection.

The client should ask the user for a state abbreviation, read the user input, open a connection to the server, and send the data. It should then read the reply and print it (the long name of the state) to the screen.

Test your server and client on your own machine by using two different terminals.

You should also, test your client against another student’s server code, and then test your server code against another student’s client.

Submit your source code along with three screen shots of both the server and client demonstrating the application works correctly for valid and invalid abbreviations.

Chad Carmickle, 4/20/2026

*/
 
// Libraries required: 
#include <winsock2.h>          // This is core windows networking Library that will allow us to use common network functions. 
#include <ws2tcpip.h>         // This futher adds to winsock2.h and allows us to use port-fordwarding, port-Handling and protocols helpers.  
#include <iostream>          // Standard Input/Output used in C++. 
#include <unordered_map>    // We will use this for a MAP variable for the states. 
using namespace std;       // Adding ability to output data without having to specifify std each time.  
#include <string>         // Adding ability to use strings in C++. 

// This library links Visual c++ to include the winsock library. 
#pragma comment(lib, "ws2_32.lib")

// This creates a port for the server to listen on. 
// Where an IP address would relate to the building a port is far more speicifc 
// inside the building relating to the apt room. 
#define PORT 54000



std::unordered_map<std::string, std::string> states = {
        {"AL","Alabama"},{"AK","Alaska"},{"AZ","Arizona"},{"AR","Arkansas"},{"CA","California"},
        {"CO","Colorado"},{"CT","Connecticut"},{"DE","Delaware"},{"FL","Florida"},{"GA","Georgia"},
        {"HI","Hawaii"},{"ID","Idaho"},{"IL","Illinois"},{"IN","Indiana"},{"IA","Iowa"},
        {"KS","Kansas"},{"KY","Kentucky"},{"LA","Louisiana"},{"ME","Maine"},{"MD","Maryland"},
        {"MA","Massachusetts"},{"MI","Michigan"},{"MN","Minnesota"},{"MS","Mississippi"},{"MO","Missouri"},
        {"MT","Montana"},{"NE","Nebraska"},{"NV","Nevada"},{"NH","New Hampshire"},{"NJ","New Jersey"},
        {"NM","New Mexico"},{"NY","New York"},{"NC","North Carolina"},{"ND","North Dakota"},{"OH","Ohio"},
        {"OK","Oklahoma"},{"OR","Oregon"},{"PA","Pennsylvania"},{"RI","Rhode Island"},{"SC","South Carolina"},
        {"SD","South Dakota"},{"TN","Tennessee"},{"TX","Texas"},{"UT","Utah"},{"VT","Vermont"},
        {"VA","Virginia"},{"WA","Washington"},{"WV","West Virginia"},{"WI","Wisconsin"},{"WY","Wyoming"},
        {"GL","Greenland (Soon...)"},{"CN","Canada (Maybe one day...)"}
    };


int main() {

// Global Variables: 
    
// This is a structure that stores info about WinSock (Windows Socket libary) IE: Version of winSock & System networking infomation. 
    WSADATA wsaData; 

// Declaring two Sockets, For the server and the client. 
    SOCKET serverSocket, clientSocket; // 
    
// This will store the IP address, Port number (IPV4). 
    sockaddr_in serverAddr, clientAddr;

// This will store the size of of the ClientAddr so that the OS knows the length of address to send. 
    int clientSize = sizeof(clientAddr);

// This will allow us to store incoming data from the client IE the state Abrivations. 
// The reason we buffer it to 3 characters and not two is to ensure an extra space for the null Terminator. 
    // IE 'I' , 'N' '\0' 
    char buffer[3] = {0}; // 2 chars + null terminator


// Initialize Winsock
// This step is like the switch to turn on the system. 

/* It asks Windows to start up the winSock giving 2,2 as the version 2.2, &wsaData as the reference. If the 
 A return value is requested asking "Was Winsock initalized correctly? 
 It returns a status code:   
    0 -> Winsock was successful in Initialize. 
    !0 -> Failure | Print error message. 
If failure occurs then the error message returns what number was used for debugging purpose. 
*/ 
int result = WSAStartup(MAKEWORD(2, 2), &wsaData);

if (result != 0) {
    std::cout << "WSAStartup failed with error code: " << result << std::endl;
    std::cout << "Check Winsock initialization or version mismatch." << std::endl; // Gives Advice to ensure the WinSock Version is correct. 
    return 1;
}

// Create socket

/* 
ServerSocket will now create an Endpoint that the server will use. 
    AF_INET     - Address Family Internet : Specifying what IP family we are using :  IE IPV4. 
    Sock_Stream - Defines the type of communcation we are using for the server : IE TCP. 
    0           - Allows windows to choose the specific protcol to use. 

A check is used to check if we have a valid Endpoint. 
    If the socket fails we call WSACleanup(); to allow windows to release the system level resouces it allocated for us in the program. We then terminate the program and exit.  
     
*/
    serverSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (serverSocket == INVALID_SOCKET) {
        std::cout << "Socket creation failed\n";
        WSACleanup();
        return 1;
    }


    // Setup address
/* 
In this step we create variables that will be used in the next step. 

*/
    serverAddr.sin_family = AF_INET; // Sets address type to IPv4. 
    serverAddr.sin_addr.s_addr = INADDR_ANY; // Listens on all available network interfaces.  
    serverAddr.sin_port = htons(PORT); // This sets the port number. Host To Network Short (HTONS) will convert machine format to a network format ensuring computers agree on how to read the data. 

// Bind
/* 
This is the step where we use the prior declarations to assign the port to this program. 
IE Port 5400 is now reserved for this server therfore no other programs can bind to this port while we are using it. 

If the socket binding failed we print a error message and close the socket / release WinSocket. 
*/
    if (bind(serverSocket, (sockaddr*)&serverAddr, sizeof(serverAddr)) == SOCKET_ERROR) {
        std::cout << "Bind failed\n";
        closesocket(serverSocket);
        WSACleanup();
        return 1;
    }
    
// Listen
// In this step the server configuration has been completed and is now ready to accept requests from Clients. 

/* 
Listen(ServerSocket SOMAXCONN) - Places the socket into a listening state where it will wait for incoming requests. 
SOMAXCONN                      - This relates to how many pending requests can be queued into the server. Allows the OS to decide the number of backlog queue size. 

If the Socket fails we close the socket thus releasing it back to the system. 
We then close WinSocket again and exit the program. 

Otherwise we print a message we are awaiting for a request on this port. 
*/

    if (listen(serverSocket, SOMAXCONN) == SOCKET_ERROR) {
        std::cout << "Listen failed\n";
        closesocket(serverSocket);
        WSACleanup();
        return 1;
    }

    std::cout << "Server listening on port " << PORT << "...\n";


// Connection Accepted: 
/* 
Once a client has been connected it creates a new socket for that connection. 
    ClientAddr - Windows will use this to fill in the with IP address & Client Port the client uses to connect. 
    ClientSize - Windows updates the size of the client address infomation.  

If INVALID_SOCKET = true - We were unable to accept the connection therfore we close the clients socket, WinSock closes and exit. 
On real-world servers we not nore exit here we would create another thread to accept connections. 
*/

    while (true) {
        // Accept connection
        clientSocket = accept(serverSocket, (sockaddr*)&clientAddr, &clientSize);
        if (clientSocket == INVALID_SOCKET) {
            std::cout << "Accept failed\n";
            continue;
        }

// If connection is made we then recive 2 bytes od data IE the State Abbrivations. 
/*  int bytesReceived = recv(clientSocket, buffer, 2, 0); - This reads data from the client. 
        recv         - Abbrivation for "recive" function. 
        ClientSocket - This is the socket created earlier by the accept() function. 
        Buffer       - Memmory location where the data is recived. 
        2            - How many bytes of data we are accepting and reading. 
        0            - Flags used by the TCP programming. 

We then then use ButesRecieved to count the counts and ensure no negitive nubmers are used. 
        buffer[2] = '\0' - Here c++ will cut off the ending byte as C++ requires null terminators to identify when a string ends.  
        Cout             - Lastly a print message to notify the user we have recived the data and what data was sent to us IE the state abriviation. 
       
        One we resive the abbirvations we then search for them in the MAP variable for all the states + Abbriviations. 
        */ 

        // Receive 2 bytes
    int bytesReceived = recv(clientSocket, buffer, 2, 0);
        
    if (bytesReceived > 0) {
            buffer[bytesReceived] = '\0';

            std::string abbr(buffer);
            std::string response;

                std::cout << "Received " << abbr << std::endl;

        if (states.find(abbr) != states.end()) {
                response = states[abbr];
        } else {
                response = "Invalid state abbreviation";
            }
                
            send(clientSocket, response.c_str(), response.size(), 0);
        }

    else {
        std::cout << "recv failed or client disconnected\n";
    }





// We then close the socket that we are done using. 
        closesocket(clientSocket);
    }

    // Cleanup is added here incase but most cases it should never reach this. Only added as a extra measure for fault. 
    closesocket(serverSocket);
    WSACleanup();

    return 0;
}



