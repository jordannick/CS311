#Nicholas Jordan
#CS 311
#HW1 Part 8

import math
import sys

def sieve(n):

    list = [] # list for all numbers 0 thru n
    primes = [] # list for primes
    nprimes=0 # number of primes

    for x in range(0,n):
        list.append(0) # set all values as unmarked

    if len(list)>1:
        list[0]=1 # mark 0 to ignore
        list[1]=1 # mark 1 as special

        for k in range(2,int(math.sqrt(n))): # until k exceeds or equals the square root of n
            if list[k] != 1: # proceed if not marked already
                m=k
                j=2
                while m*j <= n-1:
                    list[m*j]=1 # mark as a composite number
                    j+=1

    
        for y in range(2,n):
            if list[y]==0: # proceed if not marked, assume prime
                primes.append(y)
                nprimes+=1

    print "List of primes:"
    print primes
    print "Number of primes:"
    print nprimes

if len(sys.argv) != 1 and sys.argv[1].isdigit(): # check not empty and integer
    n = int(sys.argv[1]) # acquire n from first argument
else:
    n = 100
    print "No integer argument, using 100 default"

sieve(n)
