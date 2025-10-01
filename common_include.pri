# Common include configuration for QlipMon project
# This file sets up shared include paths and dependencies

# Get the absolute path to the project root
PROJECT_ROOT = $$PWD

# Add common include directory to include path
INCLUDEPATH += $$PROJECT_ROOT/include

# Define common headers directory
COMMON_HEADERS_DIR = $$PROJECT_ROOT/include

# Message for debugging
!build_pass:message("Common include path: $$COMMON_HEADERS_DIR")