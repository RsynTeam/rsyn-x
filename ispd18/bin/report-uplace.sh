#!/bin/sh

if [ -z "$1" ]; then
	echo "Usage: report-uplace.sh <log file>"
	exit
fi

grep "Final Result: superblue" $1 |  awk '{printf ("%s\t%s\t%s\t%s\t%s\t%s\t%s\t%s\n", $3, $10, $12, $6, $7, $8, $9, $11)}' > _results
grep "Flow: ISPD 2016... Done" $1 | awk '{print $6}' > _runtime

echo "Benchmark	ABU	StWL	eWNS	eTNS	lWNS	lTNS    QS	Runtime"
paste _results _runtime

rm _results
rm _runtime
