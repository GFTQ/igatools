#-------------------------------------------------------------------------------
# Igatools a general purpose Isogeometric analysis library.
# Copyright (C) 2012,2013  by the igatools authors (see authors.txt).
# 
# Distribution of this software is not allowed.
# Only personal use by authorized people in the environment of
# the IMATI institute is allowed.
#   
# When the software is reasonably stable it will be released under an 
# Open Source license (GPLv3) as follows:
# 
# This program is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
# 
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
# 
# You should have received a copy of the GNU General Public License
# along with this program.  If not, see <http://www.gnu.org/licenses/>.
#-------------------------------------------------------------------------------

# Ensures that we do an out of source build
macro( MACRO_ENSURE_OUT_OF_SOURCE_BUILD )
    string( COMPARE EQUAL "${CMAKE_SOURCE_DIR}" "${CMAKE_BINARY_DIR}" insource )
    get_filename_component( PARENTDIR ${CMAKE_SOURCE_DIR} PATH )
    string( COMPARE EQUAL "${CMAKE_SOURCE_DIR}" "${PARENTDIR}" insourcesubdir )
    if( insource OR insourcesubdir )
        MESSAGE("The build has to be different from the source directory" )
        message(FATAL_ERROR "you will have to run something like:"
        "\n rm -rf CMakeFiles and rm CMakeCache.txt")
    endif( insource OR insourcesubdir )
endmacro(MACRO_ENSURE_OUT_OF_SOURCE_BUILD)

