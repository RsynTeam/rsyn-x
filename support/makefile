VERBOSE = 0

ifeq ($(VERBOSE), 1)
	V =
else
	V = @
endif

CMD = bash

N_TRHEADS = 1

CURRENT_DIR= $(shell pwd)
DIR= $(CURRENT_DIR)/../

IMG_NAME= rsynteam/rsyn-regression:latest

USER_ID = $(shell id -u $$USER)

all: docker

build:
	$(V) echo "Building Docker image: $(IMG_NAME)"
ifeq ($(VERBOSE), 1)
	$(V) docker build -t $(IMG_NAME) .
else
	$(V) echo -n "Docker image sha: "
	$(V) docker build -q -t $(IMG_NAME) .
endif

docker:
	$(V) echo "Running Docker image ($(IMG_NAME)) with command: $(CMD)"
ifeq ($(CMD), bash)
	$(V) docker run --rm -it -e LOCAL_USER_ID=$(USER_ID) -v $(DIR):/rsyn $(IMG_NAME) $(CMD)
else
	$(V) docker run --rm -e LOCAL_USER_ID=$(USER_ID) -v $(DIR):/rsyn $(IMG_NAME) $(CMD)
endif

compile:
	$(V) echo "Compiling Rsyn in  Docker image ($(IMG_NAME))"
	$(V) make -j$(N_TRHEADS) docker CMD="/rsyn/support/compile-rsyn.sh"
