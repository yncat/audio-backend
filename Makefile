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
DLL_SOURCES = $(SRC_DIR)\main.cpp $(SRC_DIR)\version.cpp $(SRC_DIR)\context.cpp $(SRC_DIR)\core.cpp $(SRC_DIR)\bgm.cpp $(SRC_DIR)\working_thread.cpp $(SRC_DIR)\sample.cpp $(SRC_DIR)\vr.cpp $(SRC_DIR)\vrobj.cpp $(SRC_DIR)\vrplayer.cpp $(SRC_DIR)\vrroom.cpp $(SRC_DIR)\adapter_resonance.cpp $(SRC_DIR)\plugin_inspector.cpp
EXAMPLES_SOURCES = $(EXAMPLES_DIR)\main.cpp $(EXAMPLES_DIR)\helper.cpp $(EXAMPLES_DIR)\test_core_init_free.cpp $(EXAMPLES_DIR)\test_bgm_functions.cpp $(EXAMPLES_DIR)\test_loop_point.cpp $(EXAMPLES_DIR)\test_sample_oneshot.cpp $(EXAMPLES_DIR)\test_vr_initialization.cpp $(EXAMPLES_DIR)\test_plugin_inspector.cpp $(EXAMPLES_DIR)\test_3d_oneshot.cpp $(EXAMPLES_DIR)\test_vr_player_position.cpp $(EXAMPLES_DIR)\test_vr_room_effects.cpp $(EXAMPLES_DIR)\test_vr_object.cpp

# Object files
DLL_OBJECTS = $(BIN_DIR)\main_dll.obj $(BIN_DIR)\version.obj $(BIN_DIR)\context.obj $(BIN_DIR)\core.obj $(BIN_DIR)\bgm.obj $(BIN_DIR)\working_thread.obj $(BIN_DIR)\sample.obj $(BIN_DIR)\vr.obj $(BIN_DIR)\vrobj.obj $(BIN_DIR)\vrplayer.obj $(BIN_DIR)\vrroom.obj $(BIN_DIR)\adapter_resonance.obj $(BIN_DIR)\plugin_inspector.obj
EXAMPLES_OBJECTS = $(BIN_DIR)\main_examples.obj $(BIN_DIR)\helper.obj $(BIN_DIR)\test_core_init_free.obj $(BIN_DIR)\test_bgm_functions.obj $(BIN_DIR)\test_loop_point.obj $(BIN_DIR)\test_sample_oneshot.obj $(BIN_DIR)\test_vr_initialization.obj $(BIN_DIR)\test_plugin_inspector.obj $(BIN_DIR)\test_3d_oneshot.obj $(BIN_DIR)\test_vr_player_position.obj $(BIN_DIR)\test_vr_room_effects.obj $(BIN_DIR)\test_vr_object.obj

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

$(BIN_DIR)\sample.obj: $(SRC_DIR)\sample.cpp $(SRC_DIR)\sample.h $(SRC_DIR)\context.h
	@echo Compiling $(SRC_DIR)\sample.cpp...
	$(CC) $(DLL_CFLAGS) /c $(SRC_DIR)\sample.cpp /Fo:$(BIN_DIR)\sample.obj

$(BIN_DIR)\vr.obj: $(SRC_DIR)\vr.cpp $(SRC_DIR)\vr.h $(SRC_DIR)\context.h
	@echo Compiling $(SRC_DIR)\vr.cpp...
	$(CC) $(DLL_CFLAGS) /c $(SRC_DIR)\vr.cpp /Fo:$(BIN_DIR)\vr.obj

$(BIN_DIR)\vrobj.obj: $(SRC_DIR)\vrobj.cpp $(SRC_DIR)\vrobj.h $(SRC_DIR)\vrstructs.h $(SRC_DIR)\context.h
	@echo Compiling $(SRC_DIR)\vrobj.cpp...
	$(CC) $(DLL_CFLAGS) /c $(SRC_DIR)\vrobj.cpp /Fo:$(BIN_DIR)\vrobj.obj

$(BIN_DIR)\vrplayer.obj: $(SRC_DIR)\vrplayer.cpp $(SRC_DIR)\vrplayer.h $(SRC_DIR)\vrstructs.h $(SRC_DIR)\context.h
	@echo Compiling $(SRC_DIR)\vrplayer.cpp...
	$(CC) $(DLL_CFLAGS) /c $(SRC_DIR)\vrplayer.cpp /Fo:$(BIN_DIR)\vrplayer.obj

$(BIN_DIR)\vrroom.obj: $(SRC_DIR)\vrroom.cpp $(SRC_DIR)\vrroom.h $(SRC_DIR)\vrstructs.h $(SRC_DIR)\context.h $(SRC_DIR)\adapter_resonance.h
	@echo Compiling $(SRC_DIR)\vrroom.cpp...
	$(CC) $(DLL_CFLAGS) /c $(SRC_DIR)\vrroom.cpp /Fo:$(BIN_DIR)\vrroom.obj

$(BIN_DIR)\adapter_resonance.obj: $(SRC_DIR)\adapter_resonance.cpp $(SRC_DIR)\adapter_resonance.h $(SRC_DIR)\vrstructs.h $(SRC_DIR)\resonance_room_properties.h
	@echo Compiling $(SRC_DIR)\adapter_resonance.cpp...
	$(CC) $(DLL_CFLAGS) /c $(SRC_DIR)\adapter_resonance.cpp /Fo:$(BIN_DIR)\adapter_resonance.obj

$(BIN_DIR)\plugin_inspector.obj: $(SRC_DIR)\plugin_inspector.cpp $(SRC_DIR)\plugin_inspector.h $(SRC_DIR)\context.h
	@echo Compiling $(SRC_DIR)\plugin_inspector.cpp...
	$(CC) $(DLL_CFLAGS) /c $(SRC_DIR)\plugin_inspector.cpp /Fo:$(BIN_DIR)\plugin_inspector.obj

# Build the examples executable
$(EXAMPLES_TARGET): $(BIN_DIR) $(EXAMPLES_OBJECTS) $(DLL_TARGET)
	@echo Linking audiobackend_examples.exe...
	$(LINK) $(LDFLAGS) /OUT:$(EXAMPLES_TARGET) $(EXAMPLES_OBJECTS) $(BIN_DIR)\audiobackend.lib
	@echo Build successful! Executable created at $(EXAMPLES_TARGET)

# Build examples object files
$(BIN_DIR)\main_examples.obj: $(EXAMPLES_DIR)\main.cpp $(EXAMPLES_DIR)\helper.h
	@echo Compiling $(EXAMPLES_DIR)\main.cpp...
	$(CC) $(CFLAGS) /c $(EXAMPLES_DIR)\main.cpp /Fo:$(BIN_DIR)\main_examples.obj

$(BIN_DIR)\helper.obj: $(EXAMPLES_DIR)\helper.cpp $(EXAMPLES_DIR)\helper.h
	@echo Compiling $(EXAMPLES_DIR)\helper.cpp...
	$(CC) $(CFLAGS) /c $(EXAMPLES_DIR)\helper.cpp /Fo:$(BIN_DIR)\helper.obj

$(BIN_DIR)\test_core_init_free.obj: $(EXAMPLES_DIR)\test_core_init_free.cpp
	@echo Compiling $(EXAMPLES_DIR)\test_core_init_free.cpp...
	$(CC) $(CFLAGS) /c $(EXAMPLES_DIR)\test_core_init_free.cpp /Fo:$(BIN_DIR)\test_core_init_free.obj

$(BIN_DIR)\test_bgm_functions.obj: $(EXAMPLES_DIR)\test_bgm_functions.cpp $(EXAMPLES_DIR)\helper.h
	@echo Compiling $(EXAMPLES_DIR)\test_bgm_functions.cpp...
	$(CC) $(CFLAGS) /c $(EXAMPLES_DIR)\test_bgm_functions.cpp /Fo:$(BIN_DIR)\test_bgm_functions.obj

$(BIN_DIR)\test_loop_point.obj: $(EXAMPLES_DIR)\test_loop_point.cpp $(EXAMPLES_DIR)\helper.h
	@echo Compiling $(EXAMPLES_DIR)\test_loop_point.cpp...
	$(CC) $(CFLAGS) /c $(EXAMPLES_DIR)\test_loop_point.cpp /Fo:$(BIN_DIR)\test_loop_point.obj

$(BIN_DIR)\test_sample_oneshot.obj: $(EXAMPLES_DIR)\test_sample_oneshot.cpp $(EXAMPLES_DIR)\helper.h
	@echo Compiling $(EXAMPLES_DIR)\test_sample_oneshot.cpp...
	$(CC) $(CFLAGS) /c $(EXAMPLES_DIR)\test_sample_oneshot.cpp /Fo:$(BIN_DIR)\test_sample_oneshot.obj

$(BIN_DIR)\test_vr_initialization.obj: $(EXAMPLES_DIR)\test_vr_initialization.cpp $(EXAMPLES_DIR)\helper.h
	@echo Compiling $(EXAMPLES_DIR)\test_vr_initialization.cpp...
	$(CC) $(CFLAGS) /c $(EXAMPLES_DIR)\test_vr_initialization.cpp /Fo:$(BIN_DIR)\test_vr_initialization.obj

$(BIN_DIR)\test_plugin_inspector.obj: $(EXAMPLES_DIR)\test_plugin_inspector.cpp $(EXAMPLES_DIR)\helper.h
	@echo Compiling $(EXAMPLES_DIR)\test_plugin_inspector.cpp...
	$(CC) $(CFLAGS) /c $(EXAMPLES_DIR)\test_plugin_inspector.cpp /Fo:$(BIN_DIR)\test_plugin_inspector.obj

$(BIN_DIR)\test_3d_oneshot.obj: $(EXAMPLES_DIR)\test_3d_oneshot.cpp $(EXAMPLES_DIR)\helper.h
	@echo Compiling $(EXAMPLES_DIR)\test_3d_oneshot.cpp...
	$(CC) $(CFLAGS) /c $(EXAMPLES_DIR)\test_3d_oneshot.cpp /Fo:$(BIN_DIR)\test_3d_oneshot.obj

$(BIN_DIR)\test_vr_player_position.obj: $(EXAMPLES_DIR)\test_vr_player_position.cpp $(EXAMPLES_DIR)\helper.h
	@echo Compiling $(EXAMPLES_DIR)\test_vr_player_position.cpp...
	$(CC) $(CFLAGS) /c $(EXAMPLES_DIR)\test_vr_player_position.cpp /Fo:$(BIN_DIR)\test_vr_player_position.obj

$(BIN_DIR)\test_vr_room_effects.obj: $(EXAMPLES_DIR)\test_vr_room_effects.cpp $(EXAMPLES_DIR)\helper.h
	@echo Compiling $(EXAMPLES_DIR)\test_vr_room_effects.cpp...
	$(CC) $(CFLAGS) /c $(EXAMPLES_DIR)\test_vr_room_effects.cpp /Fo:$(BIN_DIR)\test_vr_room_effects.obj

$(BIN_DIR)\test_vr_object.obj: $(EXAMPLES_DIR)\test_vr_object.cpp $(EXAMPLES_DIR)\helper.h
	@echo Compiling $(EXAMPLES_DIR)\test_vr_object.cpp...
	$(CC) $(CFLAGS) /c $(EXAMPLES_DIR)\test_vr_object.cpp /Fo:$(BIN_DIR)\test_vr_object.obj

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
