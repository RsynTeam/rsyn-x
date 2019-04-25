#!/bin/bash

####################################################
#		Defining Variables	           #
####################################################

TEST_LIST=(regressionsInstances regressionsPhysicalInfo regressionsRouting regressionsPG regressionsCoordinates)

PREV_DIR=$(pwd)
SCRIPTS_FOLDER="$( cd "$(dirname "$0")" ; pwd -P )"
SUPPORT_FOLDER=$(dirname $SCRIPTS_FOLDER)
RSYN_FOLDER=$(dirname $SUPPORT_FOLDER)
BIN_FOLDER="$RSYN_FOLDER/build/bin"
RSYN_SCRIPTS_FOLDER="support/test/rsyn-scripts"
RESULTS_FOLDER="$RSYN_FOLDER/build/regressions"
TEST_SCRIPTS_FOLDER="$RESULTS_FOLDER/scripts"
RSYN="rsyn"
BENCHMARKS_FOLDER="$SUPPORT_FOLDER/benchmarks"
RSYN_RUN_OPTIONS="--no-gui --script"
DOCKER_BENCHMARKS_FOLDER="/benchmarks"
FROM_SCRIPTS_TO_BENCHMARKS="../../../support/benchmarks"
GOLDEN_FOLDER="$BENCHMARKS_FOLDER/golden-files"
PYTHON_V=python3

GENERATE_GOLD=0
RSYN_SCRIPTS=()
SCRIPTS_PATH=()
TESTS_CMD=("")
##Changing to Rsyn folder
cd $RSYN_FOLDER
####################################################
#		Checking Compilation	           #
####################################################
if [[ ! -f "$BIN_FOLDER/$RSYN" ]]; then
	echo "Compile Rsyn first, you can run the compile-rsyn script"
	cd $PREV_DIR
	exit 0
fi

####################################################
#		    Help Message	           #
####################################################
function usage {
	cat <<HEREDOC
Usage: $(basename "$0") [options] [--]

Options:
	-h | --help
		Display this message

	--benchmarks=
		Execute test cases for selected benchmarks (Example: --benchmark=iccad15,ispd19)
	
	--tests=
		Execute selected test cases (Example: --tests=regressionsInstances,regressionsPG)
	--gold
		Generate golden file for selected benchmarks and tests
HEREDOC

}


####################################################
#			Tests	                   #
####################################################

##Parsing arguments
while [ "$#" -gt 0 ]; do
	case "${1}" in
		--h|--help)
			usage;
			exit 0
			;;
		--benchmarks=*)
			oldifs=$IFS
			IFS=,
			read -r -a BENCHMARK_LIST <<< "${1#*=}"
			IFS=$oldifs
			;;
		--tests=*)
			oldifs=$IFS
			IFS=,
			read -r -a TEST_LIST <<< "${1#*=}"
			IFS=$oldifs
			;;
		--gold)
			GENERATE_GOLD=1
			;;
		*) echo "unknown option: ${1}" >&2; usage; exit 1 ;;
	esac
	shift 1
done


##Creating results folder
if [ ! -d $RESULTS_FOLDER ]; then
	mkdir $RESULTS_FOLDER
	mkdir $TEST_SCRIPTS_FOLDER
else
	rm -rf $RESULTS_FOLDER
	mkdir $RESULTS_FOLDER
	mkdir $TEST_SCRIPTS_FOLDER
fi

if [ ! -d $GOLDEN_FOLDER ]; then
	mkdir $GOLDEN_FOLDER
fi

##Copying benchmarks to inside rsyn folder
if [ -d $DOCKER_BENCHMARKS_FOLDER ]; then
	if [ ! -d $BENCHMARKS_FOLDER ]; then
		cp -r "$DOCKER_BENCHMARKS_FOLDER/." $BENCHMARKS_FOLDER
	fi
fi


##Getting .rsyn files to run tests
if [ -z $BENCHMARK_LIST ]; then  ##all benchmarks
	
	##Creating rsyn scripts
	find $RSYN_SCRIPTS_FOLDER -name "*.sh" -exec bash {} "$FROM_SCRIPTS_TO_BENCHMARKS" ';'

	while IFS= read -r -d $'\0'; do
		RSYN_SCRIPTS+=("$REPLY")
	done < <(find $RSYN_SCRIPTS_FOLDER -iname "*.rsyn" -type f -print0)
else
	##find specific benchmarks folders and create results folder
	for benchmark in ${BENCHMARK_LIST[@]}; do
		./"$RSYN_SCRIPTS_FOLDER/${benchmark}.sh" $FROM_SCRIPTS_TO_BENCHMARKS
		SCRIPTS_PATH+="$RSYN_SCRIPTS_FOLDER/$benchmark"
	done

	##find .rsyn files
	for path in ${SCRIPTS_PATH[@]}; do
		while IFS= read -r -d $'\0'; do
			RSYN_SCRIPTS+=("$REPLY")
		done < <(find $path -iname "*.rsyn" -type f -print0)
	done
fi

countError=0
for script in ${RSYN_SCRIPTS[@]}; do
	BENCHMARK_FOLDER=$(dirname $script)
	BENCHMARK=$(basename $BENCHMARK_FOLDER)
	SCRIPT_NAME=$(basename $script)

	if [ $GENERATE_GOLD -eq 1 ]; then
		echo "Generated Gold - Benchmark: ${SCRIPT_NAME/.rsyn/}"
		TEST_RESULTS_PATH=$GOLDEN_FOLDER
	else
		echo "TESTING BENCHMARK: ${SCRIPT_NAME/.rsyn/}"
		TEST_RESULTS_PATH=$RESULTS_FOLDER
	fi

	for testCommand in ${TEST_LIST[@]}; do
		echo "COMMAND: $testCommand"
		##create folders
		TEST_RESULTS_FOLDER="$TEST_RESULTS_PATH/$testCommand"
		if [ ! -d "$TEST_RESULTS_FOLDER" ]; then
			mkdir "$TEST_RESULTS_FOLDER"
			mkdir "$TEST_RESULTS_FOLDER/$BENCHMARK"
		elif [ ! -d "$TEST_RESULTS_FOLDER/$BENCHMARK" ]; then 
			mkdir "$TEST_RESULTS_FOLDER/$BENCHMARK"
		fi

		##create new script with test command
		NEW_SCRIPT="$TEST_SCRIPTS_FOLDER/${testCommand}_${SCRIPT_NAME}"
		cp $script $NEW_SCRIPT 
		cat >> "$NEW_SCRIPT" << HEREDOC

start "openroad.regressions"{};
$testCommand;
HEREDOC
	
	NEW_SCRIPT_NAME=$(basename $NEW_SCRIPT)
	#TODO: Fix the need of PORT9 on bin folder
	if [ $BENCHMARK == "iccad15" ]; then
	       	echo "Copying PORT9"
		cp -f "$BENCHMARKS_FOLDER/iccad15/evaluate_solution_0831/PORT9.dat" $BIN_FOLDER
		cp -f "$BENCHMARKS_FOLDER/iccad15/evaluate_solution_0831/POWV9.dat" $BIN_FOLDER
	fi

	cd $BIN_FOLDER
	OUTPUT_SCRIPT="$TEST_RESULTS_FOLDER/$BENCHMARK/$NEW_SCRIPT_NAME.out"
	./$RSYN $RSYN_RUN_OPTIONS "$NEW_SCRIPT" 2>&1 >  $OUTPUT_SCRIPT 
	if [ $GENERATE_GOLD -eq 1 ]; then
		sed -i '/\(#.*\)/!d' $OUTPUT_SCRIPT
	else
		NEW_OUTPUT_SCRIPT="$TEST_RESULTS_FOLDER/$BENCHMARK/$NEW_SCRIPT_NAME-test.out" 
		GOLDEN_SCRIPT="$GOLDEN_FOLDER/$testCommand/$BENCHMARK/$NEW_SCRIPT_NAME.out"
		cp $OUTPUT_SCRIPT $NEW_OUTPUT_SCRIPT
		
		sed -i '/\(#.*\)/!d' $NEW_OUTPUT_SCRIPT

		if cmp -s $GOLDEN_SCRIPT $NEW_OUTPUT_SCRIPT; then
			echo "100% match"
		else
			countError=$(($countError + 1))
			$PYTHON_V "$SCRIPTS_FOLDER/compare-files.py" -g $GOLDEN_SCRIPT -t $NEW_OUTPUT_SCRIPT
		fi
		rm $NEW_OUTPUT_SCRIPT
				
	fi
 
	cd $RSYN_FOLDER

	if [ $BENCHMARK == "iccad15" ]; then 
		rm -f "$BIN_FOLDER/PORT9.dat"
		rm -f "$BIN_FOLDER/POWV9.dat"
	fi
	echo -e "\n\n\n"
	done
done

##Changing back to original folder
cd $PREV_DIR

if [ $countError -ne 0 ]; then
	exit 1
fi
