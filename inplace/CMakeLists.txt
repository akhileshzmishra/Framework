add_library(
    inplace
    InplaceLibrary.cpp
    MPMCQueue.hpp
        InplaceOstream.hpp
        FixedList.h
)


target_include_directories(inplace PUBLIC .)