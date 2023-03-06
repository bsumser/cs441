#!/usr/bin/env python3
import sys
import time
sys.setrecursionlimit(3000)


def word_find(inputString):
    if inputString == "":
        return True
    inputStringLength = len(inputString)
    for i in range(1, inputStringLength + 1):
        if dictList.count(inputString[0:i]) == 1 and word_find(inputString[i:inputStringLength]):
            return True
    return False

def main():

    # open dictionary file
    f = open("diction10k.txt", "r")

    # read file contents
    data = f.read()

    # split file contents into list on new line
    global dictList
    dictList = data.split("\n")

    # print check for contents
    # print(dictList)

    inputList = []
    for line in sys.stdin:
        inputList.append(line.strip("\n"))
    print(inputList)

    num_split = int(inputList[0])

    print(num_split)

    # init the dp table
    dp = [[0] * num_split for i in range(num_split)]

    print(dictList.count("snickle"))

    start_time = time.time()
    print(word_find(inputList[1]))
    print("--- %s seconds ---" % (time.time() - start_time))


main()
