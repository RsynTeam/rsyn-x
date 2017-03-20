#!/bin/sh

flow="default"
if [ ! -z "$1" ]; then
  flow=$1
fi

echo "Selected flow: $flow"
EVAL_DIR="../eval_solution/"
BIN_DIR="../bin"

echo "###################################################################"
echo "# SUPERBLUE1 SHORT DISPLACEMENT                                   #"
echo "###################################################################"
make riccad15 dsg=superblue1  dis=40 ut=0.80 flow=$flow
cd $EVAL_DIR
make riccad15 dsg=superblue1 ut=0.80
cd $BIN_DIR

echo "###################################################################"
echo "# SUPERBLUE3 SHORT DISPLACEMENT                                   #"
echo "###################################################################"
make riccad15 dsg=superblue3  dis=40 ut=0.87 flow=$flow
cd $EVAL_DIR
make riccad15 dsg=superblue3 ut=0.87
cd $BIN_DIR

echo "###################################################################"
echo "# SUPERBLUE4 SHORT DISPLACEMENT                                   #"
echo "###################################################################"
make riccad15 dsg=superblue4  dis=20 ut=0.90 flow=$flow
cd $EVAL_DIR
make riccad15 dsg=superblue4 ut=0.90
cd $BIN_DIR

echo "###################################################################"
echo "# SUPERBLUE5 SHORT DISPLACEMENT                                   #"
echo "###################################################################"
make riccad15 dsg=superblue5  dis=30 ut=0.85 flow=$flow
cd $EVAL_DIR
make riccad15 dsg=superblue5 ut=0.85
cd $BIN_DIR

echo "###################################################################"
echo "# SUPERBLUE7 SHORT DISPLACEMENT                                   #"
echo "###################################################################"
make riccad15 dsg=superblue7  dis=50 ut=0.90 flow=$flow
cd $EVAL_DIR
make riccad15 dsg=superblue7 ut=0.90
cd $BIN_DIR

echo "###################################################################"
echo "# SUPERBLUE10 SHORT DISPLACEMENT                                   #"
echo "###################################################################"
make riccad15 dsg=superblue10 dis=20 ut=0.87 flow=$flow
cd $EVAL_DIR
make riccad15 dsg=superblue10 ut=0.87
cd $BIN_DIR

echo "###################################################################"
echo "# SUPERBLUE16 SHORT DISPLACEMENT                                  #"
echo "###################################################################"
make riccad15 dsg=superblue16 dis=30 ut=0.85 flow=$flow
cd $EVAL_DIR
make riccad15 dsg=superblue16 ut=0.85
cd $BIN_DIR

echo "###################################################################"
echo "# SUPERBLUE18 SHORT DISPLACEMENT                                  #"
echo "###################################################################"
make riccad15 dsg=superblue18 dis=20 ut=0.85 flow=$flow
cd $EVAL_DIR
make riccad15 dsg=superblue18 ut=0.85
cd $BIN_DIR

