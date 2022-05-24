cmake_minimum_required(VERSION 3.13)

# add this options before PROJECT keyword
set(CMAKE_DISABLE_SOURCE_CHANGES ON)
set(CMAKE_DISABLE_IN_SOURCE_BUILD ON)

set(CMAKE_CXX_STANDARD 20)
# set(CMAKE_CXX_COMPILER c++)


project(sieve_exec_comparison)

set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

# This does not seem to be needed any longer
# set(OPENSSL_ROOT_DIR /opt/homebrew/Cellar/openssl@1.1/1.1.1o)

option(EC_BUILD_SIEVE "Option to build sieve demo programs." ON)
option(EC_USE_CONCURRENCPP "Option whether or not to use concurrencpp library" ON)
option(EC_USE_WG21_P2003 "Option whether or not to use std execution library" OFF)
option(EC_USE_KIRKSHOOP "Option whether or not to use kirkshoop std execution library" ON)
option(EC_USE_LIBUNIFEX "Option whether or not to use libunifex" ON)
option(EC_USE_TBB "Option whether or not to use TBB" ON)
option(EC_USE_LOCAL_TBB "Use TBB submodule (rather than system TBB)" ON)

if (CMAKE_COMPILER_IS_GNUCXX)
  set(EC_USE_CONCURRENCPP OFF)
#  set(EC_USE_WG21_P2003 OFF)
  message("")
  message("*** Cannot use concurrencpp with GNU!!  Disabling! ***")
  message("*** Cannot use wg21_p2003 with GNU!!  Disabling! ***")
  message("")
endif()


if(EC_USE_LOCAL_TBB)
option(TBB_TEST "" OFF)

if(EC_USE_LOCAL_TBB)

# Use as submodule
add_subdirectory(external/oneTBB)

else()
find_package(TBB REQUIRED)
endif()
endif()

find_package(Threads REQUIRED)

if (EC_USE_WG21_P2003)
add_library(wg21_p2300_std_execution INTERFACE)
target_include_directories(wg21_p2300_std_execution INTERFACE external/wg21_p2300_std_execution/include)
endif()

if (EC_USE_LIBUNIFEX)
# add_library(libunifex INTERFACE)
# target_include_directories(libunifex INTERFACE external/libunifex/include)
add_subdirectory(external/libunifex)
endif()

if (EC_USE_CONCURRENCPP)
add_subdirectory(external/concurrencpp)
endif()

if (EC_BUILD_SIEVE)
add_subdirectory(sieve)
endif()

if (EC_USE_KIRKSHOOP) # a version of WG21_P2300
add_library(kirkshoop INTERFACE)
target_include_directories(kirkshoop INTERFACE external/kirkshoop/include)
endif()
