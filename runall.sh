#!/bin/bash

while getopts "e:d:s:" arg; do
  case $arg in
    e) enc=$OPTARG;;
    d) dec=$OPTARG;;
    s) src=$OPTARG;;
  esac
done

function getSrcLength {
    echo $(wc -m "$src");
}
length=$(getSrcLength)

./enc_server "$enc" &
enc_pid=$!
./dec_server "$dec" &
dec_pid=$!

./keygen $length > key.txt
./enc_client "$src" key.txt "$enc" > ciphertext.txt
./dec_client ciphertext.txt key.txt "$dec" > decipheredtext.txt

kill $enc_pid
kill $dec_pid