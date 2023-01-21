.PHONY: clean
BUILD_DIR := build
CPPFLAGS := -Wall. -I./include

ifeq ($(DEBUG),on)
	CPPFLAGS += -DDEBUG
endif

SRC := $(shell find ./src -name "*.cpp")
OBJ := $(addprefix $(BUILD_DIR)/, $(addsuffix .o, $(basename $(notdir $(SRC)))))

all: $(OBJ)
	g++ $(OBJ) -o $(BUILD_DIR)/bfjit

$(BUILD_DIR)/%.o: src/%.cpp
	@mkdir -p $(dir $@)
	g++ -c $(CPPFLAGS) $< -o $@

clean:
	rm -rf ./build/
