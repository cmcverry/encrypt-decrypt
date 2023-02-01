#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>  // ssize_t
#include <sys/socket.h> // send(),recv()
#include <netdb.h>      // gethostbyname()


// Set up the address struct
void setupAddressStruct(struct sockaddr_in* address,
    int portNumber, char* hostname) {

    // Clear out the address struct
    memset((char*)address, '\0', sizeof(*address));

    // The address should be network capable
    address->sin_family = AF_INET;
    // Store the port number
    address->sin_port = htons(portNumber);

    // Get the DNS entry for this host name
    struct hostent* hostInfo = gethostbyname(hostname);
    if (hostInfo == NULL) {
        fprintf(stderr, "CLIENT: ERROR, no such host");
        exit(1);
    }
    // Copy the first IP address from the DNS entry to sin_addr.s_addr
    memcpy((char*)&address->sin_addr.s_addr,
        hostInfo->h_addr_list[0],
        hostInfo->h_length);
}

int main(int argc, char* argv[]) {
    // goodCharacters string contains all valid characters 
    char* goodCharacters = " ABCDEFGHIJKLMNOPQRSTUVWXYZ";
    int socketFD, portNumber, charsWritten, charsRead;
    struct sockaddr_in serverAddress;

    // Check usage & args
    if (argc < 4) {
        fprintf(stderr, "USAGE: %s hostname port\n", argv[0]);
        exit(1);
    }

    // Initializes stat structs 
    struct stat keyInfo;
    struct stat textInfo;
    // Opens and creates file descriptors for file arguments 
    FILE* fdText = fopen(argv[1], "r");
    FILE* fdKey = fopen(argv[2], "r");

    // Passes file arguments to stat structs textInfo and keyInfo
    stat(argv[1], &textInfo);
    stat(argv[2], &keyInfo);

    // Assigns size of file arguments to textSize and keySize
    int textSize = textInfo.st_size;
    int keySize = keyInfo.st_size;

    // Initializes empty strings of size textSize
    char bufferText[textSize];
    char lineText[textSize];
    memset(bufferText, '\0', sizeof(bufferText));
    memset(lineText, '\0', sizeof(lineText));

    // Loops and extracts lines from ciphertext file while not EOF
    while (fgets(lineText, sizeof(lineText) - 1, fdText))
    {
        // Concatenates extracted line to bufferText
        strcat(bufferText, lineText);
        memset(lineText, '\0', sizeof(lineText));
    }
    // checks for new line character in bufferText and replaces it with null character 
    bufferText[strcspn(bufferText, "\n")] = '\0';

    // Initializes empty strings of size keySize
    char bufferKey[keySize];
    char lineKey[keySize];
    memset(bufferKey, '\0', sizeof(bufferKey));
    memset(lineKey, '\0', sizeof(lineKey));

    // Loops and extracts lines from key file while not EOF
    while (fgets(lineKey, sizeof(lineKey) - 1, fdKey))
    {
        strcat(bufferKey, lineKey);
        memset(lineKey, '\0', sizeof(lineKey));
    }
    // checks for new line character in bufferText and replaces it with null character
    bufferKey[strcspn(bufferKey, "\n")] = '\0';

    // Verifies that only valid characters are in key
    // For every character in bufferKey, that character is checked
    // against all characters in goodCharacters
    // If that bufferKey character is not found in goodCharacters
    // then bufferKey has invalid character(s)
    for (int i = 0; i < strlen(bufferKey); i++)
    {
        // If validChar is not set to 0 in the inner for loop
        // then that bufferKey[i] is not a valid character
        int validChar = 1;
        for (int j = 0; j < strlen(goodCharacters); j++)
        {
            if (bufferKey[i] == goodCharacters[j])
            {
                validChar = 0;
            }
        }
        if (validChar == 1)
        {
            fprintf(stderr, "CLIENT: ERROR Key contains invalid character\n");
            exit(1);
        }
    }

    // Verifies that only valid characters are in plaintext
    // For every character in bufferText, that character is checked
    // against all characters in goodCharacters
    // If that bufferText character is not found in goodCharacters
    // then bufferText has invalid character(s)
    for (int i = 0; i < strlen(bufferText); i++)
    {
        // If validChar is not set to 0 in the inner for loop
    // then that bufferText[i] is not a valid character
        int validChar = 1;
        for (int j = 0; j < strlen(goodCharacters); j++)
        {
            if (bufferText[i] == goodCharacters[j])
            {
                validChar = 0;
            }
        }
        if (validChar == 1)
        {
            fprintf(stderr, "CLIENT: ERROR plaintext contains invalid character\n");
            exit(1);
        }
    }

    // Checks that key is not smaller than ciphertext
    if (strlen(bufferKey) < strlen(bufferText))
    {
        fprintf(stderr, "CLIENT: ERROR Key is too small\n");
        exit(1);
    }

    // Create a socket
    socketFD = socket(AF_INET, SOCK_STREAM, 0);
    if (socketFD < 0) {
        fprintf(stderr, "CLIENT: ERROR opening socket\n");
        exit(1);
    }

    // Set up the server address struct
    char host[10] = "localhost";
    setupAddressStruct(&serverAddress, atoi(argv[3]), host);

    // Connect to server
    if (connect(socketFD, (struct sockaddr*)&serverAddress, sizeof(serverAddress)) < 0) {
        fprintf(stderr, "CLIENT: ERROR connecting to %s\n", argv[3]);
        exit(2);

    }

    // Intializes strings 
    char* firstMessage = "dec client";
    char plaintextLength[11];
    char keyLength[11];
    // Converts keySize and textSize to strings, appended to plaintextLength and keyLength
    sprintf(keyLength, "%d", keySize);
    sprintf(plaintextLength, "%d", textSize);

    // Sends "dec client" to server; used by server to verify compatible connection
    charsWritten = send(socketFD, firstMessage, strlen(firstMessage), 0);
    if (charsWritten < 0) {
        fprintf(stderr, "CLIENT: ERROR writing to socket\n");
        exit(1);
    }

    char accept[9] = "accepted";
    char checkAccept[10];
    charsRead = recv(socketFD, checkAccept, sizeof(checkAccept) - 1, 0);
    if (strcmp(checkAccept, accept) != 0)
    {
        fprintf(stderr, "CLIENT: ERROR connecting with %s rejected by server\n", argv[3]);
        exit(2);
    }

    // Sends plaintextLength (ciphertext) to server
    charsWritten = send(socketFD, plaintextLength, sizeof(plaintextLength) - 1, 0);
    if (charsWritten < 0) {
        fprintf(stderr, "CLIENT: ERROR writing to socket\n");
        exit(1);
    }

    // Sends keyLength to server
    charsWritten = send(socketFD, keyLength, sizeof(keyLength) - 1, 0);
    if (charsWritten < 0) {
        fprintf(stderr, "CLIENT: ERROR writing to socket\n");
        exit(1);
    }

    int countChars = 0;
    // Loops while the number of characters sent to the server is less than length of bufferText
    while (countChars < strlen(bufferText))
    {
        // Sends bufferText in 999 character messages
        charsWritten = send(socketFD, bufferText + countChars, 999, 0);
        if (charsWritten < 0) {
            fprintf(stderr, "CLIENT: ERROR writing to socket\n");
            exit(1);
        }
        countChars = countChars + charsWritten;
    }

    // Sends "key" to server; effectively, tells server that plaintext has all sent
    charsWritten = send(socketFD, "key", 999, 0);
    if (charsWritten < 0) {
        fprintf(stderr, "CLIENT: ERROR writing to socket\n");
        exit(1);
    }


    countChars = 0;
    // Loops while the number of characters sent to the server is less than length of bufferKey
    while (countChars < strlen(bufferKey))
    {
        // Sends bufferKey in 999 character messages
        charsWritten = send(socketFD, bufferKey + countChars, 999, 0);
        if (charsWritten < 0) {
            fprintf(stderr, "CLIENT: ERROR writing to socket\n");
            exit(1);
        }
        countChars = countChars + charsWritten;
    }

    // Sends "!!" to server; effectively, tells server that key transmission is complete
    charsWritten = send(socketFD, "!!", 999, 0);
    if (charsWritten < 0) {
        fprintf(stderr, "CLIENT: ERROR writing to socket\n");
        exit(1);
    }

    // Intializes '\0' filled string array for taking encrypted plaintext sent back from server
    char bufferEncText[(strlen(bufferText) + 2)];
    memset(bufferEncText, '\0', sizeof(bufferEncText));
    // buffer used for receiving msgs
    // msgs are concatened to bufferEncText
    char buffer[1000];
    memset(buffer, '\0', 1000);

    // Receive decrypted plaintext from server
    countChars = 0;
    // Loops while the number of received characters is less than the less of bufferText
    while (countChars < strlen(bufferText))
    {
        charsRead = recv(socketFD, buffer, 999, 0);
        strcat(bufferEncText, buffer);
        memset(buffer, '\0', 1000);
        countChars = countChars + charsRead;
    }

    // Outputs decrypted plaintext with new line character at end to stdout
    printf("%s\n", bufferEncText);


    // Close the socket
    close(socketFD);
    return 0;
}