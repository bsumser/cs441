#!/usr/bin/env python3
import sys

def main():

    # open dictionary file
    f = open("diction10k.txt", "r")

    # read file contents
    data = f.read()

    # split file contents into list on new line
    dictList = data.split("\n")

    # print check for contents
    print(dictList)

    for line in sys.stdin:
        print(line)

    # init the dp table
    t = [ [0]*3 for i in range(3)]

    print(dictList.count("snickle"))


main()
