EXECUTABLE ?= arka26
SRC_DIR ?= src
BUILD_DIR ?= build
BIN_DIR ?= bin

UNAME := $(shell uname)

MODE ?= debug

CC ?= cc
CFLAGS := -Wall -Wextra -Wpedantic -std=c99 -I$(SRC_DIR)
LDFLAGS := 

ifeq ($(UNAME), Linux)
	LDFLAGS += -lX11 -lGL -lGLX -lm -lXrandr
else ifeq ($(OS), Windows_NT)
	LDFLAGS += -lopengl32 -lgdi32
endif

ifneq ($(MODE), release)
	CFLAGS += -g3 -O0
else
	CFLAGS += -g0 -O3
endif

SRCS = $(wildcard $(SRC_DIR)/*.c)
OBJS = $(addprefix $(BUILD_DIR)/,$(SRCS:.c=.o))

.PHONY: all
build: $(EXECUTABLE)

.PHONY: build
build: $(EXECUTABLE)

.PHONY: $(EXECUTABLE)
$(EXECUTABLE): $(BIN_DIR)/$(EXECUTABLE)

$(BIN_DIR)/$(EXECUTABLE): $(OBJS)
	@mkdir -p $(BIN_DIR)
	$(CC) $(CFLAGS) -o $(BIN_DIR)/$(EXECUTABLE) $^ $(LDFLAGS)

$(BUILD_DIR)/%.o: %.c
	@mkdir -p $(dir $@)
	$(CC) -c $(CFLAGS) -o $@ $<

.PHONY: clean
clean:
	@echo "Cleaning all objects and artifacts"
	@rm -f $(OBJS)
	@rm -f $(BIN_DIR)/$(EXECUTABLE)