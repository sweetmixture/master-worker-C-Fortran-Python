#
cmake_minimum_required(VERSION 3.11)

# Project Info - very sensitive to Syntax!!
project(
	BaseTaskFarm
	VERSION 0.1
	LANGUAGES C Fortran)

# set( [ARG] [VAR1] [VAR2] ... )

find_package(MPI REQUIRED)
# MPI_LIBRARIES
# MPI_INCLUDE_PATH -> these variables will be set
include_directories(${MPI_INCLUDE_PATH})

set(CMAKE_C_COMPILER icc)
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -std=c11 -O3")
set(CMAKE_Fortran_COMPILER ifort)

# Get C compiler version
execute_process(COMMAND ${CMAKE_C_COMPILER} --version OUTPUT_VARIABLE C_COMPILER_VERSION)
string(REGEX MATCH "[0-9]+\\.[0-9]+\\.[0-9]+" C_COMPILER_VERSION "${C_COMPILER_VERSION}")
message(STATUS "C Compiler Version: ${C_COMPILER_VERSION}")


# EDIT BLOCK --------------------------------------------------

set(SRC main.c taskfarm_def.c master_worker_task.c)
set(SRC_DEV ${CMAKE_SOURCE_DIR}/develop/unit_test.c)
set(SRC_SUBPROGRAM ${CMAKE_SOURCE_DIR}/subprogram/subprogram_pi.c)
set(SRC_FORTRAN ${CMAKE_SOURCE_DIR}/subprogram/fsubroutine_pi.f90)

set(EXE tf.x)

# header file path: include_directories( [EXECUTABLE] PUBLIC [PATH1] [PATH2] ... ) 
#include_directories(${EXE} PUBLIC ${CMAKE_SOURCE_DIR}/includes ${CMAKE_SOURCE_DIR}/develop/includes ${CMAKE_SOURCE_DIR}/subprogram/includes)

include_directories(${EXE} PUBLIC ${CMAKE_SOURCE_DIR}/includes)
include_directories(${EXE} PUBLIC ${CMAKE_SOURCE_DIR}/develop/includes)
include_directories(${EXE} PUBLIC ${CMAKE_SOURCE_DIR}/subprogram/includes)

add_executable(${EXE} ${SRC} ${SRC_DEV} ${SRC_SUBPROGRAM} ${SRC_FORTRAN})
#add_executable(${EXE} ${SRC} ${SRC_DEV} ${SRC_FORTRAN})
target_link_libraries(${EXE} ${MPI_LIBRARIES} MPI::MPI_Fortran)

set_target_properties(${EXE} PROPERTIES LINKER_LANGUAGE C)
# -------------------------------------------------------------

# Print MPI library
find_package(MPI REQUIRED)
message(STATUS "MPI library: ${MPI_LIBRARIES}")
message(STATUS "MPI include: ${MPI_INCLUDE_PATH}")

# Print C compiler
message(STATUS "C Compiler: ${CMAKE_C_COMPILER}")
