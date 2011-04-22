#!/usr/bin/env python3

#-*- coding: utf-8 -*-

import os
import re
import sys
import mount_smb
import codecs

INDEX_DIR = '/srv/http/cgi-bin/'
PREVIEW_DIR = '/srv/http/'
CONVERTER_COMMAND = "ffmpeg -y -s cif -sameq -ss 20 -t 26 -i " 
CONFIG_FILE = 'indexer.conf'
indexed_files = 0

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


def index_folder(host, path):
    """indexing path folder for video files"""
    """return list of files informations or null if it's not dir"""
    if not os.path.isdir(path):
        return []
    os.chdir(path)
    video_files = []
    video_re = re.compile('.*\.(avi|ogg|mkv|mp4)')
    
    results ={}
    indexed_files = 0
    prev_indexed_files = 0
    root = ""
    for dirpath, dirnames, filenames in os.walk("."):
        if prev_indexed_files != indexed_files:
            sys.stderr.write("%d files\n"%indexed_files)
        prev_indexed_files = indexed_files
        for f in filenames:
            tmp_fullpath = os.path.join(dirpath,f)
            sys.stderr.write(tmp_fullpath)
            if video_re.match(f):
                file_info = FileInfo()
                file_info.name_ = fix_path(f)
                if file_info.name_ not in results:
                    results[file_info.name_] = file_info
                else:
                    continue
                indexed_files += 1
                file_info.fullpath_ = fix_path(os.path.dirname(tmp_fullpath))
                file_info.host_ = host
#TODO(dmitryhd): Preview needed!
                file_info.preview_ = ""
    print(results)
    for name, finfo in results.items():
        video_files.append(finfo)
    return video_files

def read_config():
    if not os.path.exists(CONFIG_FILE):
        print("config file don't exists")
        exit(2)
    f = codecs.open(CONFIG_FILE, encoding='utf-8')
    shares = []
    line = f.readline()
    while line:
        if line == "\n":
            line = f.readline()
            continue
        line = re.compile('\n').sub('', line)
        shares.append(line)
        line = f.readline()
    f.close()
    return shares

def get_files_video():
    shares = read_config()
    all_files = []
    for share in shares:
        print("share:", share)
        if share[0] == '#':
            continue
        if share[0] == '/' and share[1] != '/':
            mount_point = share
            host = 'localhost'
            file_list = index_folder(host, mount_point)
        else:
            mount_point = mount_smb.mount_smb(share)
            host, ip = mount_smb.get_ip_and_host(share)
            file_list = index_folder(host, mount_point)
            mount_smb.umount_smb(mount_point)
        for f in file_list:
            all_files.append(f)
    return all_files


def main():
    # indexing mode
    file_list = get_files_video()
    for f in file_list:
        print(f.name_)
        print(f.fullpath_)
        print(f.host_)
    exit(0)

if __name__ == '__main__':
    main()
