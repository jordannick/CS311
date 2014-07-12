# Nicholas Jordan
# CS 311
# Assignment 2 Part 4

import math
import sys

#note: only works up for finding prime numbers below 20,000
def sieve(n, userprime, nprimes):
    
    list = [] 
    primes = [] # list for primes
    found = False
    nmax = 20000 #arbitrary value

    for x in range(n,nmax+1):
        list.append(0) # set all values as unmarked

    if len(list)>1 and (nmax)<20001:#arbitrary
        list[0]=1 # mark 0 to ignore
        list[1]=1 # mark 1 as special
        n = 2

    for k in range(n,int(math.sqrt(nmax))): # until k exceeds or equals the square root of n
        if list[k] != 1: # proceed if not marked already
            m=k
            j=2
            while m*j < (nmax):
                list[m*j]=1 # mark as a composite number
                j+=1

    
    for y in range(n,nmax):
        if list[y]==0: # proceed if not marked, assume prime
            primes.append(y)
            nprimes+=1
            if nprimes == userprime:
                print "Prime number "+str(userprime)+" is: "+str(y)
                found = True
                break 


    return (found, nprimes)

def main():
    if len(sys.argv)>1:
        userprime = int(sys.argv[1]) # acquire from first argument
    else:
        print "Requires #  argument"
        sys.exit(2)
    n = 0
    found = False 
    nprimes = 0
    #while found == False: #loop nonfunctional at the moment
    found, nprimes = sieve(n, userprime, nprimes)
      #  n+=20000


if __name__ == "__main__":
    main()
