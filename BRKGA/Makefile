# Compilador e flags
CXX = clang++
CXXSTD    ?= -std=c++17

WARN      ?= -Wall -Wextra -Wpedantic
OPT       ?= -O2
DEBUG     ?=

# Diretórios
SRC_DIR   := src
BUILD_DIR := build
BIN_DIR   := bin

# Includes (pra achar headers em src/ e src/brkgaAPI/)
INCLUDES  := -I$(SRC_DIR) -I$(SRC_DIR)/brkgaAPI

# Flags finais
CXXFLAGS  := $(CXXSTD) $(WARN) $(OPT) $(DEBUG) $(INCLUDES)
LDFLAGS   :=
LDLIBS    :=

UNAME_S := $(shell uname -s)

ifeq ($(UNAME_S),Darwin)
  BREW_PREFIX := $(shell brew --prefix)
  CXXFLAGS += -I$(BREW_PREFIX)/opt/libomp/include
  LDFLAGS  += -L$(BREW_PREFIX)/opt/libomp/lib
else
  OMP_CXXFLAGS := -fopenmp
  OMP_LDFLAGS  := -fopenmp
endif

CXXFLAGS += $(OMP_CXXFLAGS)
LDFLAGS  += $(OMP_LDFLAGS)

# ===== Files =====
# Todos os .cpp dentro de src (inclui src/brkgaAPI/*.cpp)
SRCS_ALL  := $(shell find $(SRC_DIR) -name "*.cpp" -type f)

# main e teste separados
MAIN_SRC  := $(SRC_DIR)/main.cpp
TEST_SRC  := $(SRC_DIR)/teste.cpp

# remove main.cpp da lista geral pra evitar duplicação no link
SRCS_NO_MAIN := $(filter-out $(MAIN_SRC),$(SRCS_ALL))
SRCS_NO_TEST := $(filter-out $(TEST_SRC),$(SRCS_ALL))

# objetos
OBJS_MAIN := $(patsubst $(SRC_DIR)/%.cpp,$(BUILD_DIR)/%.o,$(SRCS_NO_TEST))
OBJS_TEST := $(patsubst $(SRC_DIR)/%.cpp,$(BUILD_DIR)/%.o,$(SRCS_NO_MAIN))

# targets finais
MAIN_BIN  := $(BIN_DIR)/main
TEST_BIN  := $(BIN_DIR)/teste

# ===== Default =====
.PHONY: all
all: $(MAIN_BIN) $(if $(wildcard $(TEST_SRC)),$(TEST_BIN),)

# ===== Link =====
$(MAIN_BIN): $(OBJS_MAIN) $(BUILD_DIR)/main.o | $(BIN_DIR)
	$(CXX) $(CXXFLAGS) $^ -o $@ $(LDFLAGS) $(LDLIBS)

# Só tenta buildar o teste se existir o arquivo
ifeq ($(wildcard $(TEST_SRC)),)
$(TEST_BIN):
	@echo ">> src/teste.cpp não existe; pulando target teste."
else
$(TEST_BIN): $(OBJS_TEST) $(BUILD_DIR)/teste.o | $(BIN_DIR)
	$(CXX) $(CXXFLAGS) $^ -o $@ $(LDFLAGS) $(LDLIBS)
endif

# ===== Compile =====
# Regra genérica: src/x.cpp -> build/x.o
$(BUILD_DIR)/%.o: $(SRC_DIR)/%.cpp
	@mkdir -p $(@D)
	$(CXX) $(CXXFLAGS) -c $< -o $@

# ===== Folders =====
$(BIN_DIR):
	@mkdir -p $(BIN_DIR)

# ===== Convenience =====
.PHONY: clean
clean:
	rm -rf $(BUILD_DIR) $(BIN_DIR)

.PHONY: run
run: $(MAIN_BIN)
	./$(MAIN_BIN)

.PHONY: run-test
run-test: $(TEST_BIN)
	./$(TEST_BIN)

# ===== Build modes =====
.PHONY: release
release:
	$(MAKE) OPT="-O3" DEBUG="" all

.PHONY: debug
debug:
	$(MAKE) OPT="-O0" DEBUG="-g -DDEBUG" all