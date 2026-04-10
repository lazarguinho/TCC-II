# Compilador e flags
CXX = clang++
CXXFLAGS = -std=c++17 -Wall -Wextra -g -Iinclude

# Diretórios
SRC_DIR = src
INC_DIR = include
BIN_DIR = bin
TARGET = $(BIN_DIR)/main

# Descobre todos os .cpp e gera a lista de .o correspondente
SRCS = $(wildcard $(SRC_DIR)/*.cpp)
OBJS = $(SRCS:$(SRC_DIR)/%.cpp=$(BIN_DIR)/%.o)

# Regra padrão
all: $(TARGET)

# Linkagem final
$(TARGET): $(OBJS)
	@mkdir -p $(BIN_DIR)
	$(CXX) $(CXXFLAGS) -o $@ $^

# Compilação de cada .cpp em .o
$(BIN_DIR)/%.o: $(SRC_DIR)/%.cpp $(INC_DIR)/greedy.hpp
	@mkdir -p $(BIN_DIR)
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Limpar objetos e binário
clean:
	rm -rf $(BIN_DIR)

# Rodar o executável
run: all
	$(TARGET)