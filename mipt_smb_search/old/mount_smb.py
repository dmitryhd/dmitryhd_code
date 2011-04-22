#!/usr/bin/env python3
#-*- coding: utf-8 -*-

import re
import os
import sys
import socket

SMB_MOUNT_DIR = "/tmp/smb/"
MOUNT_COMMAND = "sudo mount.cifs %(share)s %(mount_point)s -o guest"
UMOUNT_COMMAND = "sudo umount %s"


def get_ip_and_host(sharename):
    host = ''
    share_ip = ''
    ip_re = re.compile('(\d){,3}\.(\d){,3}.(\d){,3}\.(\d){,3}')
    match_res = ip_re.search(sharename)
    if match_res == None:
        host_re = re.compile('[\w|\.]+/')
        match_res = host_re.search(sharename)
        host = match_res.group()
        slash_re = re.compile('/')
        host = slash_re.sub('', host)
        share_ip = socket.gethostbyname(host)
    else:
        share_ip = match_res.group()
    return (host, share_ip)


def mount_smb(sharename):
    host, share_ip = get_ip_and_host(sharename)
    print("try to mount ", sharename, " ip =", share_ip, "host = ", host)

    if not os.path.isdir(SMB_MOUNT_DIR):
        os.mkdir(SMB_MOUNT_DIR)
    os.chdir(SMB_MOUNT_DIR)

    if not host:
        mount_point = os.path.join(SMB_MOUNT_DIR, share_ip)
    else:
        mount_point = os.path.join(SMB_MOUNT_DIR, host)

    if not os.path.isdir(mount_point):
        os.mkdir(mount_point)
    os.chdir(SMB_MOUNT_DIR)
    mount_comm = str(MOUNT_COMMAND % {'share':sharename, 'mount_point': mount_point})
    os.system(str(UMOUNT_COMMAND % mount_point))
    print("try to :", mount_comm)
    os.system(mount_comm)
    return mount_point
    
def umount_smb(mount_point):
    os.system(str(UMOUNT_COMMAND % mount_point))

def main():
    if len(sys.argv) != 2:
        usage()
        exit(1)
    mount_point = mount_smb(sys.argv[1])
    os.system("ls -l " + mount_point)
    exit(0)

if __name__ == "__main__":
    main()

def usage():
    print("usage: ", sys.argv[0], " sharename")
