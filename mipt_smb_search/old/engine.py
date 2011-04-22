#!/usr/bin/env python3
#-*- coding: utf-8 -*-

import os
import re
import sys
import socket
import codecs


INDEX_DIR = '/srv/http/cgi-bin/'
PREVIEW_DIR = '/srv/http/'
INDEX_FILE = 'index.dat'
CONVERTER_COMMAND = "ffmpeg -y -s cif -sameq -ss 20 -t 26 -i " 

SMB_MOUNT_DIR = "/tmp/smb/"
MOUNT_COMMAND = "sudo mount.cifs %(share)s %(mount_point)s -o guest"
UMOUNT_COMMAND = "sudo umount %s"


def mount_smb(sharename):
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
        print("host = ", host)
        share_ip = socket.gethostbyname(host)
    else:
        share_ip = match_res.group()

    print("try to mount ", sharename, " ip =", share_ip)

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
    mount_comm = str(MOUNT_COMMAND % {'share':sys.argv[1], 'mount_point': mount_point})
    os.system(str(UMOUNT_COMMAND % mount_point))
    print("try to :", mount_comm)
    os.system(mount_comm)
    return mount_point

def main():
    if len(sys.argv) != 2:
        usage()
        exit(1)
    mount_point = mount_smb(sys.argv[1])
    os.system("ls -l " + mount_point)
    exit(0)

if __name__ == "__main__":
    main()


class FileInfo:
    name_ = ""
    fullpath_ = "" # on network share
    host_ = "" # ip of host
    preview_ = ""

    def __init__ (self):
        name = "."

def fix_path(src_name):
    p = re.compile('\[')
    src_name = p.sub('\\\[', src_name)
    p = re.compile('\]')
    src_name = p.sub('\\\]', src_name)
    p = re.compile(' ')
    src_name = p.sub('\\ ', src_name)
    return os.path.normpath(src_name)


def index_folder(path):
    """indexing path folder for video files"""
    """return list of files informations or null if it's not dir"""
    if not os.path.isdir(path):
        return []
    os.chdir(path)
    video_files = []
    video_re = re.compile('.*\.(avi|ogg|mkv|mp4)')

    for dirpath, dirnames, filenames in os.walk("."):
        for f in filenames:
            if video_re.match(f):
                file_info = FileInfo()
                file_info.name_ = fix_path(f)
                file_info.fullpath_ = os.path.join(dirpath, f)
                file_info.fullpath_= fix_path(os.path.abspath(file_info.fullpath_))
#TODO(dmitryhd):......
#getting preview
                convert_command = str( CONVERTER_COMMAND + 
                        file_info.fullpath_  + 
                        " " + 
                        PREVIEW_DIR + 
                        "preview-" + 
                        file_info.name_ +
                        ".ogg")
                print("try to", convert_command)
                os.system(convert_command)
                file_info.preview_ = "preview-" + file_info.name_ + ".ogg"
                video_files.append(file_info)
    return video_files


def load_database(file_required):
    """loading file database"""
    """if there is no database stored, if file_required - fall in error, else-exit"""
    """return list of files informations"""
    os.chdir(INDEX_DIR)
    if not os.path.exists(INDEX_FILE):
        if file_required:
            print("index don't exists. try -i")
            exit(2)
        else:
            return []
    f = codecs.open(INDEX_FILE, encoding='utf-8')
    database = []
    line = f.readline()
    while line:
        if line == "\n":
            line = f.readline()
            continue
        splitted_line = line.split('  ')
        file_info = FileInfo()
        file_info.name_ = splitted_line[0]
        file_info.fullpath_ = splitted_line[1]
        file_info.preview_ =  splitted_line[2]
        database.append(file_info)
        line = f.readline()
    f.close()
    return database


def store_database(database):
    """saving file database"""
    """if there is no database stored, we create one"""
    os.chdir(INDEX_DIR)
    f = open(INDEX_FILE,'w')
    for file_info in database:
        f.write(file_info.name_ + "  " + file_info.fullpath_ + "  " + 
                file_info.preview_ + '\n')
    f.close()
