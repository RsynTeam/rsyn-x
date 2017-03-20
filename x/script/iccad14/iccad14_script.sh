#!/bin/bash

BINDIR="./bin"
EXECMD="$BINDIR/cada110"

path="benchmark/iccad2014"
parm="bin/ICCAD14.parm"
dsg=("b19" "vga_lcd" "leon2" "leon3mp" "netcard")
ut=("0.76" "0.7" "0.7" "0.7" "0.72")
minDisp=(20 10 40 30 50)
maxDisp=(200 200 400 300 400)
file_name="check_legality.log"



div ()  # Arguments: dividend and divisor
{
	if [ $2 -eq 0 ]; then echo division by 0; exit; fi
	p=12                            # precision
	c=${c:-0}                       # precision counter
	d=.                             # decimal separator
	r=$(($1/$2)); echo -n $r        # result of division
	m=$(($r*$2))
	[ $c -eq 0 ] && [ $m -ne $1 ] && echo -n $d
	[ $1 -eq $m ] || [ $c -eq $p ] && exit
	d=$(($1-$m))
	let c=c+1
	div $(($d*10)) $2
}



if [ -d violations ]
then
	rm -r violations
fi

mkdir violations

numDsg=${#dsg[@]}

#for((i=0; i<${numDsg}; i++));
#do	
#	max=${maxDisp[$i]}
#	min=${minDisp[$i]}
#	echo $max
#	echo $min
#	num=5
#	val= expr ${max} - ${min}
#	step= div [ expr ${max} - ${min} ] 5
#	echo ${step}
#	set dis step
#	for((dis=${minDisp[$i]}; dis<=${maxDisp[$i]}; dis=dis+step))
#	do
#		echo "#################################################################################################################"
#		echo "#                                                                                                               #"
#		echo "#                                                                                                               #"
#		echo "                       Running Circuit ${dsg[$i]} for ${dis} um Displacement                                     "
#		echo "#                                                                                                               #"
#		echo "#                                                                                                               #"
#		echo "#################################################################################################################"

#		if [ -f ${file_name} ]
#		then
#			mv ${file_name} violations/${file_name}_${dsg[$i]}_${dis}
#		fi

		#$EXECMD -settings $parm -input $path/${dsg[$i]}/${dsg[$i]}.iccad2014 -ut ${ut[$i]} -max_disp ${dis}
		#./bin/iccad2014_evaluate_solution $parm $path/${dsg[$i]}/${dsg[$i]}.iccad2014 ${ut[$i]} ${dsg[$i]}-cada110.def 

#		if [ -f ${file_name} ]
#		then
#			mv ${file_name} violations/${file_name}_${dsg[$i]}_${dis}
#		fi
#	done
	
#done


for((i=0; i<${numDsg}; i++));
do	
	
	echo "#################################################################################################################"
	echo "#                                                                                                               #"
	echo "#                                                                                                               #"
	echo "                       Running Circuit ${dsg[$i]} for ${minDisp[$i]} um Displacement                                     "
	echo "#                                                                                                               #"
	echo "#                                                                                                               #"
	echo "#################################################################################################################"
	
	if [ -f  ${dsg[$i]}-cada110.def ]
	then
		rm ${dsg[$i]}-cada110.def
	fi
	echo "Time: "
	date
	$EXECMD -settings $parm -input $path/${dsg[$i]}/${dsg[$i]}.iccad2014 -ut ${ut[$i]} -max_disp ${minDisp[$i]}
	echo "Time: "
	date
	echo "************************* Starting evaluate circuit solution ************************"
	./bin/iccad2014_evaluate_solution $parm $path/${dsg[$i]}/${dsg[$i]}.iccad2014 ${ut[$i]} ${dsg[$i]}-cada110.def 

	if [ -f ${file_name} ]
	then
		mv ${file_name} violations/${file_name}_${dsg[$i]}_${minDisp[$i]}
	fi
done
	echo "Time: "
	date
for((i=0; i<${numDsg}; i++));
do
	echo "#################################################################################################################"
	echo "#                                                                                                               #"
	echo "#                                                                                                               #"
	echo "                       Running Circuit ${dsg[$i]} for ${maxDisp[$i]} um Displacement                                     "
	echo "#                                                                                                               #"
	echo "#                                                                                                               #"
	echo "#################################################################################################################"
	
	if [ -f  ${dsg[$i]}-cada110.def ]
	then
		rm ${dsg[$i]}-cada110.def
	fi
	echo "Time: "
	date
	$EXECMD -settings $parm -input $path/${dsg[$i]}/${dsg[$i]}.iccad2014 -ut ${ut[$i]} -max_disp ${maxDisp[$i]}
	echo "Time: "
	date
	echo "************************* Starting evaluate circuit solution ************************"
	./bin/iccad2014_evaluate_solution $parm $path/${dsg[$i]}/${dsg[$i]}.iccad2014 ${ut[$i]} ${dsg[$i]}-cada110.def 

	if [ -f ${file_name} ]
	then
		mv ${file_name} violations/${file_name}_${dsg[$i]}_${maxDisp[$i]}
	fi
done
	echo "Time: "
	date