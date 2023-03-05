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

    inputList = []
    for line in sys.stdin:
        inputList.append(line.strip("\n"))
    print(inputList)

    num_split = int(inputList[0])

    print(num_split)

    # init the dp table
    dp = [ [0] * num_split]

    print(dictList.count("snickle"))


main()
