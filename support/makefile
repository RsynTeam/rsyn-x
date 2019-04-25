VERBOSE = 0

ifeq ($(VERBOSE), 1)
	V =
else
	V = @
endif

CMD = bash

N_TRHEADS = 4

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

remove:
	$(V) docker stop rsyn && docker rm rsyn || true

create:
	$(V) docker run --name rsyn -it -d -e LOCAL_USER_ID=$(USER_ID) -v $(DIR):/rsyn $(IMG_NAME) "/bin/bash"

docker:
	$(V) echo "Running Docker image ($(IMG_NAME)) with command: $(CMD)"
ifeq ($(CMD), bash)
	$(V) docker run --rm -it -e LOCAL_USER_ID=$(USER_ID) -v $(DIR):/rsyn $(IMG_NAME) $(CMD)
else
	$(V) docker run --rm -e LOCAL_USER_ID=$(USER_ID) -v $(DIR):/rsyn $(IMG_NAME) $(CMD)
endif

execute:
	docker exec -it rsyn $(CMD)

compile:
	$(V) echo "Compiling Rsyn in  Docker image ($(IMG_NAME))"
	$(V) make -j$(N_TRHEADS) execute CMD="/rsyn/support/compile-rsyn.sh"

runiccad14:
	$(V) echo "Running ICCAD14 regression tests"
	$(V) make execute CMD="/rsyn/support/test/run-tests.sh --benchmarks=iccad14"

runiccad15:
	$(V) echo "Running ICCAD15 regression tests"
	$(V) make execute CMD="/rsyn/support/test/run-tests.sh --benchmarks=iccad15"

runispd18:
	$(V) echo "Running ISPD18 regression tests"
	$(V) make execute CMD="/rsyn/support/test/run-tests.sh --benchmarks=ispd18"

runispd19:
	$(V) echo "Running ISPD19 regression tests"
	$(V) make execute CMD="/rsyn/support/test/run-tests.sh --benchmarks=ispd19"

