
# WebServer Makefile - Final Working Version
# -----------------------------------------

# Compiler settings
CXX      := c++
CXXFLAGS :=  -g -O0 -Wall -Wextra -Werror -std=c++98
INCLUDES := -Iheaders

# Project name
NAME     := webserv

# Directory structure
SRC_DIR  := srcs
VAL_DIR  := $(SRC_DIR)/validation
REQ_DIR  := $(SRC_DIR)/request
RESP_DIR := $(SRC_DIR)/response
SERV_DIR := $(SRC_DIR)/server
CGI_DIR := $(SRC_DIR)/CGI
OBJ_DIR  := obj

# Source files (verify these paths match your actual files)
MAIN_SRC := webserv.cpp
CLASS_SRCS := $(wildcard $(VAL_DIR)/*.cpp) \
             $(wildcard $(REQ_DIR)/*.cpp) \
             $(wildcard $(RESP_DIR)/*.cpp) \
			$(wildcard) $(SERV_DIR)/*.cpp \
             $(wildcard $(CGI_DIR)/*.cpp) \

# Object files
MAIN_OBJ  := $(OBJ_DIR)/webserv.o
VAL_OBJS  := $(patsubst $(VAL_DIR)/%.cpp,$(OBJ_DIR)/%.o,$(wildcard $(VAL_DIR)/*.cpp))
REQ_OBJS  := $(patsubst $(REQ_DIR)/%.cpp,$(OBJ_DIR)/%.o,$(wildcard $(REQ_DIR)/*.cpp))
RESP_OBJS := $(patsubst $(RESP_DIR)/%.cpp,$(OBJ_DIR)/%.o,$(wildcard $(RESP_DIR)/*.cpp))
SERV_OBJS := $(patsubst $(SERV_DIR)/%.cpp,$(OBJ_DIR)/%.o,$(wildcard $(SERV_DIR)/*.cpp))
CGI_OBJS := $(patsubst $(CGI_DIR)/%.cpp,$(OBJ_DIR)/%.o,$(wildcard $(CGI_DIR)/*.cpp))

OBJS     := $(MAIN_OBJ) $(VAL_OBJS) $(REQ_OBJS) $(RESP_OBJS) $(SERV_OBJS) $(CGI_OBJS)

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

$(OBJ_DIR)/%.o: $(REQ_DIR)/%.cpp | $(OBJ_DIR)
	@echo "🔨 Compiling $<..."
	@$(CXX) $(CXXFLAGS) $(INCLUDES) -c $< -o $@

$(OBJ_DIR)/%.o: $(RESP_DIR)/%.cpp | $(OBJ_DIR)
	@echo "🔨 Compiling $<..."
	@$(CXX) $(CXXFLAGS) $(INCLUDES) -c $< -o $@

$(OBJ_DIR)/%.o: $(SERV_DIR)/%.cpp | $(OBJ_DIR)
	@echo "🔨 Compiling $<..."
	@$(CXX) $(CXXFLAGS) $(INCLUDES) -c $< -o $@

$(OBJ_DIR)/%.o: $(CGI_DIR)/%.cpp | $(OBJ_DIR)
	@echo "🔨 Compiling $<..."
	@$(CXX) $(CXXFLAGS) $(INCLUDES) -c $< -o $@

# Special rule for compiling webserv.cpp
$(OBJ_DIR)/webserv.o: webserv.cpp | $(OBJ_DIR)
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
