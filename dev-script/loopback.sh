#!/usr/bin/bash

#create file for backup
sudo iptables-save > iptablesBackup

#news rules
sudo iptables -t nat -I PREROUTING --src 0/0 --dst 127.0.0.1 -p tcp --dport 22222 -j REDIRECT --to-ports 23333
sudo iptables -t nat -I OUTPUT --src 0/0 --dst 127.0.0.1 -p tcp --dport 22222 -j REDIRECT --to-ports 23333

#restore initial parameters from testfile
sudo iptables-restore < iptablesBackup

#delete backup file
sudo rm iptablesBackup

