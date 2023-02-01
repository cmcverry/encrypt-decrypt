#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>


// Set up the address struct for the server socket
void setupAddressStruct(struct sockaddr_in* address,
    int portNumber) {

    // Clear out the address struct
    memset((char*)address, '\0', sizeof(*address));

    // network capable address
    address->sin_family = AF_INET;
    // Store the port number
    address->sin_port = htons(portNumber);
    // Allow a client at any address to connect to this server
    address->sin_addr.s_addr = INADDR_ANY;
}

int main(int argc, char* argv[]) {
    int connectionSocket, charsRead, charsWritten;
    struct sockaddr_in serverAddress, clientAddress;
    socklen_t sizeOfClientInfo = sizeof(clientAddress);

    // Check usage & args
    if (argc < 2) {
        fprintf(stderr, "SERVER: USAGE: %s port\n", argv[0]);
        exit(1);
    }

    // Create the socket that will listen for connections
    int listenSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (listenSocket < 0) {
        fprintf(stderr, "SERVER: ERROR opening socket\n");
        exit(1);
    }

    // Set up the address struct for the server socket
    setupAddressStruct(&serverAddress, atoi(argv[1]));

    // Associate the socket to the port
    if (bind(listenSocket,
        (struct sockaddr*)&serverAddress,
        sizeof(serverAddress)) < 0) {
        fprintf(stderr, "SERVER: ERROR on binding\n");
        exit(1);
    }

    // Start listening for connections. Allow up to 5 connections to queue up
    listen(listenSocket, 5);

    // Accept a connection, blocking if one is not available until one connects
    while (1) {
        // Accept the connection request which creates a connection socket
        connectionSocket = accept(listenSocket,
            (struct sockaddr*)&clientAddress,
            &sizeOfClientInfo);
        if (connectionSocket < 0) {
            fprintf(stderr, "SERVER: ERROR on accept\n");
        }
        // Creates a child process
        pid_t spawnpid = fork();
        switch (spawnpid)
        {

        // Handles fork() failure 
        case -1:
        {
            fprintf(stderr, "fork() failed\n");
            break;
        }

        // Inside child process
        case 0:
        {
            // Initializes empty strings
            char connCheck[11];
            char ptLength[11];
            char kLength[11];
            memset(kLength, '\0', sizeof(kLength));
            memset(connCheck, '\0', sizeof(connCheck));
            memset(ptLength, '\0', sizeof(ptLength));

            // Read the client message from the socket
            charsRead = recv(connectionSocket, connCheck, sizeof(connCheck) - 1, 0);
            if (charsRead < 0) {
                fprintf(stderr, "SERVER: ERROR reading from socket (checking connection)\n");
                exit(1);
                break;
            }

            // Verifies client
            char accept[10] = "accepted";
            char reject[10] = "rejected";
            if (strcmp(connCheck, "enc client") != 0)
            {
                charsWritten = send(connectionSocket, reject, sizeof(reject) - 1, 0);
                fprintf(stderr, "SERVER: ERROR unaccepted client\n");
                // exits child process
                exit(1);
                break;
            }
            else
            {
                charsWritten = send(connectionSocket, accept, sizeof(accept) - 1, 0);
            }

            // Receives length of plaintext from client
            charsRead = recv(connectionSocket, ptLength, sizeof(ptLength) - 1, 0);
            if (charsRead < 0) {
                fprintf(stderr, "SERVER: ERROR reading from socket (getting plaintext length)\n");
                // exits child process
                exit(1);
                break;
            }

            // Receives length of key from client
            charsRead = recv(connectionSocket, kLength, sizeof(kLength) - 1, 0);
            if (charsRead < 0) {
                fprintf(stderr, "SERVER: ERROR reading from socket (getting key length)\n");
                // exits child process
                exit(1);
                break;
            }

            // Variable initializations and '\0' filling arrays
            int plaintextLength = atoi(ptLength);
            int keyLength = atoi(kLength);
            char bufferText[plaintextLength];
            char bufferKey[keyLength];
            // buffer used for receiving msgs via sockets
            // msgs received by buffer are concatenated to bufferText and bufferKey
            char buffer[1000];
            memset(buffer, '\0', 1000);
            memset(bufferText, '\0', plaintextLength);
            memset(bufferKey, '\0', keyLength);

            // Starts Receiving plaintext from client
            charsRead = recv(connectionSocket, buffer, 999, 0);
            if (charsRead < 0) {
                fprintf(stderr, "SERVER: ERROR reading from socket (getting plaintext)\n");
                // exits child process
                exit(1);
                break;
            }

            // Loops through receiving plaintext until client sends "key"
            while (strcmp(buffer, "key") != 0)
            {
                // Concatenates received string to bufferText
                strcat(bufferText, buffer);
                memset(buffer, '\0', 1000);
                charsRead = recv(connectionSocket, buffer, 999, 0);
                if (charsRead < 0) {
                    fprintf(stderr, "SERVER: ERROR reading from socket (getting plaintext)\n");
                    // exits child process
                    exit(1);
                    break;
                }
            }

            // Resets buffer
            memset(buffer, '\0', 1000);

            //Starts receiving key from client
            charsRead = recv(connectionSocket, buffer, 999, 0);
            if (charsRead < 0) {
                fprintf(stderr, "SERVER: ERROR reading from socket (getting key)\n");
                // exits child process
                exit(1);
                break;
            }

            // Loops through receiving key until client sends "!!"
            while (strcmp(buffer, "!!") != 0)
            {
                // Concatenates received string to bufferKey
                strcat(bufferKey, buffer);
                memset(buffer, '\0', 1000);
                charsRead = recv(connectionSocket, buffer, 999, 0);
                if (charsRead < 0) {
                    fprintf(stderr, "SERVER: ERROR reading from socket (getting key)\n");
                    // exits child process
                    exit(1);
                    break;
                }
            }

            // Arithmetic with ASCII character decimal values
            // Treats space character as having numerical value 0 in encryption calculation
            // In case a where the decimal value 64 (@ character) is assigned to bufferText[i], 
            // reassigns space character to bufferText[i]
            for (int i = 0; i < strlen(bufferText); i++)
            {
                // Does nothing if key character is space but text character is not space
                if (bufferKey[i] == ' ' && bufferText[i] != ' ')
                {
                }
                // Sets plaintext character to key character at index i, if key character is not a space 
                // but plaintext character is 
                else if (bufferKey[i] != ' ' && bufferText[i] == ' ')
                {
                    bufferText[i] = bufferKey[i];
                }
                // Does nothing if key character and plaintext character  are both space characters
                else if (bufferText[i] == ' ' && bufferKey[i] == ' ')
                {
                }
                // Encrypts plaintext via ASCII decimal value arithmetic and modulo 27 
                else
                {
                    bufferText[i] = bufferText[i] - 64;
                    bufferText[i] = bufferText[i] + bufferKey[i] - 64;
                    bufferText[i] = bufferText[i] % 27;
                    bufferText[i] = bufferText[i] + 64;
                    // @ character is treated as numerical value 0 and changed to space character
                    if (bufferText[i] == '@')
                    {
                        bufferText[i] = ' ';
                    }
                }
            }

            int countChars = 0;
            // Loops while the number of characters sent to the client is less than the encrypted plaintext
            while (countChars < strlen(bufferText))
            {
                charsWritten = send(connectionSocket, bufferText + countChars, 999, 0);
                if (charsWritten < 0) {
                    fprintf(stderr, "SERVER: ERROR writing to socket (sending cyphertext)\n");
                    // exits child process
                    exit(1);
                    break;
                }
                countChars = countChars + charsWritten;
            }
            // exits child process
            exit(0);
            break;
        }
        }
        // Inside parent process
        // Close connection
        close(connectionSocket);
    }
    // Close the listening socket
    close(listenSocket);
    return 0;
}