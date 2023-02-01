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

    // Start listening for connetions. Allow up to 5 connections to queue up
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

            // Read the client's message from the socket
            charsRead = recv(connectionSocket, connCheck, sizeof(connCheck) - 1, 0);
            if (charsRead < 0) {
                fprintf(stderr, "SERVER: ERROR reading from socket (checking connection)\n");
                exit(1);
                break;
            }

            // Verifies client
            char accept[10] = "accepted";
            char reject[10] = "rejected";
            if (strcmp(connCheck, "dec client") != 0)
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

            // Receives length of ciphertext from client
            charsRead = recv(connectionSocket, ptLength, sizeof(ptLength) - 1, 0);
            if (charsRead < 0) {
                fprintf(stderr, "SERVER: ERROR reading from socket (getting ciphertext length)\n");
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

            // Starts Receiving ciphertext from client
            charsRead = recv(connectionSocket, buffer, 999, 0);
            if (charsRead < 0) {
                fprintf(stderr, "SERVER: ERROR reading from socket (getting plaintext)\n");
                // exits child process
                exit(1);
                break;
            }
            
            // Loops through receiving ciphertext until client sends "key"
            while (strcmp(buffer, "key") != 0)
            {
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

            //Starts Receiving key from client
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


            // Decrypts each character in bufferText
            // Using bufferText char and bufferKey char performs arithmetic and 
            // modulo 95 operation to calculate new ASCII decimal value for character
            // Reassigns calculated ASCII character to bufferText[i]
            for (int i = 0; i < strlen(bufferText); i++)
            {
                char ct = bufferText[i];
                char kt = bufferKey[i];
                int t = (int)bufferText[i];
                int k = (int)bufferKey[i];
                k = k - 32;
                t = t - 32; 
                t = t - k;

                if (t < ' ');
                {
                    t = t + 95;
                }
                t = t % 95;
                t = t + 32;
                bufferText[i] = (char)t;
            }
            // {
            //     // Does nothing if key character is space but text character is not space
            //     if (bufferKey[i] == ' ' && bufferText[i] != ' ')
            //     {
            //     }
            //     // Does nothing if key character and plaintext are both space characters
            //     else if (bufferText[i] == ' ' && bufferKey[i] == ' ')
            //     {
            //     }
            //     // Decrypts ciphertext via ASCII decimal value arithmetic and modulo 27 
            //     else
            //     {
            //         // Assigns @ character to ciphertext at index i, if it is a space character
            //         if (bufferText[i] == ' ')
            //         {
            //             bufferText[i] = '@';
            //         }
            //         bufferText[i] = bufferText[i] - 64;
            //         bufferText[i] = bufferText[i] - (bufferKey[i] - 64);

            //         // If ciphertext character has an ASCII decimal value less than @, add 27 to the character's decimal value;
            //         if (bufferText[i] < '@')
            //         {
            //             bufferText[i] = bufferText[i] + 27;
            //         }
            //         bufferText[i] = bufferText[i] % 27;
            //         bufferText[i] = bufferText[i] + 64;
            //         // @ character is treated as numerical value 0 and changed to space character
            //         if (bufferText[i] == '@')
            //         {
            //             bufferText[i] = ' ';
            //         }
            //     }
            // }

            int countChars = 0;
            // Loops while the number of characters sent to the client is less than the decrypted plaintext
            while (countChars < strlen(bufferText))
            {
                charsWritten = send(connectionSocket, bufferText + countChars, 999, 0);
                if (charsWritten < 0) {
                    fprintf(stderr, "CLIENT: ERROR writing to socket\n");
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