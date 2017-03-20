#/bin/bash
bison -d -v SimplifiedVerilog.yy
flex --outfile=SimplifiedVerilog.yy.cc SimplifiedVerilog.l

path=../../src/parser/verilog/generated
mv *.cc $path
mv *.hh $path

rm *.output
