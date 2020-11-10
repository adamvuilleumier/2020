#!/usr/bin/python

import random, sys, argparse

class shuf:
    def __init__(self, filename=None, lo=None, hi=None, filePresent=False):

        self.lines = [ ] #initialize self.lines as an empty array

        #case of reading from stdin
        if (not filePresent) and lo == None:
            for line in sys.stdin:
                self.lines.append(line)

        #case of reading from a file
        elif filename != None:
            f = open(filename)
            self.lines = f.readlines()
            f.close()

        #case of input range
        else:
            f = range(lo,hi+1)
            for number in f:
                self.lines.append(str(number)+"\n")

    #this function is a helper for createPermWithReplacement()
    def pickRandomLine(self):
        return random.choice(self.lines)

    #for non-repeat case
    def createPermutation(self, ncp=None):
        if ncp == None or ncp > len(self.lines): #if n is invalid, handle like normal
            numLines = len(self.lines)
        else:
            numLines = ncp
            
        randomLines = random.sample(self.lines, numLines)
        for line in randomLines:
            sys.stdout.write(line)

    #for repeat case
    def createPermWithReplacement(self, n=None):
        if n == None or n > len(self.lines): #if n is not specified
            while True: #run infinitely with replacement
                sys.stdout.write(self.pickRandomLine())
        else: #if n is specified
            for rep in range(0,n): #run n times with replacement
                sys.stdout.write(self.pickRandomLine())
    
if __name__ == '__main__':
    
    version_msg = "%prog 2.0"
    usage_msg = """%prog [OPTION]... FILE
    or: shuf -e [OPTION]... [ARG]...
    or: shuf -i LO-HI [OPTION]...

Output randomly selected lines from FILE. When given - or no args, read from stdin"""

    #initializes variables in default states
    filePresent = True
    hi = None
    lo = None
    hc = None
    
    #parser is initialized
    parser = argparse.ArgumentParser()

    #parser recognizes optional arguments
    parser.add_argument("-n", "--head-count",
                      action="store", dest="n", default=sys.maxsize,
                      help="output specified number of lines, default all")
    parser.add_argument("-i", "--input-range",
                        action="store", dest="i", metavar="lo-hi",
                        help="input is the set of all numbers lo-hi inclusive")
    parser.add_argument("-r", "--repeat",
                        action="store_true", dest="r",
                        help="include replacement, when used without n will run forever")
    parser.add_argument("name", type=str, nargs='?', help="the input file name")
    
    args = parser.parse_args()

    #determines if an input file has been given or not                    
    if args.name == None or args.name == '-':
        filePresent = False
                        
    #handles case of head count
    try:
        if args.n != None:
            hc = int(args.n)
            if hc < 0:
                parser.error("negative headcount: {0}".
                             format(args.n))
    except:
        parser.error("invalid headcount: {0}".
                     format(args.n))

    #handles case of input range
    try:
        if args.i != None:
            range_string = str(args.i)
            hilo = range_string.split("-")
            lo = int(hilo[0])
            hi = int(hilo[1])
    except:
        parser.error("Invalid input range given: {}".format(args.i))

    if args.i != None:
        if hi < lo or lo < 0:
            parser.error("Invalid range, LO must be less than HI and greater than 0")
        if filePresent:
            parser.error("Error: Cannot use -i with file present")

    #handles case of repeat
    isRepeat = bool(args.r)

    #variables hc, lo, hi, isRepeat, filePresent have been set

    #initialize shuf for each case
    try:
        if filePresent and args.i == None:
            shuffle = shuf(filename=args.name, lo=lo, hi=hi, filePresent=True)
        else:
            shuffle = shuf(lo=lo, hi=hi) #handles cases of no input file or -i

        if isRepeat:
            shuffle.createPermWithReplacement(hc)
        else:
            shuffle.createPermutation(hc)
        
    except IOError as error:
        errno, strerror = error.args
        parser.error("I/O error({0}): {1}".
                     format(errno, strerror))

