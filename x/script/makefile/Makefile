-include run.mk

INFOMSG := "\033[1;34m[INFO]\033[0m" 

#NUMPROCS := $(shell grep -c ^processor /proc/cpuinfo)
NUMPROCS := $(shell nproc)
MAKEFLAGS += -j${NUMPROCS}

BINDIR := .
OBJDIR := ./obj

RSYN_INCDIR := ../include
RSYN_LIBDIR := ../lib

X_INCDIR := ../../rsyn/include
X_LIBDIR := ../../rsyn/lib

SRCDIR := ./src
GUIDIR := $(SRCDIR)/rsyn/gui
GUXDIR := $(SRCDIR)/x/gui

EXEGUI = ${BINDIR}/rsyn-gui
EXECMD = ${BINDIR}/rsyn-cmd

OBJDIR_COM = $(OBJDIR)/common
OBJDIR_CMD = $(OBJDIR)/cmd
OBJDIR_GUI = $(OBJDIR)/gui

MAIN = $(SRCDIR)/x/main.cpp

CSRCS := $(shell find -L $(SRCDIR) -name '*.c')
CCSRCS := $(filter-out $(MAIN), $(filter-out $(GUXDIR)%, $(filter-out $(GUIDIR)%, $(shell find -L $(SRCDIR) -name '*.cc'))))

CPPSRCS := $(filter-out $(MAIN), $(filter-out $(GUXDIR)%, $(filter-out $(GUIDIR)%, $(shell find -L $(SRCDIR) -name '*.cpp'))))
CMDSRCS := $(MAIN)
GUISRCS := $(MAIN) $(shell find -L $(GUIDIR) -name '*.cpp') $(shell find -L $(GUXDIR) -name '*.cpp')


DEPS := $(shell find -L $(OBJDIR) -name '*.d')

LIBERTYFLAG := -DPACKAGE_NAME=\"liberty_parser\" -DPACKAGE_TARNAME=\"liberty_parser\" -DPACKAGE_VERSION=\"2.5\" "-DPACKAGE_STRING=\"liberty_parser 2.5\"" -DPACKAGE_BUGREPORT=\"\" -DPACKAGE=\"liberty_parser\" -DSTDC_HEADERS=1 -DHAVE_SYS_TYPES_H=1 -DHAVE_SYS_STAT_H=1 -DHAVE_STDLIB_H=1 -DHAVE_STRING_H=1 -DHAVE_MEMORY_H=1 -DHAVE_STRINGS_H=1 -DHAVE_INTTYPES_H=1 -DHAVE_STDINT_H=1 -DHAVE_UNISTD_H=1 -DHAVE_DLFCN_H=1 -DSTDC_HEADERS=1 -DHAVE_DIRENT_H=1 -DHAVE_MALLOC_H=1 -DHAVE_UNISTD_H=1 -DHAVE_STRSTR=1 -DHAVE_STRTOD=1 -I. -DYYERROR_VERBOSE

ROUTERLIBS = -lnctugr -fopenmp
COMMONFLAGS = $(OPTOFLAGS) -I$(SRCDIR) -I$(RSYN_INCDIR) -I$(X_INCDIR) -I$(RSYN_SRCDIR)/parser/liberty -lm -Wno-unused-result -Wno-format -Wno-return-type -Wno-unused-local-typedefs 
COMMONLIBS = -L$(RSYN_LIBDIR) -L$(X_LIBDIR) $(ROUTERLIBS) -llef -ldef -lmunkres -pthread -lboost_filesystem -lboost_system -lboost_program_options -lemon -lCbc -lCbcSolver -lCgl -lOsi -lOsiCbc -lOsiClp -lCoinUtils -lClp -lpng


CFLAGS = $(COMMONFLAGS) 
CPPFLAGS = $(COMMONFLAGS) $(CPPSTD)
CMDFLAGS = -DRSYN_NO_GUI
GUIFLAGS = `wx-config --debug=no --cppflags` $(CPPSTD)
DEBUGFLAGS = -DDEBUG -DDIAGNOSTIC -DRSYN_DEBUG -g3
OPTOFLAGS = -O3

CLIBS = $(COMMONLIBS) -lm
CPPLIBS = $(COMMONLIBS)
GUILIBS = `wx-config --debug=no --libs --gl-libs` `wx-config --optional-libs propgrid` -lGL -lglut -lGLU -lGLEW


CC = @gcc
CXX = @g++

CPPSTD = -std=c++11 

COBJS := ${CSRCS:$(SRCDIR)/%.c=${OBJDIR_COM}/%.o}
CCOBJS := ${CCSRCS:$(SRCDIR)/%.cc=${OBJDIR_COM}/%.o}
CPPOBJS := ${CPPSRCS:$(SRCDIR)/%.cpp=${OBJDIR_COM}/%.o}
GUIOBJS := ${GUISRCS:$(SRCDIR)/%.cpp=${OBJDIR_GUI}/%.o}
CMDOBJS := ${CMDSRCS:$(SRCDIR)/%.cpp=${OBJDIR_CMD}/%.o}

-include ${DEPS}

# -------------------------------------------------------------------------------------------------

all: cmd gui 
	
# -------------------------------------------------------------------------------------------------

cmd: ${CMDOBJS} ${COBJS} ${CCOBJS} ${CPPOBJS}
	@echo $(INFOMSG) Linking $@
	$(CXX) -o $(EXECMD) $(CMDFLAGS) ${CMDOBJS} ${COBJS} ${CCOBJS} ${CPPOBJS} $(CLIBS) $(CPPLIBS) 

# -------------------------------------------------------------------------------------------------

gui: ${GUIOBJS} ${COBJS} ${CCOBJS} ${CPPOBJS}
	@echo $(INFOMSG) Linking $@
	$(CXX) -o $(EXEGUI) ${GUIOBJS} ${COBJS} ${CCOBJS} ${CPPOBJS} $(CLIBS) $(CPPLIBS) $(GUILIBS) $(GUIFLAGS) 

# -------------------------------------------------------------------------------------------------

$(CPPOBJS): $(OBJDIR_COM)/%.o: $(SRCDIR)/%.cpp
	@mkdir -p $(dir $@)
	@echo $(INFOMSG) Compiling $< ...
	$(CXX) $(CPPFLAGS) -c -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.o)" -o "$@" "$<"

$(CCOBJS): $(OBJDIR_COM)/%.o: $(SRCDIR)/%.cc
	@mkdir -p $(dir $@)
	@echo $(INFOMSG) Compiling $< ...
	$(CXX) $(CPPFLAGS) -c -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.o)" -o "$@" "$<"

${COBJS}: $(OBJDIR_COM)/%.o: $(SRCDIR)/%.c
	@mkdir -p $(dir $@)
	@echo $(INFOMSG) Compiling $< ...
	$(CC) $(CFLAGS) -c -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.o)" -o "$@" "$<"

${CMDOBJS}: $(OBJDIR_CMD)/%.o: $(SRCDIR)/%.cpp
	@mkdir -p $(dir $@)
	@echo $(INFOMSG) Compiling $< ...
	$(CXX) $(CPPFLAGS) $(CMDFLAGS) -c -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.o)" -o "$@" "$<"

${GUIOBJS}: $(OBJDIR_GUI)/%.o: $(SRCDIR)/%.cpp
	@mkdir -p $(dir $@)
	@echo $(INFOMSG) Compiling $< ...
	$(CXX) $(CPPFLAGS) $(GUIFLAGS) -c -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.o)" -o "$@" "$<"

# -------------------------------------------------------------------------------------------------

tracetime: COMMONFLAGS:= $(filter-out $(OPTOFLAGS),$(COMMONFLAGS)) $(DEBUGFLAGS) -pg
tracetime: COMMONLIBS+= -pg
tracetime:  all

# -------------------------------------------------------------------------------------------------

debug: COMMONFLAGS:= $(filter-out $(OPTOFLAGS),$(COMMONFLAGS)) $(DEBUGFLAGS)
debug: all

# -------------------------------------------------------------------------------------------------

clean:
	rm -rf ${OBJDIR}/*
	rm -rf ${EXEGUI}
	rm -rf ${EXECMD}

.PHONY: clean gui all cmd tracetime debug
