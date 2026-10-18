TARGET   = test
CXX      = g++
CXXFLAGS = -Wall -Wextra -Werror -O2 -Os -march=native -std=c++20 -MMD -MP
CPPFLAGS = -Isrc

SRC_DIR = src
OBJ_DIR = obj
BIN_DIR = bin

SRCS = $(wildcard $(SRC_DIR)/*.cpp)
OBJS = $(patsubst $(SRC_DIR)/%.cpp,$(OBJ_DIR)/%.o,$(SRCS))
DEPS = $(OBJS:.o=.d)

BIN = $(BIN_DIR)/$(TARGET)

.PHONY: all clean

all: $(BIN)

$(BIN): $(OBJS) | $(BIN_DIR)
	$(CXX) $(CXXFLAGS) $(OBJS) -o $@

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp | $(OBJ_DIR)
	$(CXX) $(CXXFLAGS) $(CPPFLAGS) -c $< -o $@

$(BIN_DIR) $(OBJ_DIR):
	mkdir -p $@

clean:
	rm -rf $(BIN_DIR) $(OBJ_DIR)

-include $(DEPS)