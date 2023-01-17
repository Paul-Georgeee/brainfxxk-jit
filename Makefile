.PHONY: clean
BUILD_DIR := build
CPPFLAGS := -Wall -I./include -I./dynasm

ifeq ($(DEBUG),on)
	CPPFLAGS += -DDEBUG
endif

SRC := $(shell find ./src -name "*.cpp")
OBJ := $(addprefix $(BUILD_DIR)/, $(addsuffix .o, $(basename $(notdir $(SRC)))))

all: $(OBJ)
	g++ $(OBJ) -o $(BUILD_DIR)/bfjit

jit: src/dasc
	lua dynasm/dynasm.lua src/jit.dasc > include/jit.hpp

$(BUILD_DIR)/%.o: src/%.cpp
	@mkdir -p $(dir $@)
	g++ -c $(CPPFLAGS) $< -o $@

clean:
	rm -rf ./build/
