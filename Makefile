# Makefile for audiobackend.dll and audiobackend_examples.exe
# Use with nmake from x64 Native Tools Command Prompt for VS

# Compiler and linker settings
CC = cl
LINK = link

# Compiler flags
CFLAGS = /EHsc /O2 /W4 /nologo
DLL_CFLAGS = $(CFLAGS) /D_USRDLL /D_WINDLL

# Linker flags
LDFLAGS = /MACHINE:X64 /nologo
DLL_LDFLAGS = $(LDFLAGS) /DLL

# Directories
SRC_DIR = src
EXAMPLES_DIR = examples
BIN_DIR = bin

# Output files
DLL_TARGET = $(BIN_DIR)\audiobackend.dll
EXAMPLES_TARGET = $(BIN_DIR)\audiobackend_examples.exe

# Source files (automatically find all .cpp files)
DLL_SOURCES = $(SRC_DIR)\main.cpp $(SRC_DIR)\version.cpp $(SRC_DIR)\context.cpp $(SRC_DIR)\core.cpp $(SRC_DIR)\bgm.cpp $(SRC_DIR)\working_thread.cpp
EXAMPLES_SOURCES = $(EXAMPLES_DIR)\main.cpp

# Object files
DLL_OBJECTS = $(BIN_DIR)\main_dll.obj $(BIN_DIR)\version.obj $(BIN_DIR)\context.obj $(BIN_DIR)\core.obj $(BIN_DIR)\bgm.obj $(BIN_DIR)\working_thread.obj
EXAMPLES_OBJECTS = $(BIN_DIR)\main_examples.obj

# Default target - build both
all: $(DLL_TARGET) $(EXAMPLES_TARGET)

# Create bin directory if it doesn't exist
$(BIN_DIR):
	@if not exist "$(BIN_DIR)" mkdir "$(BIN_DIR)"

# Build the DLL
$(DLL_TARGET): $(BIN_DIR) $(DLL_OBJECTS)
	@echo Linking audiobackend.dll...
	$(LINK) $(DLL_LDFLAGS) /OUT:$(DLL_TARGET) $(DLL_OBJECTS) lib\fmod_vc.lib
	@echo Build successful! DLL created at $(DLL_TARGET)

# Build DLL object files
$(BIN_DIR)\main_dll.obj: $(SRC_DIR)\main.cpp
	@echo Compiling $(SRC_DIR)\main.cpp...
	$(CC) $(DLL_CFLAGS) /c $(SRC_DIR)\main.cpp /Fo:$(BIN_DIR)\main_dll.obj

$(BIN_DIR)\version.obj: $(SRC_DIR)\version.cpp $(SRC_DIR)\version.h
	@echo Compiling $(SRC_DIR)\version.cpp...
	$(CC) $(DLL_CFLAGS) /c $(SRC_DIR)\version.cpp /Fo:$(BIN_DIR)\version.obj

$(BIN_DIR)\context.obj: $(SRC_DIR)\context.cpp $(SRC_DIR)\context.h
	@echo Compiling $(SRC_DIR)\context.cpp...
	$(CC) $(DLL_CFLAGS) /c $(SRC_DIR)\context.cpp /Fo:$(BIN_DIR)\context.obj

$(BIN_DIR)\core.obj: $(SRC_DIR)\core.cpp $(SRC_DIR)\context.h
	@echo Compiling $(SRC_DIR)\core.cpp...
	$(CC) $(DLL_CFLAGS) /c $(SRC_DIR)\core.cpp /Fo:$(BIN_DIR)\core.obj

$(BIN_DIR)\bgm.obj: $(SRC_DIR)\bgm.cpp $(SRC_DIR)\context.h
	@echo Compiling $(SRC_DIR)\bgm.cpp...
	$(CC) $(DLL_CFLAGS) /c $(SRC_DIR)\bgm.cpp /Fo:$(BIN_DIR)\bgm.obj

$(BIN_DIR)\working_thread.obj: $(SRC_DIR)\working_thread.cpp $(SRC_DIR)\working_thread.h $(SRC_DIR)\context.h
	@echo Compiling $(SRC_DIR)\working_thread.cpp...
	$(CC) $(DLL_CFLAGS) /c $(SRC_DIR)\working_thread.cpp /Fo:$(BIN_DIR)\working_thread.obj

# Build the examples executable
$(EXAMPLES_TARGET): $(BIN_DIR) $(EXAMPLES_OBJECTS) $(DLL_TARGET)
	@echo Linking audiobackend_examples.exe...
	$(LINK) $(LDFLAGS) /OUT:$(EXAMPLES_TARGET) $(EXAMPLES_OBJECTS) $(BIN_DIR)\audiobackend.lib
	@echo Build successful! Executable created at $(EXAMPLES_TARGET)

# Build examples object files
$(BIN_DIR)\main_examples.obj: $(EXAMPLES_DIR)\main.cpp
	@echo Compiling $(EXAMPLES_DIR)\main.cpp...
	$(CC) $(CFLAGS) /c $(EXAMPLES_DIR)\main.cpp /Fo:$(BIN_DIR)\main_examples.obj

# Clean build artifacts
clean:
	@echo Cleaning build artifacts...
	@if exist "$(BIN_DIR)\*.obj" del /Q "$(BIN_DIR)\*.obj"
	@if exist "$(BIN_DIR)\*.exp" del /Q "$(BIN_DIR)\*.exp"
	@if exist "$(BIN_DIR)\*.lib" del /Q "$(BIN_DIR)\*.lib"
	@if exist "$(DLL_TARGET)" del /Q "$(DLL_TARGET)"
	@if exist "$(EXAMPLES_TARGET)" del /Q "$(EXAMPLES_TARGET)"
	@echo Clean complete.

# Build only the DLL
dll: $(DLL_TARGET)

# Build only the examples
examples: $(EXAMPLES_TARGET)

# Help target
help:
	@echo Available targets:
	@echo   all       - Build both DLL and examples (default)
	@echo   dll       - Build only audiobackend.dll
	@echo   examples  - Build only audiobackend_examples.exe
	@echo   clean     - Remove all build artifacts
	@echo   help      - Show this help message

.PHONY: all clean dll examples help
