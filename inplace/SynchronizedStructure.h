

#ifndef SYNCHRONIZEDSTRUCTURE_H
#define SYNCHRONIZEDSTRUCTURE_H

#include <unordered_map>

namespace SynchronizedStructure {

template <class Key, class Value, class...Extra>
class Structure{
    public:

};

template <class Key, class Data, class Container = std::unordered_map<Key, Data>>
class SynchronizedStructure {
    Container d_container;
    public:
};


}

#endif //SYNCHRONIZEDSTRUCTURE_H
