cmake_minimum_required (VERSION 2.8.12)

project (appveyor_test)

foreach(p
    CMP0054 # CMake 3.1
    CMP0056 # export EXE_LINKER_FLAGS to try_run
    )
  if(POLICY ${p})
    cmake_policy(SET ${p} NEW)
  endif()
endforeach()

# Make sure we can import out CMake functions
list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")

# Import our CMake modules
include(CheckCXXCompilerFlag)
include(AddCXXCompilerFlag)


if ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "MSVC")
  # Turn compiler warnings up to 11
  string(REGEX REPLACE "[-/]W[1-4]" "" CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}")
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /W4")
  add_definitions(-D_CRT_SECURE_NO_WARNINGS)

else()
  # Try and enable C++11. Don't use C++14 because it doesn't work in some
  # configurations.
  add_cxx_compiler_flag(-std=c++11)
  if (NOT HAVE_CXX_FLAG_STD_CXX11)
    add_cxx_compiler_flag(-std=c++0x)
  endif()

  # Turn compiler warnings up to 11
  add_cxx_compiler_flag(-Wall)

  add_cxx_compiler_flag(-Wextra)
  add_cxx_compiler_flag(-Wshadow)
  add_cxx_compiler_flag(-Werror RELEASE)
  add_cxx_compiler_flag(-Werror RELWITHDEBINFO)
  add_cxx_compiler_flag(-Werror MINSIZEREL)
  add_cxx_compiler_flag(-pedantic)
  add_cxx_compiler_flag(-pedantic-errors)
  add_cxx_compiler_flag(-Wshorten-64-to-32)
  add_cxx_compiler_flag(-Wfloat-equal)
  add_cxx_compiler_flag(-fstrict-aliasing)
  if (HAVE_CXX_FLAG_FSTRICT_ALIASING)
    if (NOT CMAKE_CXX_COMPILER_ID STREQUAL "Intel") #ICC17u2: Many false positives for Wstrict-aliasing
      add_cxx_compiler_flag(-Wstrict-aliasing)
    endif()
  endif()
  # ICC17u2: overloaded virtual function "benchmark::Fixture::SetUp" is only partially overridden
  # (because of deprecated overload)
  add_cxx_compiler_flag(-wd654)

  # On most UNIX like platforms g++ and clang++ define _GNU_SOURCE as a
  # predefined macro, which turns on all of the wonderful libc extensions.
  # However g++ doesn't do this in Cygwin so we have to define it ourselfs
  # since we depend on GNU/POSIX/BSD extensions.
  if (CYGWIN)
    add_definitions(-D_GNU_SOURCE=1)
  endif()
endif()



# Ensure we have pthreads
find_package(Threads REQUIRED)

add_executable(test_main test.cpp)

add_custom_target(run_test_main ALL
  COMMAND test_main
  WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
  COMMENT "run generated test in ${CMAKE_CURRENT_SOURCE_DIR}"
  SOURCES test.cpp
)

