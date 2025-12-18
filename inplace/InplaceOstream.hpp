//
// Created by Akhilesh Mishra on 07/01/2025.
//

#ifndef INPLACEOSTREAM_HPP
#define INPLACEOSTREAM_HPP
#include <iostream>

namespace inplace{
    template <bool>
    class InplaceOstream{
        public:
        template<class... Args>
        static void print(Args&&... args){
            (std::cout << ... << args) << std::endl;
        }
        static void print() {
            std::cout << std::endl;
        }
    };
};

#endif //INPLACEOSTREAM_HPP
