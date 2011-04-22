#!/usr/bin/env python3 -u
#-*- coding: utf-8 -*-


def insert_video(name):
    print("""<video controls="controls" height="200" width="300">
    <source src="../%s"
    type="video/ogg" />
    Your browser does not support the video tag.</video>"""%name)

def head(request):
    print("Content-Type: text/html\n")
    print("""
    <!DOCTYPE HTML>
    <html>
    <head>
    <meta http-equiv="Content-Type" content="text/html; charset=utf-8">
    </head>
    <body>
    <center><h1>MIPT SMB SEARCH</h1><center>
    <form name="search" method="post" action="./search.py">
    Searching for: <b>%s</b>
    <hr>
    <input name="request" type="text" size="60">
    <input type="submit" e>
    """ % request)

def results(results):
    print("<hr><table>")
    row = 0
    for file_info in results:
        if row == 3:
            print("""</tr>""")
            row = 0
        if row == 0: 
            print("""<tr bgcolor="#f0f0f0">""")
        print("""<td>""", file_info.name_, "<br>")
        insert_video(file_info.preview_)
        print("</td>")
        row = row + 1
    

def tail():
    print("""""")
    print("""</body></html>""")
