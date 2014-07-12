# Nicholas Jordan
# CS 311
# Assignment 2 Part 5

import subprocess
import shlex
import getopt
import sys

input = 'w'

try:
    optlist,args =  getopt.getopt(sys.argv[1:],"husfV",[]) 
except getopt.GetoptError as err:
    print str(err)
    sys.exit(2)

for option, value in optlist:
    if option == '-h':
        input+=' -h'
    if option == '-u':
        input+=' -u'
    if option == '-s':
        input+=' -s'
    if option == '-f':
        input+=' -f'
    if option == '-V':
        input+=' -V'

cmd = shlex.split(input)

p=subprocess.Popen(cmd)
p.communicate()
