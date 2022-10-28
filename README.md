# encrypt-decrypt

This program will encrypt and decrypt plaintext into ciphertext, using a key, using modulo 27 operations on 
27 characters that include 26 capital letters and the space character. All 27 characters will be encrypted and decrypted.
Generated ciphertext can be decrypted through dec_server and dec_client interations. 

Using the Bash Unix Shell navigate to the directory containing keygen.c, enc_server.c, enc_client.c, dec_server.c, dec_client.c, and compileall script.

To compile the program, enter './compileall' into Bash.

Start servers by entering into Bash:

'enc_server listening_port', where listening_port is the port enc_server listens on
'dec_server listening_port', where listening_port is the port enc_server listens on

Example: enc_server 57171 &


A key file must be created to be used in the client-server interaciton.
Enter into Bash:

'keygen keylength > key' where keylength is length of the key and key is the filed outputted to

Example: keygen 256 > mykey


Use a client by entering into Bash:

'enc_client plaintext key port' where plaintext is your file to be encrypted, key is your encryption key, and port is the server port to be connected to
'dec_client cyphertext key port' where ciphertext is your file to be decrypted, key is your encryption key, and port is the server port to be connected to

Example:
'enc_client myplaintext mykey 57171'
or 'enc_client myplaintext mykey 57171 > myciphertext'
or 'enc_client myplaintext mykey 57171 > myciphertext &'
