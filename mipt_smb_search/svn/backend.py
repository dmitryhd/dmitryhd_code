#!/usr/bin/env python3

#-*- coding: utf-8 -*-

import os
import re
import mount_smb
import codecs

CONFIG_FILE = 'indexer.conf'
MAX_DEPTH = 15

indexed_files = 0

#precompiled regexp for best performance
sq_bracket_re = re.compile('\[')
sq_bracket_end_re = re.compile('\]')
space_re = re.compile(' ')
video_re = re.compile('.*\.(avi|ogg|mkv|mp4)')

class FileInfo:
    """
    Semantics: This class contains information about indexing file
    name of file, full path to file, full path to preview
    constructor
    """
    def __init__ (self, name = None, fullpath = None, preview = None, host = None):
        self.name = name
        self.fullpath = fullpath
        self.preview = preview
        self.host = host


def fix_path(src_name):
    """
    Semantics: substitudes forbridden characters for unix file names, like [,],' '
    Arguments: src_name = string, which may contain forbridden characters
    Return: normalized string
    """
    # syntax note: to substitude A to B in STR
    # STR = A(regexp).sub(B, STR)
    src_name = sq_bracket_re.sub('\\\[', src_name)
    src_name = sq_bracket_end_re.sub('\\\]', src_name)
    src_name = space_re.sub('\\ ', src_name)
    return os.path.normpath(src_name)

def index_folder_rec(host, path, depth):
    """
    Semantics: recursively indexing path folder for video files
    Arguments: host, path, depth
    Return" list of files informations or null if it's not dir
    """
    if not os.path.isdir(path):
        return []
    # set limit to recursion depth
    if depth > MAX_DEPTH:
        return []
    # get down to new directory
    os.chdir(path)
    video_files = []
    print("indexing folder: ", path)
    # hash where key is file name, value is FileInfo class
    results ={}
    # subdirs of current working directory
    dirs = []

    for f in os.listdir("."):
        # in case of video file
        if os.path.isfile(f) and video_re.match(f):
            file_info = FileInfo()
            file_info.name = fix_path(f)
            # if we already found this file
            if file_info.name not in results:
                results[file_info.name] = file_info
            else:
                continue
            tmp_fullpath = os.path.join(path, f)
            file_info.fullpath = fix_path(os.path.dirname(tmp_fullpath))
            file_info.host = host
            #TODO(dmitryhd): Preview needed!
            file_info.preview = ""
            continue
        if os.path.isdir(f):
            dirs.append(f)

    # for all subdirs
    for dir_entry in dirs:
        video_files.append(index_folder_rec(host, os.path.join(path, dir_entry),
                    depth+1))

    for name, finfo in results.items():
        video_files.append(finfo)

    # get back to upper directory
    os.chdir("..")
    return video_files


def read_config():
    """
    Semantics: read CONFIG_FILE for hosts to scan
    if there is no any - get error
    Arguments: None
    Return: list of hosts
    """
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
    """
    Semantics: scan all shares in config for video files
    Arguments: None
    Return: list of FileInfo classes
    """
    shares = read_config()
    all_files = []
    for share in shares:
        print("share:", share)
        # comment
        if share[0] == '#':
            continue
        # local folder
        if share[0] == '/' and share[1] != '/':
            mount_point = share
            host = 'localhost'
            file_list = index_folder_rec(host, mount_point, 0)
        # network share
        else:
            # mounting
            mount_point = mount_smb.mount_smb(share)
            host, ip = mount_smb.get_ip_and_host(share)
            # find video files
            file_list = index_folder_rec(host, mount_point, 0)
            mount_smb.umount_smb(mount_point)
        for f in file_list:
            all_files.append(f)
    return all_files


def main():
    """
    Semantics: test fuction
    """
    # get all video files
    file_list = get_files_video()
    # test output
    for f in file_list:
        print(f.name)
        print(f.fullpath)
        print(f.host)
    exit(0)

if __name__ == '__main__':
    main()
