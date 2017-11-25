#/bin/bash

# If you make changes to the lex and grammar files, you need to call this script
# in order to regenerate the parser files.

mkdir -p build
cd build

# Copy scanner and gramar specifications.
cp ../Script.l .
cp ../Script.yy .

# Let's copy this include to improve portability among different operating 
# systems.
cp /usr/include/FlexLexer.h FlexLexerCopy.h 

# Generate parser.
bison -d -v Script.yy

# Generate scanner.
flex --outfile=Script.yy.cc Script.l

# Patch to improve portability.
sed -i -e 's/#include <FlexLexer.h>/#include "FlexLexerCopy.h"/g' Script.yy.cc

# Move files to the target source folder.
path=../../../src/rsyn/io/parser/script/base
rm -rf $path
mkdir $path
rm *.output
mv * $path

# Clean-up
cd ..
rm -rf build
