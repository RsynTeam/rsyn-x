BENCHMARK_FOLDER=$1

CUR_DIR="$( cd "$(dirname "$0")" ; pwd -P )"

if [ ! -d "$CUR_DIR/iccad15" ]; then
	mkdir "$CUR_DIR/iccad15"
fi

cat > "$CUR_DIR/iccad15/iccad15_superblue7_generic.rsyn" << HEREDOC
open "generic" {
	"lefFiles" : "$BENCHMARK_FOLDER/iccad15/superblue7/superblue7.lef",
	"defFiles" : "$BENCHMARK_FOLDER/iccad15/superblue7/superblue7.def",
	"config" : "$BENCHMARK_FOLDER/iccad15/superblue7/superblue7.iccad2015",
	"maxDisplacement" : 400,
	"targetUtilization" : 0.8,
	"parms" : "$BENCHMARK_FOLDER/iccad15/evaluate_solution_0831/ICCAD15.parm"
};
HEREDOC

cat > "$CUR_DIR/iccad15/iccad15_superblue5_generic.rsyn" << HEREDOC
open "generic" {
	"lefFiles" : "$BENCHMARK_FOLDER/iccad15/superblue5/superblue5.lef",
	"defFiles" : "$BENCHMARK_FOLDER/iccad15/superblue5/superblue5.def",
	"config" : "$BENCHMARK_FOLDER/iccad15/superblue5/superblue5.iccad2015",
	"maxDisplacement" : 400,
	"targetUtilization" : 0.8,
	"parms" : "$BENCHMARK_FOLDER/iccad15/evaluate_solution_0831/ICCAD15.parm"
};
HEREDOC

cat > "$CUR_DIR/iccad15/iccad15_superblue4_generic.rsyn" << HEREDOC
open "generic" {
	"lefFiles" : "$BENCHMARK_FOLDER/iccad15/superblue4/superblue4.lef",
	"defFiles" : "$BENCHMARK_FOLDER/iccad15/superblue4/superblue4.def",
	"config" : "$BENCHMARK_FOLDER/iccad15/superblue4/superblue4.iccad2015",
	"maxDisplacement" : 400,
	"targetUtilization" : 0.8,
	"parms" : "$BENCHMARK_FOLDER/iccad15/evaluate_solution_0831/ICCAD15.parm"
};
HEREDOC

cat > "$CUR_DIR/iccad15/iccad15_superblue3_generic.rsyn" << HEREDOC
open "generic" {
	"lefFiles" : "$BENCHMARK_FOLDER/iccad15/superblue3/superblue3.lef",
	"defFiles" : "$BENCHMARK_FOLDER/iccad15/superblue3/superblue3.def",
	"config" : "$BENCHMARK_FOLDER/iccad15/superblue3/superblue3.iccad2015",
	"maxDisplacement" : 400,
	"targetUtilization" : 0.8,
	"parms" : "$BENCHMARK_FOLDER/iccad15/evaluate_solution_0831/ICCAD15.parm"
};
HEREDOC

cat > "$CUR_DIR/iccad15/iccad15_superblue1_generic.rsyn" << HEREDOC
open "generic" {
	"lefFiles" : "$BENCHMARK_FOLDER/iccad15/superblue1/superblue1.lef",
	"defFiles" : "$BENCHMARK_FOLDER/iccad15/superblue1/superblue1.def",
	"config" : "$BENCHMARK_FOLDER/iccad15/superblue1/superblue1.iccad2015",
	"maxDisplacement" : 400,
	"targetUtilization" : 0.8,
	"parms" : "$BENCHMARK_FOLDER/iccad15/evaluate_solution_0831/ICCAD15.parm"
};
HEREDOC

cat > "$CUR_DIR/iccad15/iccad15_superblue18_generic.rsyn" << HEREDOC
open "generic" {
	"lefFiles" : "$BENCHMARK_FOLDER/iccad15/superblue18/superblue18.lef",
	"defFiles" : "$BENCHMARK_FOLDER/iccad15/superblue18/superblue18.def",
	"config" : "$BENCHMARK_FOLDER/iccad15/superblue18/superblue18.iccad2015",
	"maxDisplacement" : 400,
	"targetUtilization" : 0.8,
	"parms" : "$BENCHMARK_FOLDER/iccad15/evaluate_solution_0831/ICCAD15.parm"
};
HEREDOC

cat > "$CUR_DIR/iccad15/iccad15_superblue16_generic.rsyn" << HEREDOC
open "generic" {
	"lefFiles" : "$BENCHMARK_FOLDER/iccad15/superblue16/superblue16.lef",
	"defFiles" : "$BENCHMARK_FOLDER/iccad15/superblue16/superblue16.def",
	"config" : "$BENCHMARK_FOLDER/iccad15/superblue16/superblue16.iccad2015",
	"maxDisplacement" : 400,
	"targetUtilization" : 0.8,
	"parms" : "$BENCHMARK_FOLDER/iccad15/evaluate_solution_0831/ICCAD15.parm"
};
HEREDOC

cat > "$CUR_DIR/iccad15/iccad15_superblue10_generic.rsyn" << HEREDOC
open "generic" {
	"lefFiles" : "$BENCHMARK_FOLDER/iccad15/superblue10/superblue10.lef",
	"defFiles" : "$BENCHMARK_FOLDER/iccad15/superblue10/superblue10.def",
	"config" : "superblue10.iccad2015",
	"maxDisplacement" : 400,
	"targetUtilization" : 0.8,
	"parms" : "$BENCHMARK_FOLDER/iccad15/evaluate_solution_0831/ICCAD15.parm"
};
HEREDOC

cat > "$CUR_DIR/iccad15/iccad15_simple_generic.rsyn" << HEREDOC
open "generic" {
	"lefFiles" : "$BENCHMARK_FOLDER/iccad15/simple/simple.lef",
	"defFiles" : "$BENCHMARK_FOLDER/iccad15/simple/simple.def",
	"config" : "$BENCHMARK_FOLDER/iccad15/simple/simple.iccad2015",
	"maxDisplacement" : 400,
	"targetUtilization" : 0.8,
	"parms" : "$BENCHMARK_FOLDER/iccad15/evaluate_solution_0831/ICCAD15.parm"
};
HEREDOC

: ' 
cat > "$CUR_DIR/iccad15/iccad15_superblue16.rsyn" << HEREDOC
open "iccad2015" {
	"config" : "$BENCHMARK_FOLDER/iccad15/superblue16/superblue16.iccad2015",
	"maxDisplacement" : 400,
	"targetUtilization" : 0.85,
	"parms" : "$BENCHMARK_FOLDER/iccad15/evaluate_solution_0831/ICCAD15.parm"
};
HEREDOC

cat > "$CUR_DIR/iccad15/iccad15_simple.rsyn" << HEREDOC
open "iccad2015" {
	"config" : "$BENCHMARK_FOLDER/iccad15/simple/simple.iccad2015",
	"maxDisplacement" : 400,
	"targetUtilization" : 0.8,
	"parms" : "$BENCHMARK_FOLDER/iccad15/evaluate_solution_0831/ICCAD15.parm"
};
HEREDOC

cat > "$CUR_DIR/iccad15/iccad15_superblue10.rsyn" << HEREDOC
open "iccad2015" {
	"config" : "$BENCHMARK_FOLDER/iccad15/superblue10/superblue10.iccad2015",
	"maxDisplacement" : 400,
	"targetUtilization" : 0.87,
	"parms" : ".$BENCHMARK_FOLDER/iccad15/evaluate_solution_0831/ICCAD15.parm"
};
HEREDOC

cat > "$CUR_DIR/iccad15/iccad15_superblue18.rsyn" << HEREDOC
open "iccad2015" {
	"config" : "$BENCHMARK_FOLDER/iccad15/superblue18/superblue18.iccad2015",
	"maxDisplacement" : 400,
	"targetUtilization" : 0.85,
	"parms" : "$BENCHMARK_FOLDER/iccad15/evaluate_solution_0831/ICCAD15.parm"
};
HEREDOC

cat > "$CUR_DIR/iccad15/iccad15_superblue1.rsyn" << HEREDOC
open "iccad2015" {
	"config" : "$BENCHMARK_FOLDER/iccad15/superblue1/superblue1.iccad2015",
	"maxDisplacement" : 400,
	"targetUtilization" : 0.8,
	"parms" : "$BENCHMARK_FOLDER/iccad15/evaluate_solution_0831/ICCAD15.parm"
};
HEREDOC

cat > "$CUR_DIR/iccad15/iccad15_superblue3.rsyn" << HEREDOC
open "iccad2015" {
	"config" : "$BENCHMARK_FOLDER/iccad15/superblue3/superblue3.iccad2015",
	"maxDisplacement" : 400,
	"targetUtilization" : 0.87,
	"parms" : "$BENCHMARK_FOLDER/iccad15/evaluate_solution_0831/ICCAD15.parm"
};
HEREDOC

cat > "$CUR_DIR/iccad15/iccad15_superblue4.rsyn" << HEREDOC
open "iccad2015" {
	"config" : "$BENCHMARK_FOLDER/iccad15/superblue4/superblue4.iccad2015",
	"maxDisplacement" : 400,
	"targetUtilization" : 0.9,
	"parms" : "$BENCHMARK_FOLDER/iccad15/evaluate_solution_0831/ICCAD15.parm"
};
HEREDOC

cat > "$CUR_DIR/iccad15/iccad15_superblue5.rsyn" << HEREDOC
open "iccad2015" {
	"config" : "$BENCHMARK_FOLDER/iccad15/superblue5/superblue5.iccad2015",
	"maxDisplacement" : 400,
	"targetUtilization" : 0.85,
	"parms" : "$BENCHMARK_FOLDER/iccad15/evaluate_solution_0831/ICCAD15.parm"
};
HEREDOC

cat > "$CUR_DIR/iccad15/iccad15_superblue7.rsyn" << HEREDOC
open "iccad2015" {
	"config" : "$BENCHMARK_FOLDER/iccad15/superblue7/superblue7.iccad2015",
	"maxDisplacement" : 400,
	"targetUtilization" : 0.9,
	"parms" : "$BENCHMARK_FOLDER/iccad15/evaluate_solution_0831/ICCAD15.parm"
};
HEREDOC
'

