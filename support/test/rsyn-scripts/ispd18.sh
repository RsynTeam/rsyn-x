BENCHMARK_FOLDER=$1

CUR_DIR="$( cd "$(dirname "$0")" ; pwd -P )"

if [ ! -d "$CUR_DIR/ispd18" ]; then
	mkdir "$CUR_DIR/ispd18"
fi


cat > "$CUR_DIR/ispd18/ispd18_test9_generic.rsyn" << HEREDOC
open "generic" {
	"lefFiles" : "$BENCHMARK_FOLDER/ispd18/ispd18_test9/ispd18_test9.input.lef",
	"defFiles" : "$BENCHMARK_FOLDER/ispd18/ispd18_test9/ispd18_test9.input.def"
};
HEREDOC

cat > "$CUR_DIR/ispd18/ispd18_test8_generic.rsyn" << HEREDOC
open "generic" {
	"lefFiles" : "$BENCHMARK_FOLDER/ispd18/ispd18_test8/ispd18_test8.input.lef",
	"defFiles" : "$BENCHMARK_FOLDER/ispd18/ispd18_test8/ispd18_test8.input.def"
};
HEREDOC

cat > "$CUR_DIR/ispd18/ispd18_test7_generic.rsyn" << HEREDOC
open "generic" {
	"lefFiles" : "$BENCHMARK_FOLDER/ispd18/ispd18_test7/ispd18_test7.input.lef",
	"defFiles" : "$BENCHMARK_FOLDER/ispd18/ispd18_test7/ispd18_test7.input.def"
};
HEREDOC

cat > "$CUR_DIR/ispd18/ispd18_test6_generic.rsyn" << HEREDOC
open "generic" {
	"lefFiles" : "$BENCHMARK_FOLDER/ispd18/ispd18_test6/ispd18_test6.input.lef",
	"defFiles" : "$BENCHMARK_FOLDER/ispd18/ispd18_test6/ispd18_test6.input.def"
};
HEREDOC

cat > "$CUR_DIR/ispd18/ispd18_test5_generic.rsyn" << HEREDOC
open "generic" {
	"lefFiles" : "$BENCHMARK_FOLDER/ispd18/ispd18_test5/ispd18_test5.input.lef",
	"defFiles" : "$BENCHMARK_FOLDER/ispd18/ispd18_test5/ispd18_test5.input.def"
};
HEREDOC

cat > "$CUR_DIR/ispd18/ispd18_test4_generic.rsyn" << HEREDOC
open "generic" {
	"lefFiles" : "$BENCHMARK_FOLDER/ispd18/ispd18_test4/ispd18_test4.input.lef",
	"defFiles" : "$BENCHMARK_FOLDER/ispd18/ispd18_test4/ispd18_test4.input.def"
};
HEREDOC

cat > "$CUR_DIR/ispd18/ispd18_test3_generic.rsyn" << HEREDOC
open "generic" {
	"lefFiles" : "$BENCHMARK_FOLDER/ispd18/ispd18_test3/ispd18_test3.input.lef",
	"defFiles" : "$BENCHMARK_FOLDER/ispd18/ispd18_test3/ispd18_test3.input.def"
};
HEREDOC

cat > "$CUR_DIR/ispd18/ispd18_test2_generic.rsyn" << HEREDOC
open "generic" {
	"lefFiles" : "$BENCHMARK_FOLDER/ispd18/ispd18_test2/ispd18_test2.input.lef",
	"defFiles" : "$BENCHMARK_FOLDER/ispd18/ispd18_test2/ispd18_test2.input.def"
};
HEREDOC

cat > "$CUR_DIR/ispd18/ispd18_test1_generic.rsyn" << HEREDOC
open "generic" {
	"lefFiles" : "$BENCHMARK_FOLDER/ispd18/ispd18_test1/ispd18_test1.input.lef",
	"defFiles" : "$BENCHMARK_FOLDER/ispd18/ispd18_test1/ispd18_test1.input.def"
};
HEREDOC

cat > "$CUR_DIR/ispd18/ispd18_test10_generic.rsyn" << HEREDOC
open "generic" {
	"lefFiles" : "$BENCHMARK_FOLDER/ispd18/ispd18_test10/ispd18_test10.input.lef",
	"defFiles" : "$BENCHMARK_FOLDER/ispd18/ispd18_test10/ispd18_test10.input.def"
};
HEREDOC

cat > "$CUR_DIR/ispd18/ispd18_sample3_generic.rsyn" << HEREDOC
open "generic" {
	"lefFiles" : "$BENCHMARK_FOLDER/ispd18/ispd18_sample3/ispd18_sample3.input.lef",
	"defFiles" : "$BENCHMARK_FOLDER/ispd18/ispd18_sample3/ispd18_sample3.input.def"
};
HEREDOC

cat > "$CUR_DIR/ispd18/ispd18_sample2_generic.rsyn" << HEREDOC
open "generic" {
	"lefFiles" : "$BENCHMARK_FOLDER/ispd18/ispd18_sample2/ispd18_sample2.input.lef",
	"defFiles" : "$BENCHMARK_FOLDER/ispd18/ispd18_sample2/ispd18_sample2.input.def"
};
HEREDOC

cat > "$CUR_DIR/ispd18/ispd18_sample_generic.rsyn" << HEREDOC
open "generic" {
	"lefFiles" : "$BENCHMARK_FOLDER/ispd18/ispd18_sample/ispd18_sample.input.lef",
	"defFiles" : "$BENCHMARK_FOLDER/ispd18/ispd18_sample/ispd18_sample.input.def"
};
HEREDOC

cat > "$CUR_DIR/ispd18/ispd18_sample.rsyn" << HEREDOC
open "ispd18" {
	"lefFile" : "$BENCHMARK_FOLDER/ispd18/ispd18_sample/ispd18_sample.input.lef",
	"defFile" : "$BENCHMARK_FOLDER/ispd18/ispd18_sample/ispd18_sample.input.def",
	"guideFile" : "$BENCHMARK_FOLDER/ispd18/ispd18_sample/ispd18_sample.input.guide"
};
HEREDOC

cat > "$CUR_DIR/ispd18/ispd18_sample2.rsyn" << HEREDOC
open "ispd18" {
	"lefFile" : "$BENCHMARK_FOLDER/ispd18/ispd18_sample2/ispd18_sample2.input.lef",
	"defFile" : "$BENCHMARK_FOLDER/ispd18/ispd18_sample2/ispd18_sample2.input.def",
	"guideFile" : "$BENCHMARK_FOLDER/ispd18/ispd18_sample2/ispd18_sample2.input.guide"
};
HEREDOC

cat > "$CUR_DIR/ispd18/ispd18_sample3.rsyn" << HEREDOC
open "ispd18" {
	"lefFile" : "$BENCHMARK_FOLDER/ispd18/ispd18_sample3/ispd18_sample3.input.lef",
	"defFile" : "$BENCHMARK_FOLDER/ispd18/ispd18_sample3/ispd18_sample3.input.def",
	"guideFile" : "$BENCHMARK_FOLDER/ispd18/ispd18_sample3/ispd18_sample3.input.guide"
};
HEREDOC

cat > "$CUR_DIR/ispd18/ispd18_test10.rsyn" << HEREDOC
open "ispd18" {
	"lefFile" : "$BENCHMARK_FOLDER/ispd18/ispd18_test10/ispd18_test10.input.lef",
	"defFile" : "$BENCHMARK_FOLDER/ispd18/ispd18_test10/ispd18_test10.input.def",
	"guideFile" : "$BENCHMARK_FOLDER/ispd18/ispd18_test10/ispd18_test10.input.guide"
};
HEREDOC

cat > "$CUR_DIR/ispd18/ispd18_test1.rsyn" << HEREDOC
open "ispd18" {
	"lefFile" : "$BENCHMARK_FOLDER/ispd18/ispd18_test1/ispd18_test1.input.lef",
	"defFile" : "$BENCHMARK_FOLDER/ispd18/ispd18_test1/ispd18_test1.input.def",
	"guideFile" : "$BENCHMARK_FOLDER/ispd18/ispd18_test1/ispd18_test1.input.guide"
};
HEREDOC

cat > "$CUR_DIR/ispd18/ispd18_test2.rsyn" << HEREDOC
open "ispd18" {
	"lefFile" : "$BENCHMARK_FOLDER/ispd18/ispd18_test2/ispd18_test2.input.lef",
	"defFile" : "$BENCHMARK_FOLDER/ispd18/ispd18_test2/ispd18_test2.input.def",
	"guideFile" : "$BENCHMARK_FOLDER/ispd18/ispd18_test2/ispd18_test2.input.guide"
};
HEREDOC

cat > "$CUR_DIR/ispd18/ispd18_test3.rsyn" << HEREDOC
open "ispd18" {
	"lefFile" : "$BENCHMARK_FOLDER/ispd18/ispd18_test3/ispd18_test3.input.lef",
	"defFile" : "$BENCHMARK_FOLDER/ispd18/ispd18_test3/ispd18_test3.input.def",
	"guideFile" : "$BENCHMARK_FOLDER/ispd18/ispd18_test3/ispd18_test3.input.guide"
};
HEREDOC

cat > "$CUR_DIR/ispd18/ispd18_test4.rsyn" << HEREDOC
open "ispd18" {
	"lefFile" : "$BENCHMARK_FOLDER/ispd18/ispd18_test4/ispd18_test4.input.lef",
	"defFile" : "$BENCHMARK_FOLDER/ispd18/ispd18_test4/ispd18_test4.input.def",
	"guideFile" : "$BENCHMARK_FOLDER/ispd18/ispd18_test4/ispd18_test4.input.guide"
};
HEREDOC

cat > "$CUR_DIR/ispd18/ispd18_test5.rsyn" << HEREDOC
open "ispd18" {
	"lefFile" : "$BENCHMARK_FOLDER/ispd18/ispd18_test5/ispd18_test5.input.lef",
	"defFile" : "$BENCHMARK_FOLDER/ispd18/ispd18_test5/ispd18_test5.input.def",
	"guideFile" : "$BENCHMARK_FOLDER/ispd18/ispd18_test5/ispd18_test5.input.guide"
};
HEREDOC

cat > "$CUR_DIR/ispd18/ispd18_test6.rsyn" << HEREDOC
open "ispd18" {
	"lefFile" : "$BENCHMARK_FOLDER/ispd18/ispd18_test6/ispd18_test6.input.lef",
	"defFile" : "$BENCHMARK_FOLDER/ispd18/ispd18_test6/ispd18_test6.input.def",
	"guideFile" : "$BENCHMARK_FOLDER/ispd18/ispd18_test6/ispd18_test6.input.guide"
};
HEREDOC

cat > "$CUR_DIR/ispd18/ispd18_test7.rsyn" << HEREDOC
open "ispd18" {
	"lefFile" : "$BENCHMARK_FOLDER/ispd18/ispd18_test7/ispd18_test7.input.lef",
	"defFile" : "$BENCHMARK_FOLDER/ispd18/ispd18_test7/ispd18_test7.input.def",
	"guideFile" : "$BENCHMARK_FOLDER/ispd18/ispd18_test7/ispd18_test7.input.guide"
};
HEREDOC

cat > "$CUR_DIR/ispd18/ispd18_test8.rsyn" << HEREDOC
open "ispd18" {
	"lefFile" : "$BENCHMARK_FOLDER/ispd18/ispd18_test8/ispd18_test8.input.lef",
	"defFile" : "$BENCHMARK_FOLDER/ispd18/ispd18_test8/ispd18_test8.input.def",
	"guideFile" : "$BENCHMARK_FOLDER/ispd18/ispd18_test8/ispd18_test8.input.guide"
};
HEREDOC

cat > "$CUR_DIR/ispd18/ispd18_test9.rsyn" << HEREDOC
open "ispd18" {
	"lefFile" : "$BENCHMARK_FOLDER/ispd18/ispd18_test9/ispd18_test9.input.lef",
	"defFile" : "$BENCHMARK_FOLDER/ispd18/ispd18_test9/ispd18_test9.input.def",
	"guideFile" : "$BENCHMARK_FOLDER/ispd18/ispd18_test9/ispd18_test9.input.guide"
};
HEREDOC
