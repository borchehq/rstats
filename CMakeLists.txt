cmake_minimum_required(VERSION 3.0)
project(librstats VERSION 0.1 LANGUAGES C)

# Must use GNUInstallDirs to install libraries into correct
# locations on all platforms.
include(GNUInstallDirs)

# Targets that we develop here
enable_testing()
add_subdirectory(src)
