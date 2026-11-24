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
# ---------------------------------------------------------------------
# Experimentos (branch experimental; nao faz parte da entrega do VPL)
#   make experimentos      -> compila bin/bench.out
#   make rodar-experimentos -> roda tudo e gera CSVs + graficos
# ---------------------------------------------------------------------
EXP_DIR    := experimentos
EXP_TARGET := $(BIN_DIR)/bench.out
LIB_OBJS   := $(filter-out $(OBJ_DIR)/main.o,$(OBJS))

.PHONY: experimentos rodar-experimentos

experimentos: $(EXP_TARGET)

$(EXP_TARGET): $(LIB_OBJS) $(OBJ_DIR)/bench.o
	@mkdir -p $(BIN_DIR)
	$(CXX) $(CXXFLAGS) $^ -o $@

$(OBJ_DIR)/bench.o: $(EXP_DIR)/bench.cpp
	@mkdir -p $(OBJ_DIR)
	$(CXX) $(CXXFLAGS) -I$(INC_DIR) -MMD -MP -c $< -o $@

rodar-experimentos: $(EXP_TARGET)
	@mkdir -p $(EXP_DIR)/resultados $(EXP_DIR)/graficos
	./$(EXP_TARGET) todos $(EXP_DIR)/resultados 3
	PYTHONPATH=$(EXP_DIR)/.pylibs python3 $(EXP_DIR)/graficos.py
