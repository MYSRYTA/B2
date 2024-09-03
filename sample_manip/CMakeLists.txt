cmake_minimum_required(VERSION 3.13)

# include the project setting file
include($ENV{DEVKIT_LOCATION}/cmake/pluginEntry.cmake)

# specify project name
set(PROJECT_NAME sample_manip)

# set SOURCE_FILES
set(SOURCE_FILES
   sample_manip.cpp
)

# set linking libraries
set(LIBRARIES
     OpenMaya
     OpenMayaUI
     Foundation
)

# Build plugin
build_plugin()

