# Nicholas Jordan
# CS 311
# Assignment 2 Part 3

import urllib2
import sys
import re


def main():

    if len(sys.argv) > 2:
        url = sys.argv[1]
        filename = sys.argv[2]
        p = re.compile('http://|https://', re.IGNORECASE)
        m = p.match(url)

        if m:
            read_url(url, filename)
        else:
            url = "http://"+url
            read_url(url, filename)
    else:
        print "Two arguments required in form: [web address] [file to save]"

def read_url(url,filename):

    try:
        response = urllib2.urlopen(url)
    except urllib2.URLError as e:
        print e.reason
        sys.exit(2)

    html = response.read()

    f = open(filename, "w")
    f.write(html)
    f.close()

    print url
    print "Saved to "+filename+"\n"


if __name__ == "__main__":
    main()
