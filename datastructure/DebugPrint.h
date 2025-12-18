#pragma once
#include <iostream>

class DebugPrint final {
   public:
    DebugPrint () = delete;

    template <class... Args>
    static void printLine (Args... args) {
        ([] (const auto& x) { std::cout << x; }(args), ...);
        std::cout << std::endl;
    }
};