#!/bin/bash

sort ../bin/uplacer.tr -o uplacer_sorted.tr
sort ../eval_solution/uitimer.tr -o uitimer_sorted.tr
diff uplacer_sorted.tr uitimer_sorted.tr > timingReport.diff
gvim timingReport.diff &
