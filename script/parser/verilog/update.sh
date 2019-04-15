#/bin/bash

# If you make changes to the lex and grammar files, you need to call this script
# in order to regenerate the parser files.

mkdir -p build
cd build

# Copy scanner and gramar specifications.
cp ../SimplifiedVerilog.l .
cp ../SimplifiedVerilog.yy .

# Let's copy this include to improve portability among different operating 
# systems.
cp /usr/include/FlexLexer.h FlexLexerCopy.h 

# Generate parser.
bison -d -v SimplifiedVerilog.yy

# Generate scanner.
flex --outfile=SimplifiedVerilog.yy.cc SimplifiedVerilog.l

# Patch to improve portability.
sed -i -e 's/#include <FlexLexer.h>/#include "FlexLexerCopy.h"/g' SimplifiedVerilog.yy.cc

# Move files to the target source folder.
path=../../../src/rsyn/io/parser/verilog/base
rm -rf $path
mkdir -p $path
rm *.output
mv * $path

# Clean-up
cd ..
rm -rf build

