# encrypt-decrypt

This project features plaintext encryption/decryption over C-programmed local server/client pipelines. The server and client interactions happen over a system's local ports. Program execution is done in a command-line interface.

Note: currently only plaintext files containing only upper-case alphabetical letters and spaces are supported for encryption and decryption. 

## Instructions
Required: a GCC / C Compiler must be installed to compile this program.

I developed/tested this project using bash and included a couple bash scripts, so I recommend compiling and executing this project within a bash shell. <br />

## Bash Scripting
I have included bash scripts for compiling and executing this project.

Navigate to the directory containing keygen.c, enc_server.c, enc_client.c, dec_server.c, dec_client.c, and bash scripts.

To compile the project, enter ```./compileall``` into bash. <br />
To execute the project, enter ```./runall -e {encyption server port} -d {decryption server port} -s {your plaintext file}``` <br />
Example using the sample plaintext file: ```./runall -e 18501 -d 23501 -s plaintext```
<br />

The script will run the encryption and decryption servers and establish connections with the client programs. As a result, three new plaintext files are 
generated: key (containing the random key), ciphertext (the encrypted version of the original plaintext), and dicipheredtext (the dicphered version of ciphertext).
<br />

decipheredtext and the original file should contain the same content if the project executed succesfully.

## Manual Compilation and Execution (not recommended)

Navigate to the directory containing keygen.c, enc_server.c, enc_client.c, dec_server.c, and dec_client.c.
To compile enter:
```
gcc -std=gnu99 -o enc_server enc_server.c
gcc -std=gnu99 -o enc_client enc_client.c
gcc -std=gnu99 -o dec_server dec_server.c
gcc -std=gnu99 -o dec_client dec_client.c
gcc -std=gnu99 -o keygen keygen.c
```

Start the encryption and decryption servers by executing:

```./enc_server {listening_port} &``` where listening_port is the port enc_server listens on <br />
```./dec_server {listening_port} &``` where listening_port is the port dec_server listens on <br />

Your chosen ports must be different between the server programs.
I recommend choosing random ports anywhere from 10000 - 65000. 
Keep track of which ports you have chosen. <br />

Example: 
```
./enc_server 57171 & 
./dec_server 19823 &
```

Note: your system's network firewall might block some features of these server programs and you will need to provide permission. On Window's systems you might get a Windows Security Alert popup and you will need to click Allow access.

The encryption and decryption servers should now each be listening on a port and ready to establish local client connections.

<br />Before you can start encrypting and decrypting plaintext. You need to create a randomly generated key. 

Execute: ```./keygen {keylength} > {keyfile}``` where keylength is length of the key and keyfile is the name of file output.

Example: 
```
./keygen 256 > mykey
```

<br />

#### Encrypting:

You can now start encrypting and decrypting plaintext files via local port connections.
<br /> 
<br />Before continuing:<br />
Recall the designated ports that the servers are running on. 
If you have not already, create a plaintext file in the local directory.<br />
Note: Again, only uppercase alphabetical letters and spaces are supported for encryption/decryption. 

To have your plaintext encrypted and outputted to a file execute: 
```
./enc_client {plaintext} {key} {enc_server port} > {ciphertext}
```
where plaintext is file whose contents are to be encrypted, key is the file containing your key, enc_server port is the local port where enc_server is running, and ciphertext is an output file which will contain your encrypted plaintext <br />
<br />
Alternatively, To have the contents of your plaintext file encrypted and outputted to the command-line execute:
```
./enc_client {plaintext} {key} {enc_server port}
```
<br />

#### Decrypting 
To decrypt your ciphertext execute:
```
./dec_client {ciphertext} {key} {port} > {decipheredtext}
```
where ciphertext is your file containing encrypted plaintext, key is the file containing your key, dec_server port is the local port where dec_server is running, and decipheredtext is an output file which will contain your decrypted plaintext 

<br />
<br />
Full example using my previous designated ports and naming schemes:

```
./enc_client myplaintext mykey 57171 > myciphertext
./dec_client myciphertext mykey 19823 > mydecipheredtext
```

<br /> At this point, you can compare the contents of your original myplaintext and new mydecipheredtext files. After encrypting and decrypting your original plaintext, myplaintext and mydecipheredtext should have the same contents. Awesome, you have successfully encrypted and decrypted your plaintext using this project.

