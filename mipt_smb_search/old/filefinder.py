#!/usr/bin/env python3

#-*- coding: utf-8 -*-

import os
import re
import sys
import pickle

TMP_DIR = '/tmp/'
INDEX_DIR = './'
INDEX_FILE = 'index.dat'


class FileInfo:
    name_ = ""
    fullpath_ = ""
    def __init__ (self, name, fullpath):
        name_ = name
        fullpath_ = fullpath


def index_folder(path):
    """indexing path folder for video files"""
    """return list of files informations"""
    os.chdir(path)
    video_files = []
    video_re = re.compile('.*\.(avi|ogg|mkv|mp4)')

    for dirpath, dirnames, filenames in os.walk("."):
        for f in filenames:
            if video_re.match(f):
                file_info = FileInfo(f, os.path.abspath(f))
                file_info.name_ = f
                file_info.fullpath_ = os.path.abspath(f)
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
    f = open(INDEX_FILE, "r")
    database = []
    line = f.readline()
    while line:
        if line == "\n":
            line = f.readline()
            continue
        splitted_line = line.split(' ')
        file_info = FileInfo(splitted_line[0], splitted_line[1])
        file_info.name_ = splitted_line[0]
        file_info.fullpath_ = splitted_line[1]
        database.append(file_info)
        line = f.readline()
    f.close()
    return database


def store_database(database):
    """saving file database"""
    """if there is no database stored, we create one"""
    os.chdir(INDEX_DIR)
    f = open(INDEX_FILE, "w")
    for file_info in database:
        f.write(file_info.name_ + " " + file_info.fullpath_ + '\n')
    f.close()

def usage():
    print("usage:", sys.argv[0], "-s(earch) request")
    print("or usage:", sys.argv[0], "-i(ndex) directory")


def main():
    if len(sys.argv) < 3:
        usage()
        exit(1)
    # searching mode
    if sys.argv[1] == "-s":
        database = load_database(True)
        request_re = re.compile(".*" + sys.argv[2] + ".*")
        for file_info in database:
            if request_re.match(file_info.name_):
                print(file_info.name_)
        exit(0)

    # indexing mode
    if sys.argv[1] == "-i":
        database_old = load_database(False)
        database_new = index_folder(sys.argv[2])
        for file_info_new in database_new:
            file_presented = False
            for file_info_old in database_old:
                if file_info_new.name_ == file_info_old.name_:
                    file_presented = True
                    break
            if not file_presented:
                database_old.append(file_info_new)
        store_database(database_old)
        exit(0)
    usage()
    exit(0)

if __name__ == '__main__':
    main()
