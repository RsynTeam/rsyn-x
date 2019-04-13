#! /bin/bash
#http://unix.stackexchange.com/questions/105468/split-the-file-and-put-it-in-corresponding-data-structure
flow="default"
if [ ! -z "$1" ]; then
  flow=$1
fi

declare -a teams=("cada014" "cada070" "cada085")
declare -a circuits='{superblue16=[30, 400], superblue18=[20, 400], superblue4=[20, 400], superblue10=[20, 500], superblue7=[50, 500], superblue1=[40, 400], superblue3=[40, 400], superblue5=[30, 400]}'
declare -a dirPath="path"


declare -A "$(
    echo "x=("
    grep -oP '(\w+)(?==)|(?<==\[).*?(?=\])' <<< "$circuits" |
      tr -d , |
      paste - - |
      sed 's/^/[/; s/\t/]="/; s/$/"/'
    echo ")"
)"

for team in "${teams[@]}";
do
	for cir in "${!x[@]}"; do           # need quotes here
		for dis in ${x[$cir]}; do   # no quotes here
			declare -a var="short"
			if [ $dis -gt 80 ]
			then
				var="long"
			fi
			echo "###################################################################"
			echo "# "${cir}" from team "${team}" displacement: " ${var}"              #"
			echo "###################################################################"

			make rcalcmd dsg=${cir} dis=${dis} ut=0.85 flow=${flow} placedCir=benchmark/iccad2015/solutions/${team}/${cir}.${var}.def
		done
	done
	echo "------------------------------------"
done


#./cada085 -trdp -settings ICCAD15.parm -input benchmark/iccad2015/superblue18/superblue18.iccad2015 -ut 0.85 -max_disp 400 -flow default -placed ../../downloads/cada014/superblue18.long.def
#make rcalcmd dsg=superblue16 dis=30 ut=0.85 flow=$flow

