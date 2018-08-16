#!/bin/bash

../../rsyn/script/install-dependencies.sh

echo "##############################################"
echo "#                                            #"
echo "#   Installing Rsyn-X Library Dependencies   #"
echo "#                                            #"
echo "##############################################"

declare -a libs=(
)

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

