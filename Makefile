# WebServer Makefile - Final Working Version
# -----------------------------------------

# Compiler settings
CXX      := c++
CXXFLAGS := -Wall -Wextra -Werror -std=c++98
INCLUDES := -Iheaders

# Project name
NAME     := webserv

# Directory structure
SRC_DIR  := srcs
VAL_DIR  := $(SRC_DIR)/validation
OBJ_DIR  := obj

# Source files (verify these paths match your actual files)
MAIN_SRC := webserv.cpp
CLASS_SRCS := $(wildcard $(VAL_DIR)/*.cpp)

# Object files
OBJS     := $(patsubst $(SRC_DIR)/%.cpp,$(OBJ_DIR)/%.o,$(MAIN_SRC)) \
            $(patsubst $(VAL_DIR)/%.cpp,$(OBJ_DIR)/%.o,$(CLASS_SRCS))

# Main build rule
all: $(NAME)

$(NAME): $(OBJS)
	@echo "🔗 Linking $(NAME)..."
	@$(CXX) $(CXXFLAGS) $(OBJS) -o $@
	@echo "✅ Build successful!"

# Compilation rules
$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp | $(OBJ_DIR)
	@echo "🔨 Compiling $<..."
	@$(CXX) $(CXXFLAGS) $(INCLUDES) -c $< -o $@

$(OBJ_DIR)/%.o: $(VAL_DIR)/%.cpp | $(OBJ_DIR)
	@echo "🔨 Compiling $<..."
	@$(CXX) $(CXXFLAGS) $(INCLUDES) -c $< -o $@

# Create obj directory
$(OBJ_DIR):
	@mkdir -p $@

# Clean rules
clean:
	@echo "🧹 Cleaning object files..."
	@rm -rf $(OBJ_DIR)

fclean: clean
	@echo "🗑️ Removing executable..."
	@rm -f $(NAME)

re: fclean all

.PHONY: all clean fclean re