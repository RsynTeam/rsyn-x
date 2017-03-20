#!/bin/sh

flow="default"
if [ ! -z "$1" ]; then
  flow=$1
fi

echo "Selected flow: $flow"


echo "###################################################################"
echo "#		SUPERBLUE1 LONG DISPLACEMENT				#"
echo "###################################################################"
make rcalcmd dsg=superblue1  dis=400 ut=0.80 flow=$flow

echo "###################################################################"
echo "#		SUPERBLUE3 LONG DISPLACEMENT				#"
echo "###################################################################"
make rcalcmd dsg=superblue3  dis=400 ut=0.87 flow=$flow

echo "###################################################################"
echo "#		SUPERBLUE4 LONG DISPLACEMENT				#"
echo "###################################################################"
make rcalcmd dsg=superblue4  dis=400 ut=0.90 flow=$flow

echo "###################################################################"
echo "#		SUPERBLUE5 LONG DISPLACEMENT				#"
echo "###################################################################"
make rcalcmd dsg=superblue5  dis=400 ut=0.85 flow=$flow

echo "###################################################################"
echo "#		SUPERBLUE7 LONG DISPLACEMENT				#"
echo "###################################################################"
make rcalcmd dsg=superblue7  dis=500 ut=0.90 flow=$flow

echo "###################################################################"
echo "#		SUPERBLUE10 LONG DISPLACEMENT				#"
echo "###################################################################"
make rcalcmd dsg=superblue10 dis=500 ut=0.87 flow=$flow

echo "###################################################################"
echo "#		SUPERBLUE16 LONG DISPLACEMENT				#"
echo "###################################################################"
make rcalcmd dsg=superblue16 dis=400 ut=0.85 flow=$flow

echo "###################################################################"
echo "#		SUPERBLUE18 LONG DISPLACEMENT				#"
echo "###################################################################"
make rcalcmd dsg=superblue18 dis=400 ut=0.85 flow=$flow


echo "###################################################################"
echo "#		SUPERBLUE1 SHORT DISPLACEMENT				#"
echo "###################################################################"
make rcalcmd dsg=superblue1  dis=40 ut=0.80 flow=$flow

echo "###################################################################"
echo "#		SUPERBLUE3 SHORT DISPLACEMENT				#"
echo "###################################################################"
make rcalcmd dsg=superblue3  dis=40 ut=0.87 flow=$flow

echo "###################################################################"
echo "#		SUPERBLUE4 SHORT DISPLACEMENT				#"
echo "###################################################################"
make rcalcmd dsg=superblue4  dis=20 ut=0.90 flow=$flow

echo "###################################################################"
echo "#		SUPERBLUE5 SHORT DISPLACEMENT				#"
echo "###################################################################"
make rcalcmd dsg=superblue5  dis=30 ut=0.85 flow=$flow

echo "###################################################################"
echo "#		SUPERBLUE7 SHORT DISPLACEMENT				#"
echo "###################################################################"
make rcalcmd dsg=superblue7  dis=50 ut=0.90 flow=$flow

echo "###################################################################"
echo "#		SUPERBLUE10 SHORT DISPLACEMENT				#"
echo "###################################################################"
make rcalcmd dsg=superblue10 dis=20 ut=0.87 flow=$flow

echo "###################################################################"
echo "#		SUPERBLUE16 SHORT DISPLACEMENT				#"
echo "###################################################################"
make rcalcmd dsg=superblue16 dis=30 ut=0.85 flow=$flow

echo "###################################################################"
echo "#		SUPERBLUE18 SHORT DISPLACEMENT				#"
echo "###################################################################"
make rcalcmd dsg=superblue18 dis=20 ut=0.85 flow=$flow
