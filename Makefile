CXX      := g++
CXXFLAGS := -std=c++11 -Wall -Wextra -O2

INC_DIR := include
SRC_DIR := src
OBJ_DIR := obj
BIN_DIR := bin

# ALTERADO: tp1.out para tp2.out
TARGET := $(BIN_DIR)/tp2.out

SRCS := $(shell find $(SRC_DIR) -name "*.cpp")
OBJS := $(patsubst $(SRC_DIR)/%.cpp,$(OBJ_DIR)/%.o,$(SRCS))
DEPS := $(OBJS:.o=.d)

.PHONY: all clean run sanitize

all: $(TARGET)

# Liga todos os .o no executavel final
$(TARGET): $(OBJS)
	@mkdir -p $(BIN_DIR)
	$(CXX) $(CXXFLAGS) $(OBJS) -o $@

# Compila cada .cpp de src em um .o dentro de obj
# -MMD -MP gera arquivos .d para recompilar quando um .h mudar
$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp
	@mkdir -p $(dir $@)
	$(CXX) $(CXXFLAGS) -I$(INC_DIR) -MMD -MP -c $< -o $@

# Build de depuracao (so para voce): make sanitize
# Usa pasta de objetos propria para nao misturar com o build normal
# ALTERADO: referências a tp1_san.out para tp2_san.out
sanitize:
	$(MAKE) OBJ_DIR=$(OBJ_DIR)/san TARGET=$(BIN_DIR)/tp2_san.out \
		CXXFLAGS="-std=c++11 -Wall -Wextra -g -O1 -fno-omit-frame-pointer -fsanitize=address,undefined" \
		$(BIN_DIR)/tp2_san.out


# Deixa bin e obj vazias (como exigido na entrega)
clean:
	rm -rf $(OBJ_DIR)/* $(BIN_DIR)/*

# ALTERADO: tp1.out para tp2.out
run:
	./bin/tp2.out

-include $(DEPS)