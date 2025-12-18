add_library(
    generalDS
    GeneralDS.cpp
    MyFunction.h
    Mytuple.h
    MyIntegerIndexer.h
    MyTuplePacker.h
    MyAny.h
    AnyStorage.h
    TrieMap.h
    ZipView.h
    DebugPrint.h
)


target_include_directories(generalDS PUBLIC .)