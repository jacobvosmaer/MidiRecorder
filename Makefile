# Project Name
TARGET = MidiRecorder

# Sources
CPP_SOURCES = MidiRecorder.cpp

# Library Locations
LIBDAISY_DIR = ./libDaisy

USE_FATFS = 1

# Core location, and generic Makefile.
SYSTEM_FILES_DIR = $(LIBDAISY_DIR)/core
include $(SYSTEM_FILES_DIR)/Makefile

format: *.cpp
	clang-format -i *.cpp
