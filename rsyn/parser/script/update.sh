#/bin/bash

# If you make changes to the lex and grammar files, you need to call this script
# in order to regenerate the parser files.

mkdir -p build
cp Script.l build
cp Script.yy build
cd build

bison -d -v Script.yy
flex --outfile=Script.yy.cc Script.l

path=../../../src/rsyn/io/parser/script/base
rm -rf $path
mkdir -p $path
rm *.output
mv * $path

cd ..
rm -rf build
