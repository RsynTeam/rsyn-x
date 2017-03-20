#!bin/bash

export GUROBI_HOME="/opt/gurobi702/linux64" 

if [! -d "$GUROBI_HOME" ]; then
  echo "[ERROR] Gurobi not installed!"
  exit 1
fi

export PATH="${PATH}:${GUROBI_HOME}/bin" 
export LD_LIBRARY_PATH="${GUROBI_HOME}/lib"

gurobi_cl Threads=$1 ResultFile=$2 $3