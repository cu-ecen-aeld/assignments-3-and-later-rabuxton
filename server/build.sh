#!/bin/sh

make clean
make all
rm -rf /var/tmp/aesdsocketdata
./aesdsocket $1