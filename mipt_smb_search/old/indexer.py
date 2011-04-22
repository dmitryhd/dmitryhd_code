#!/usr/bin/env python3

#-*- coding: utf-8 -*-

import sys
import engine


def main():
    # indexing mode
    if len(sys.argv) != 2:
        print("wrong usage")
        exit(0)
    database_old = engine.load_database(False)
    database_new = engine.index_folder(sys.argv[1])
    for file_info_new in database_new:
        file_presented = False
        for file_info_old in database_old:
            if file_info_new.name_ == file_info_old.name_:
                file_presented = True
                break
        if not file_presented:
            database_old.append(file_info_new)
    engine.store_database(database_old)
    for f in database_old:
        print(f.name_)
    exit(0)

if __name__ == '__main__':
    main()
