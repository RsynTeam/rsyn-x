#/bin/bash

# If you make changes to the lex and grammar files, you need to call this script
# in order to regenerate the parser files.

mkdir build
cp SimplifiedVerilog.l build
cp SimplifiedVerilog.yy build
cd build

bison -d -v SimplifiedVerilog.yy
flex --outfile=SimplifiedVerilog.yy.cc SimplifiedVerilog.l

path=../../../src/rsyn/io/parser/verilog/base
rm -rf $path
mkdir -p $path
rm *.output
mv * $path

cd ..
rm -rf build

