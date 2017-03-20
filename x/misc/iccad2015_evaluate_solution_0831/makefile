EXE = iccad2015_evaluate_solution
CXX = g++

SRC = $(wildcard *.cpp)
DEP = $(wildcard *.h)
OBJ = flute_new.o

CFLAGS = -std=c++0x -Wall -O3
LFLAGS = -fopenmp -static -L./libs -lui-timer
INC = -I./includes

$(EXE): $(SRC) $(DEP) $(OBJ)	
	/bin/rm -f $(EXE) 
	$(CXX) $(CFLAGS) $(SRC) $(OBJ) $(INC) $(LFLAGS) -o $(EXE)

flute_new.o: Flute/flute.h Flute/flute_new.cpp
	/bin/rm -f flute_new.o
	$(CXX) $(CFLAGS) Flute/flute_new.cpp -c 

.PHONY: test
test: $(EXE) 
	./$(EXE) ICCAD15.parm simple/simple.iccad2015 1.0 simple/final.def simple/final.ops ||true

############################################################################################
dsg = superblue18
ut = 0.85
riccad15: $(EXE)
	./$(EXE) ICCAD15.parm $(dsg)/$(dsg).iccad2015 $(ut) ../bin/$(dsg)-cada085.def ../bin/$(dsg)-cada085.ops || true
############################################################################################

.PHONY: clean
clean:
	/bin/rm -f $(EXE) *.gch $(OBJ)

.PHONY: backup
backup:
	@mkdir -p script-backup/`date +%m%d`
	@tar -cvzf iccad2015_evaluate_solution_`date +%m%d`.tar.gz $(SRC) $(DEP) $(OBJ) $(EXE) makefile *.parm simple Flute *.dat includes libs
	@mv iccad2015_evaluate_solution_`date +%m%d`.tar.gz script-backup/`date +%m%d`
