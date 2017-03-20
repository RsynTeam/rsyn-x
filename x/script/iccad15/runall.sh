#!/bin/sh

flow="default"
if [ ! -z "$1" ]; then
  flow=$1
fi

sh runall-long.sh $1
sh runall-short.sh $1
