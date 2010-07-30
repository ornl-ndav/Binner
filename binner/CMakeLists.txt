cmake_minimum_required(VERSION 2.6)

# 
# $Id$
#
# cmake allows out of place building
# you can do a "mkdir build; cd build" under any directory
#
# then run command as:
#     cmake PATH-TO-BINNER-TREE -DNOGL=yes -DBINNER_INSTALL_DIR=/usr/local
#
# if not BINNER_INSTALL_DIR is defined, the
#     BINNER_INSTALL_DIR is defaulted as PATH-TO-BINNER-TREE
#
# installation goes under:
#     ${BINNER_INSTALL_DIR}/bin, and
#     ${BINNER_INSTALL_DIR}/lib
#
# if NOGL=yes is defined:
#     then don't build any components that require OpenGL and GLUT
#

# BINNER version number in popular convention
set(BINNER_MAJOR_VERSION 1)
set(BINNER_MINOR_VERSION 1)
set(BINNER_BUILD_VERSION 0)
set(BINNER_VERSION
    "${BINNER_MAJOR_VERSION}.${BINNER_MINOR_VERSION}.${BINNER_BUILD_VERSION}")

if (BINNER_INSTALL_DIR)
   set (BINNER_INSTALL_DIR 
        ${BINNER_INSTALL_DIR}/binner/${BINNER_VERSION})
endif (BINNER_INSTALL_DIR)

add_subdirectory(vcb)

project(binner)
add_subdirectory(src)

add_custom_target(manual ALL
                  DEPENDS ${PROJECT_SOURCE_DIR}/docs/Doxyfile
                  COMMAND doxygen
                  WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}/docs)
