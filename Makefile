# build environment
BUILD_PATH = build
OBJECTS_PATH = objects
EXECUTABLE_PATH = $(BUILD_PATH)/hook

# install environment
PREFIX = /usr/local
BINDIR = $(PREFIX)/bin

# source files
SOURCES = $(wildcard src/includes/*.c src/*.c)

# object files
OBJECTS = $(addprefix $(OBJECTS_PATH)/, $(notdir $(SOURCES:.c=.o)))

# Compiler and flags
CC = clang
CPPFLAGS = -I src/includes --std=c23
CFLAGS   = -Wall -Wextra -Werror -pedantic
CFLAGS  += -Wno-unused-parameter -Wno-unused-variable -Wno-unused-function
CFLAGS  += -Wno-unused-value -Wno-unused-but-set-variable
CFLAGS  += -Wno-unused-but-set-parameter -Wno-unused-local-typedefs
LDFLAGS  = -lncurses

# Default target
all: $(EXECUTABLE_PATH)
	@echo "All tasks completed."

# Cleaning rule
clean:
	@echo "Cleaning..."
	@rm -rf $(BUILD_PATH) $(OBJECTS_PATH)

# Linking the executable
$(EXECUTABLE_PATH): $(OBJECTS) | $(BUILD_PATH)
	@echo "Building $(EXECUTABLE_PATH)..."
	$(CC) $(OBJECTS) $(LDFLAGS) -o $@

# Rule for compiling object files from C source files
$(OBJECTS_PATH)/%.o: src/%.c | $(OBJECTS_PATH)
	@echo "Compiling $< to $@..."
	$(CC) $(CPPFLAGS) $(CFLAGS) -c $< -o $@

$(OBJECTS_PATH)/%.o: src/includes/%.c | $(OBJECTS_PATH)
	@echo "Compiling $< to $@..."
	$(CC) $(CPPFLAGS) $(CFLAGS) -c $< -o $@

# Create directories if needed
$(OBJECTS_PATH):
	@mkdir -p $(OBJECTS_PATH)

$(BUILD_PATH):
	@mkdir -p $(BUILD_PATH)

test: clean $(EXECUTABLE_PATH)
	@echo "Running tests on $(EXECUTABLE_PATH)..."
	$(EXECUTABLE_PATH) --interval 1 ls -la /Users/franciscodelahoz

install: $(EXECUTABLE_PATH)
	@echo "Installing $(EXECUTABLE_PATH) to $(BINDIR)..."
	@mkdir -p $(BINDIR)
	@install -m 755 $(EXECUTABLE_PATH) $(BINDIR)

uninstall:
	@echo "Uninstalling $(EXECUTABLE_PATH) from $(BINDIR)..."
	@rm -f $(BINDIR)/hook

.PHONY: all clean install uninstall test
