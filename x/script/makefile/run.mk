
# Only set the default value if it's not defined in the enviroment.
ifeq ($(origin benchmarks_path), undefined)
benchmarks_path = benchmark/iccad2015
endif

dsg		= superblue18
name		= superblue18-long
gui		= iccad
script 		= $(benchmarks_path)/$(dsg)/$(name).rsyn

rgui: gui
	$(EXEGUI) -gui $(gui) &

rscript: cmd 
	(time $(EXECMD) -script $(script))

rshell: cmd
	(time $(EXECMD) -interative)

rgprof:  
	gprof $(EXECMD)  gmon.out

rval:
	valgrind --leak-check=full --log-file="log_valgrind_uplace.out" $(EXEGUI) -gui $(gui) &
	
rvalscript:
	valgrind --leak-check=full --log-file="log_valgrind_uplace.out" $(EXECMD) -script $(script) &

.PHONY:  rgui rscript rshell rgprof rval
