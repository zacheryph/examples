#!/bin/sh
echo -n "password: "
stty -echo
read pass
echo
stty echo
echo "your password is: $pass"
