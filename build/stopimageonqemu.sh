
#!/bin/bash

PID=`sudo ps a | awk '$5~/qemu-system-i386$/ {print $1}'`

# Once the process's PID is known, its termination is very trivial:
sudo kill $PID
