#!/bin/bash
#
# This shell script is only an example of how the same type of
# processing as is required for Homweork #5 can be done in a Bash
# script. 
#
# example.bash JUNK.txt < simple2.txt
#
# rev | sort | uniq -c | tee <blaa> | wc
#
#
# Jesse

rev | sort | uniq -c | tee $2| wc

