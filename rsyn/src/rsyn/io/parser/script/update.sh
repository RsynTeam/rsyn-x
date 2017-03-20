#/bin/bash

# If you make changes to the lex and grammar files, you need to call this script
# in order to regenerate the parser files.

gendir="base"

mkdir -p $gendir
cd $gendir
bison -d -v ../Script.yy
flex --outfile=Script.yy.cc ../Script.l
rm *.output
cd ..
