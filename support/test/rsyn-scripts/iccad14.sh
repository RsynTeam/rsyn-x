BENCHMARK_FOLDER=$1

CUR_DIR="$( cd "$(dirname "$0")" ; pwd -P )"

if [ ! -d "$CUR_DIR/iccad14" ]; then
	mkdir "$CUR_DIR/iccad14"
fi

cat > "$CUR_DIR/iccad14/iccad14_b19.rsyn" << HEREDOC
open "generic" {
	"lefFiles" : "$BENCHMARK_FOLDER/iccad14/b19/techlib.lef",
	"defFiles" : "$BENCHMARK_FOLDER/iccad14/b19/b19.def",
	"config" : "$BENCHMARK_FOLDER/iccad14/b19/b19.iccad2014",
	"maxDisplacement" : 200,
	"targetUtilization" : 0.76,
	"parms" : "$BENCHMARK_FOLDER/iccad14/ICCAD14.parm"
};
HEREDOC

cat > "$CUR_DIR/iccad14/iccad14_leon2.rsyn" << HEREDOC
open "generic" {
	"lefFiles" : "$BENCHMARK_FOLDER/iccad14/leon2/techlib.lef",
	"defFiles" : "$BENCHMARK_FOLDER/iccad14/leon2/leon2.def",
	"config" : "$BENCHMARK_FOLDER/iccad14/leon2/leon2.iccad2014",
	"maxDisplacement" : 200,
	"targetUtilization" : 0.76,
	"parms" : "$BENCHMARK_FOLDER/iccad14/ICCAD14.parm"
};
HEREDOC

cat > "$CUR_DIR/iccad14/iccad14_leon3mp.rsyn" << HEREDOC
open "generic" {
	"lefFiles" : "$BENCHMARK_FOLDER/iccad14/leon3mp/techlib.lef",
	"defFiles" : "$BENCHMARK_FOLDER/iccad14/leon3mp/leon3mp.def",
	"config" : "$BENCHMARK_FOLDER/iccad14/leon3mp/leon3mp.iccad2014",
	"maxDisplacement" : 200,
	"targetUtilization" : 0.76,
	"parms" : "$BENCHMARK_FOLDER/iccad14/ICCAD14.parm"
};
HEREDOC

cat > "$CUR_DIR/iccad14/iccad14_mgc_edit_dist.rsyn" << HEREDOC
open "generic" {
	"lefFiles" : "$BENCHMARK_FOLDER/iccad14/mgc_edit_dist/techlib.lef",
	"defFiles" : "$BENCHMARK_FOLDER/iccad14/mgc_edit_dist/mgc_edit_dist.def",
	"config" : "$BENCHMARK_FOLDER/iccad14/mgc_edit_dist/mgc_edit_dist.iccad2014",
	"maxDisplacement" : 200,
	"targetUtilization" : 0.76,
	"parms" : "$BENCHMARK_FOLDER/iccad14/ICCAD14.parm"
};
HEREDOC

cat > "$CUR_DIR/iccad14/iccad14_mgc_matrix_mult.rsyn" << HEREDOC
open "generic" {
	"lefFiles" : "$BENCHMARK_FOLDER/iccad14/mgc_matrix_mult/techlib.lef",
	"defFiles" : "$BENCHMARK_FOLDER/iccad14/mgc_matrix_mult/mgc_matrix_mult.def",
	"config" : "$BENCHMARK_FOLDER/iccad14/mgc_matrix_mult/mgc_matrix_mult.iccad2014",
	"maxDisplacement" : 200,
	"targetUtilization" : 0.76,
	"parms" : "$BENCHMARK_FOLDER/iccad14/ICCAD14.parm"
};
HEREDOC

cat > "$CUR_DIR/iccad14/iccad14_netcard.rsyn" << HEREDOC
open "generic" {
	"lefFiles" : "$BENCHMARK_FOLDER/iccad14/netcard/techlib.lef",
	"defFiles" : "$BENCHMARK_FOLDER/iccad14/netcard/netcard.def",
	"config" : "$BENCHMARK_FOLDER/iccad14/netcard/netcard.iccad2014",
	"maxDisplacement" : 200,
	"targetUtilization" : 0.76,
	"parms" : "$BENCHMARK_FOLDER/iccad14/ICCAD14.parm"
};
HEREDOC

cat > "$CUR_DIR/iccad14/iccad14_simple.rsyn" << HEREDOC
open "generic" {
	"lefFiles" : "$BENCHMARK_FOLDER/iccad14/simple/techlib.lef",
	"defFiles" : "$BENCHMARK_FOLDER/iccad14/simple/simple.def",
	"config" : "$BENCHMARK_FOLDER/iccad14/simple/simple.iccad2014",
	"maxDisplacement" : 200,
	"targetUtilization" : 0.76,
	"parms" : "$BENCHMARK_FOLDER/iccad14/ICCAD14.parm"
};
HEREDOC

cat > "$CUR_DIR/iccad14/iccad14_vga_lcd.rsyn" << HEREDOC
open "generic" {
	"lefFiles" : "$BENCHMARK_FOLDER/iccad14/vga_lcd/techlib.lef",
	"defFiles" : "$BENCHMARK_FOLDER/iccad14/vga_lcd/vga_lcd.def",
	"config" : "$BENCHMARK_FOLDER/iccad14/vga_lcd/vga_lcd.iccad2014",
	"maxDisplacement" : 200,
	"targetUtilization" : 0.76,
	"parms" : "$BENCHMARK_FOLDER/iccad14/ICCAD14.parm"
};
HEREDOC

