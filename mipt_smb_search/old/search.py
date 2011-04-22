#!/usr/bin/env python3

#-*- coding: utf-8 -*-

import re
import print_page
import engine
import cgi, cgitb 

def main():
    # searching
    # Create instance of FieldStorage 
    form = cgi.FieldStorage() 
    request = form.getvalue('request')
    if not request:
        request = "."
    database = engine.load_database(True)
    request_re = re.compile(".*" + request + ".*", re.IGNORECASE)

    results = []
    for file_info in database:
        if request_re.match(file_info.name_):
            results.append(file_info)
    print_page.head(request)
    print_page.results(results)
    print_page.tail()

    exit(0)

if __name__ == '__main__':
    cgitb.enable()
    main()

