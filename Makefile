EXECUTABLE ?= arka26
SRC_DIR ?= src
BUILD_DIR ?= build

PLATFORM ?= desktop
UNAME := $(shell uname)

MODE ?= debug

CC ?= cc
CFLAGS := -Wall -Wextra -Wpedantic -I$(SRC_DIR)
LDFLAGS :=

ifeq ($(PLATFORM), web)
	CC := emcc
	EXECUTABLE := index.html
	CFLAGS += -pthread -std=gnu11
	LDFLAGS += -sMIN_WEBGL_VERSION=2 -sMAX_WEBGL_VERSION=2 -pthread
else ifeq ($(UNAME), Linux)
	CFLAGS += -std=c11
	LDFLAGS += -lX11 -lXcursor -lGL -lGLX -lm -lXrandr
else ifeq ($(OS), Windows_NT)
	EXECUTABLE += .exe
	CFLAGS += -std=c99
	LDFLAGS += -lopengl32 -lgdi32
endif

ifneq ($(MODE), release)
	CFLAGS += -g3 -O0
else
	CFLAGS += -g0 -O3
endif

SRCS = $(wildcard $(SRC_DIR)/*.c) $(wildcard $(SRC_DIR)/**/*.c)
OBJS = $(addprefix $(BUILD_DIR)/,$(SRCS:.c=.o))

.PHONY: all
all: $(EXECUTABLE)

.PHONY: build
build: $(EXECUTABLE)

.PHONY: rebuild
rebuild: clean build

.PHONY: $(EXECUTABLE)
$(EXECUTABLE): $(BUILD_DIR)/$(EXECUTABLE)

$(BUILD_DIR)/$(EXECUTABLE): $(OBJS)
	$(CC) -o $@ $^ $(LDFLAGS)

$(BUILD_DIR)/%.o: %.c
	@mkdir -p $(dir $@)
	$(CC) -c $(CFLAGS) -o $@ $<

.PHONY: clean
clean:
	@echo "Cleaning all objects and artifacts"
	rm -rf $(BUILD_DIR)/*
