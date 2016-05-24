#!/bin/sh
if mkdir /var/lock/mylock; then
  echo "Locking succeeded" >&2
else
  echo "Lock failed - exit" >&2
  exit 1
fi
sudo ./client 66.215.199.45 65500 && rm -r /var/lock/mylock/ && sudo poweroff
