#!/bin/sh

#Path to directory of Rsyn scripts
RSYN_SCRIPT_DIR=$1
EVAL_DIR="../misc/iccad2015_evaluate_solution_0831/"
BIN_DIR="../../bin"

echo "Directory of Rsyn Scripts: ${RSYN_SCRIPT_DIR}"

echo "###################################################################"
echo "# SUPERBLUE1 SHORT DISPLACEMENT                                   #"
echo "###################################################################"
make rscript script=${RSYN_SCRIPT_DIR}/superblue1-short.rsyn
cd $EVAL_DIR
make riccad15 dsg=superblue1 ut=0.80
cd $BIN_DIR

echo "###################################################################"
echo "# SUPERBLUE3 SHORT DISPLACEMENT                                   #"
echo "###################################################################"
make rscript script=${RSYN_SCRIPT_DIR}/superblue3-short.rsyn
cd $EVAL_DIR
make riccad15 dsg=superblue3 ut=0.87
cd $BIN_DIR

echo "###################################################################"
echo "# SUPERBLUE4 SHORT DISPLACEMENT                                   #"
echo "###################################################################"
make rscript script=${RSYN_SCRIPT_DIR}/superblue4-short.rsyn
cd $EVAL_DIR
make riccad15 dsg=superblue4 ut=0.90
cd $BIN_DIR

echo "###################################################################"
echo "# SUPERBLUE5 SHORT DISPLACEMENT                                   #"
echo "###################################################################"
make rscript script=${RSYN_SCRIPT_DIR}/superblue5-short.rsyn
cd $EVAL_DIR
make riccad15 dsg=superblue5 ut=0.85
cd $BIN_DIR

echo "###################################################################"
echo "# SUPERBLUE7 SHORT DISPLACEMENT                                   #"
echo "###################################################################"
make rscript script=${RSYN_SCRIPT_DIR}/superblue7-short.rsyn
cd $EVAL_DIR
make riccad15 dsg=superblue7 ut=0.90
cd $BIN_DIR

echo "###################################################################"
echo "# SUPERBLUE10 SHORT DISPLACEMENT                                   #"
echo "###################################################################"
make rscript script=${RSYN_SCRIPT_DIR}/superblue10-short.rsyn
cd $EVAL_DIR
make riccad15 dsg=superblue10 ut=0.87
cd $BIN_DIR

echo "###################################################################"
echo "# SUPERBLUE16 SHORT DISPLACEMENT                                  #"
echo "###################################################################"
make rscript script=${RSYN_SCRIPT_DIR}/superblue16-short.rsyn
cd $EVAL_DIR
make riccad15 dsg=superblue16 ut=0.85
cd $BIN_DIR

echo "###################################################################"
echo "# SUPERBLUE18 SHORT DISPLACEMENT                                  #"
echo "###################################################################"
make rscript script=${RSYN_SCRIPT_DIR}/superblue18-short.rsyn
cd $EVAL_DIR
make riccad15 dsg=superblue18 ut=0.85
cd $BIN_DIR

