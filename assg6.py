#!/usr/bin/env python3
import sys
import time


def word_find(inputString, dp, orig_length):
    if inputString == "":
        return True
    inputStringLength = len(inputString)
    for i in range(1, inputStringLength + 1):
        if dp[orig_length - inputStringLength][i] == "FALSE":
            continue
        if dictList.count(inputString[0:i]) == 1 and word_find(inputString[i:inputStringLength], dp, orig_length):
            dp[orig_length - inputStringLength][i] = inputString[0:i]
            return True
    dp[orig_length - inputStringLength][i] = "FALSE"
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


    for i in range(1,4):
        inputLength = len(inputList[i]) + 1

        # init the dp table
        dp = [[None] * inputLength for i in range(inputLength)]

        start_time = time.time()
        print(word_find(inputList[i], dp, inputLength - 1))
        print("--- %s seconds ---" % (time.time() - start_time))


main()
