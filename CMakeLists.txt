cmake_minimum_required(VERSION 3.19)
project(CompileTimeAlternatives)

set(CMAKE_CXX_STANDARD 20)

add_library(CompileTimeAlternatives include/Boxes.h Union.h Tests/BoxTests.cpp Box.h internal/BelongsTo.h ValueInSet.h BoxValueInSet.h BoxValueInSetMaker.h internal/OverloadStateful.h ValueInRange.h BoxValueInRange.h BoxValueInRangeMaker.h FlatBox.h)

add_subdirectory(Tests)
