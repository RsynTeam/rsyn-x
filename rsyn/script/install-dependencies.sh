#!/bin/bash

echo "############################################"
echo "#                                          #"
echo "#   Installing Rsyn Library Dependencies   #"
echo "#                                          #"
echo "############################################"

declare -a libs=(
"libboost-all-dev"
"flex"
"bison"
"qtbase5-dev"
"libgl1-mesa-dev"
"libglu1-mesa-dev"
"freeglut3-dev"
"libglew-dev"
"libpng16-dev"
)

if [[ `lsb_release -rs` == "18.04" ]] # replace 8.04 by the number of release you want
then
	echo "Updated libpng16-dev to libpng-dev in Ubuntu 18.04"
	libs[-1]="libpng-dev"
fi

echo "You are required to have an administrative account to install UPlace library dependencies!"
for i in "${libs[@]}"
do
        sudo apt install -y $i
        
        # Install only dependencies that were not yet installed 
        # error while searching if package coinor-* is installed
        #status=$(dpkg -s $i | grep Status)
        #if [ "$status" != "Status: install ok installed" ]; then
        #    instMsg="******  Installing "
        #    instMsg+=$i
        #    instMsg+=" ******"
        #    echo $instMsg
        #    sudo apt install -y $i
        #fi;
done

