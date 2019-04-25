BENCHMARK_FOLDER=$1

CUR_DIR="$( cd "$(dirname "$0")" ; pwd -P )"

if [ ! -d "$CUR_DIR/ispd19" ]; then
	mkdir "$CUR_DIR/ispd19"
fi

cat > "$CUR_DIR/ispd19/ispd19_test9_generic.rsyn" << HEREDOC
open "generic" {
	"lefFiles" : "$BENCHMARK_FOLDER/ispd19/ispd19_test9/ispd19_test9.input.lef",
	"defFiles" : "$BENCHMARK_FOLDER/ispd19/ispd19_test9/ispd19_test9.input.def"
};
HEREDOC

cat > "$CUR_DIR/ispd19/ispd19_test6_generic.rsyn" << HEREDOC
open "generic" {
	"lefFiles" : "$BENCHMARK_FOLDER/ispd19/ispd19_test6/ispd19_test6.input.lef",
	"defFiles" : "$BENCHMARK_FOLDER/ispd19/ispd19_test6/ispd19_test6.input.def"
};
HEREDOC

cat > "$CUR_DIR/ispd19/ispd19_test4_generic.rsyn" << HEREDOC
open "generic" {
	"lefFiles" : "$BENCHMARK_FOLDER/ispd19/ispd19_test4/ispd19_test4.input.lef",
	"defFiles" : "$BENCHMARK_FOLDER/ispd19/ispd19_test4/ispd19_test4.input.def"
};
HEREDOC

cat > "$CUR_DIR/ispd19/ispd19_test_generic.rsyn" << HEREDOC
open "generic" {
	"lefFiles" : "$BENCHMARK_FOLDER/ispd19/ispd19_test/ispd19_test2.input.lef",
	"defFiles" : "$BENCHMARK_FOLDER/ispd19/ispd19_test2/ispd19_test2.input.def"
};
HEREDOC

cat > "$CUR_DIR/ispd19/ispd19_test1_generic.rsyn" << HEREDOC
open "generic" {
	"lefFiles" : "$BENCHMARK_FOLDER/ispd19/ispd19_test1/ispd19_test1.input.lef",
	"defFiles" : "$BENCHMARK_FOLDER/ispd19/ispd19_test1/ispd19_test1.input.def"
};
HEREDOC

cat > "$CUR_DIR/ispd19/ispd19_test3_generic.rsyn" << HEREDOC
open "generic" {
	"lefFiles" : "$BENCHMARK_FOLDER/ispd19/ispd19_test3/ispd19_test3.input.lef",
	"defFiles" : "$BENCHMARK_FOLDER/ispd19/ispd19_test3/ispd19_test3.input.def"
};
HEREDOC

cat > "$CUR_DIR/ispd19/ispd19_sample4_generic.rsyn" << HEREDOC
open "generic" {
	"lefFiles" : "$BENCHMARK_FOLDER/ispd19/ispd19_sample4/ispd19_sample4.input.lef",
	"defFiles" : "$BENCHMARK_FOLDER/ispd19/ispd19_sample4/ispd19_sample4.input.def"
};
HEREDOC

cat > "$CUR_DIR/ispd19/ispd19_sample3_generic.rsyn" << HEREDOC
open "generic" {
	"lefFiles" : "$BENCHMARK_FOLDER/ispd19/ispd19_sample3/ispd19_sample3.input.lef",
	"defFiles" : "$BENCHMARK_FOLDER/ispd19/ispd19_sample3/ispd19_sample3.input.def"
};
HEREDOC

cat > "$CUR_DIR/ispd19/ispd19_sample2_generic.rsyn" << HEREDOC
open "generic" {
	"lefFiles" : "$BENCHMARK_FOLDER/ispd19/ispd19_sample2/ispd19_sample2.input.lef",
	"defFiles" : "$BENCHMARK_FOLDER/ispd19/ispd19_sample2/ispd19_sample2.input.def"
};
HEREDOC

cat > "$CUR_DIR/ispd19/ispd19_sample_generic.rsyn" << HEREDOC
open "generic" {
	"lefFiles" : "$BENCHMARK_FOLDER/ispd19/ispd19_sample/ispd19_sample.input.lef",
	"defFiles" : "$BENCHMARK_FOLDER/ispd19/ispd19_sample/ispd19_sample.input.def"
};
HEREDOC

cat > "$CUR_DIR/ispd19/ispd19_sample2.rsyn" << HEREDOC
open "ispd19" {
	"lefFile" : "$BENCHMARK_FOLDER/ispd19/ispd19_sample2/ispd19_sample2.input.lef",
	"defFile" : "$BENCHMARK_FOLDER/ispd19/ispd19_sample2/ispd19_sample2.input.def",
	"guideFile" : "$BENCHMARK_FOLDER/ispd19/ispd19_sample2/ispd19_sample2.input.guide"
};
HEREDOC

cat > "$CUR_DIR/ispd19/ispd19_sample3.rsyn" << HEREDOC
open "ispd19" {
	"lefFile" : "$BENCHMARK_FOLDER/ispd19/ispd19_sample3/ispd19_sample3.input.lef",
	"defFile" : "$BENCHMARK_FOLDER/ispd19/ispd19_sample3/ispd19_sample3.input.def",
	"guideFile" : "$BENCHMARK_FOLDER/ispd19/ispd19_sample3/ispd19_sample3.input.guide"
};
HEREDOC

cat > "$CUR_DIR/ispd19/ispd19_sample4.rsyn" << HEREDOC
open "ispd19" {
	"lefFile" : "$BENCHMARK_FOLDER/ispd19/ispd19_sample4/ispd19_sample4.input.lef",
	"defFile" : "$BENCHMARK_FOLDER/ispd19/ispd19_sample4/ispd19_sample4.input.def",
	"guideFile" : "$BENCHMARK_FOLDER/ispd19/ispd19_sample4/ispd19_sample4.input.guide"
};
HEREDOC

cat > "$CUR_DIR/ispd19/ispd19_sample.rsyn" << HEREDOC
open "ispd19" {
	"lefFile" : "$BENCHMARK_FOLDER/ispd19/ispd19_sample/ispd19_sample.input.lef",
	"defFile" : "$BENCHMARK_FOLDER/ispd19/ispd19_sample/ispd19_sample.input.def",
	"guideFile" : "$BENCHMARK_FOLDER/ispd19/ispd19_sample/ispd19_sample.input.guide"
};
HEREDOC

cat > "$CUR_DIR/ispd19/ispd19_test1.rsyn" << HEREDOC
open "ispd19" {
	"lefFile" : "$BENCHMARK_FOLDER/ispd19/ispd19_test1/ispd19_test1.input.lef",
	"defFile" : "$BENCHMARK_FOLDER/ispd19/ispd19_test1/ispd19_test1.input.def",
	"guideFile" : "$BENCHMARK_FOLDER/ispd19/ispd19_test1/ispd19_test1.guide"
};
HEREDOC

cat > "$CUR_DIR/ispd19/ispd19_test2.rsyn" << HEREDOC
open "ispd19" {
	"lefFile" : "$BENCHMARK_FOLDER/ispd19/ispd19_test2/ispd19_test2.input.lef",
	"defFile" : "$BENCHMARK_FOLDER/ispd19/ispd19_test2/ispd19_test2.input.def",
	"guideFile" : "$BENCHMARK_FOLDER/ispd19/ispd19_test2/ispd19_test2.guide"
};
HEREDOC

cat > "$CUR_DIR/ispd19/ispd19_test3.rsyn" << HEREDOC
open "ispd19" {
	"lefFile" : "$BENCHMARK_FOLDER/ispd19/ispd19_test3/ispd19_test3.input.lef",
	"defFile" : "$BENCHMARK_FOLDER/ispd19/ispd19_test3/ispd19_test3.input.def",
	"guideFile" : "$BENCHMARK_FOLDER/ispd19/ispd19_test3/ispd19_test3.guide"
};
HEREDOC

cat > "$CUR_DIR/ispd19/ispd19_test4.rsyn" << HEREDOC
open "ispd19" {
	"lefFile" : "$BENCHMARK_FOLDER/ispd19/ispd19_test4/ispd19_test4.input.lef",
	"defFile" : "$BENCHMARK_FOLDER/ispd19/ispd19_test4/ispd19_test4.input.def",
	"guideFile" : "$BENCHMARK_FOLDER/ispd19/ispd19_test4/ispd19_test4.input.guide"
};
HEREDOC

cat > "$CUR_DIR/ispd19/ispd19_test6.rsyn" << HEREDOC
open "ispd18" {
	"lefFile" : "$BENCHMARK_FOLDER/ispd19/ispd19_test6/ispd19_test6.input.lef",
	"defFile" : "$BENCHMARK_FOLDER/ispd19/ispd19_test6/ispd19_test6.input.def",
	"guideFile" : "$BENCHMARK_FOLDER/ispd19/ispd19_test6/ispd19_test6.guide"
};
HEREDOC

cat > "$CUR_DIR/ispd19/ispd19_test9.rsyn" << HEREDOC
open "ispd19" {
	"lefFile" : "$BENCHMARK_FOLDER/ispd19/ispd19_test9/ispd19_test9.input.lef",
	"defFile" : "$BENCHMARK_FOLDER/ispd19/ispd19_test9/ispd19_test9.input.def",
	"guideFile" : "$BENCHMARK_FOLDER/ispd19/ispd19_test9/ispd19_test9.input.guide"
};
HEREDOC

