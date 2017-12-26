#!/bin/sh

if [ -z "$1" ]; then
	echo "Usage: runall-report.sh <log file>"
	exit
fi

grep "  benchmark name  : " $1 |  awk '{print $4}' > _bench
grep "  ABU penalty     : " $1 |  awk '{print $4}' > _abu
grep "  HPWL, StWL (um) : " $1 |  awk '{print $6}'  > _stwl
grep "  early WNS, TNS  : " $1 |  awk '{print $5 $6}' | awk -F , '{printf("%s\t%s\n", $1, $2)}' > _early
grep "  late  WNS, TNS  : " $1 |  awk '{print $5 $6}' | awk -F , '{printf("%s\t%s\n", $1, $2)}' > _late
grep "Running ICCAD 15 Mode... Done" $1 | awk '{print $7}' > _runtime

echo "Benchmark	ABU	StWL	eWNS	eTNS	lWNS	lTNS	Runtime"
paste _bench _abu _stwl _early _late _runtime

rm _bench
rm _abu
rm _stwl
rm _early
rm _late
rm _runtime
