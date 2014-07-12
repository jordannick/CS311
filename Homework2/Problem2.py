# Nicholas Jordan
# CS 311
# Assignment 2 Part 2

import os
import sys
import getopt


def main():
    
    if len(sys.argv)>4:
        path=make_dir()
        make_links(path)
    else:
        print "Requires two options with arguments:\n -t [term name] or --term [term name]\n -c [class name] or --class [class name]"

def make_dir():
    try:
        optlist, args =  getopt.getopt(sys.argv[1:],"t:c:",["term=","class="]) 
    except getopt.GetoptError as err:
        print str(err)
        sys.exit(2)

    for option, value in optlist:
        if option == '-t' or option == '--term':
            termname = value
        if option == '-c' or option == '--class':
            classname = value

    path = termname+"/"+classname+"/"

    if os.path.exists(path):
        print "Directory already exists: "+path
    else:
        folders = ["assignments", "examples", "exams", "lecture_notes", "submissions"]
        for x in folders:
            os.makedirs(path + x)

    return path

def make_links(path):

    symlink_path = "usr/local/classes/eecs/"+path

    if os.path.islink("README"):
        print "Symbolic link already exists: "+symlink_path+"README"
    else:
        os.symlink(symlink_path+"README", "README")

    if os.path.islink("class_src"):
         print "Symbolic link already exists: "+symlink_path+"src"
    else:
        os.symlink(symlink_path+"src", "class_src")


if __name__ == "__main__":
    main()

