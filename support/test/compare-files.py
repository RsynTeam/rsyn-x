from argparse import ArgumentParser
from copy import copy,deepcopy
import itertools
from collections import deque


def compare(goldFile, testFile):
    diffGold = []
    diffTest = []
    with open(goldFile, 'r') as gold, open(testFile, 'r') as test:
        for lineGold, lineTest in itertools.zip_longest(gold, test):
            if lineGold != None:
                lineGold = lineGold.strip()
            if lineTest != None:
                lineTest = lineTest.strip()
            
            if lineGold != lineTest:
                if lineGold != None:
                    diffGold.append(lineGold)
                if lineTest != None:
                    diffTest.append(lineTest)

    diffGold = sorted(diffGold)
    diffTest = sorted(diffTest)
    if diffGold == diffTest:
        print("###############################################################")
        print("##         The following lines are out of order:             ##")
        print("###############################################################")
        for line in diffTest:
            print(line)
    else:
        diff = set(diffTest) - set(diffGold)
        common = set(diffTest) - diff
        if len(common) != 0:
            print("###############################################################")
            print("##         The following lines are out of order:             ##")
            print("###############################################################")
            for line in list(common):
                print(line)
        if len(diff) != 0:
            print("###############################################################")
            print("##   The following lines don`t match the golden file:        ##")
            print("###############################################################")
            for line in list(diff):
                print(line)

def main():

    parser = ArgumentParser()

    parser.add_argument(
            '-g',
            '--gold',
            action='store',
            dest='goldFile',
            required=True,
            help="gold file")

    parser.add_argument(
            '-t',
            '--test',
            action='store',
            dest='testFile',
            required=True,
            help="test file")

    args = parser.parse_args()
    compare(args.goldFile, args.testFile)

if __name__ == "__main__":
        main()
