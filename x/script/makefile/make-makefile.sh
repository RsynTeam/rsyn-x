#!bin/bash

tree=$(find ./src -type d -exec echo "{}" \;)

for directory in $tree; do 
	echo $directory; 

	if [ -f "$directory/Makefile" ]
	then
		echo "\t Note: directory has inner makefile."
	fi

	files=$(find $directory -name "*.cpp" -exec echo "\t{}\n" \;)
	echo $files;

done
