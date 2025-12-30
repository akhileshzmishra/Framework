#include <MyAny.h>
#include <MyIntegerIndexer.h>
#include <MyTuplePacker.h>
#include <TrieMap.h>
#include <gtest/gtest.h>

#include <iostream>
#include <string>

#include "DebugPrint.h"

using namespace datastructure;
std::unordered_map<void*, void*> d_memoryMap{};

template <class Tp>
class CustomAllocator : std::allocator<Tp> {

   public:
    typedef size_t size_type;
    typedef ptrdiff_t difference_type;
    typedef Tp value_type;

    template <class _Other>
    struct rebind {
        typedef CustomAllocator<_Other> other;
    };

    CustomAllocator () noexcept = default;

    template <class _Up>
    CustomAllocator (const CustomAllocator<_Up>&) noexcept {}

    static Tp* allocate (size_t n) {
        auto value = std::aligned_alloc (alignof (Tp), n * sizeof (Tp));
        d_memoryMap.insert ({value, value});
        DebugPrint::printLine ("CustomAllocator::allocate -> ", value);

        return static_cast<Tp*> (value);
    }

    static void deallocate (Tp* p, size_t) noexcept {
        DebugPrint::printLine ("CustomAllocator::deallocate -> ", p);
        assert (d_memoryMap.contains (static_cast<void*> (p)));
        std::free (p);
        d_memoryMap.erase (p);
    }
};

TEST (TrieMapTest, TrieMap_set_valid) {
    TrieMap<char, std::string> trieMap;
    trieMap.set ("Akhilesh", "akhilesh");
    EXPECT_TRUE (trieMap.get ("Akhilesh").has_value ());
    EXPECT_EQ (trieMap.get ("Akhilesh").value ().get (), std::string ("akhilesh"));
}

TEST (TrieMapTest, TrieMap_test_invalid) {
    TrieMap<char, std::string> trieMap;
    trieMap.set ("Akhilesh", "akhilesh");
    EXPECT_FALSE (trieMap.get ("Akhilesh1").has_value ());
}

TEST (TrieMapTest, generalTest) {
    TrieMap<char, std::string> trieMap;
    std::vector<std::string> keys{
        "Akhilesh",      "akhilesh", "test1", "test2",  "test3",         "test4",
        "1test",         "2test",    "3test", "4test",  "test_1_test_2", "test_3_test_4",
        "test_4_test_5", "abcdefgh", "bcdef", "cdefgh", "china"};

    for (auto key : keys) {
        trieMap.set (key, key);
    }

    for (auto key : keys) {
        EXPECT_EQ (trieMap.get (key).has_value (), true);
        EXPECT_EQ (trieMap.get (key).value ().get (), key);
    }

    for (auto key : keys) {
        EXPECT_EQ (trieMap.get (key + "1").has_value (), false);
    }
}

TEST (TrieMapTest, testAllocator) {
    {
        TrieMap<char, std::string, CustomAllocator<char>> trieMap;
        std::vector<std::string> keys{
            "Akhilesh",      "akhilesh", "test1", "test2",  "test3",         "test4",
            "1test",         "2test",    "3test", "4test",  "test_1_test_2", "test_3_test_4",
            "test_4_test_5", "abcdefgh", "bcdef", "cdefgh", "china"};

        for (auto key : keys) {
            trieMap.set (key, key);
        }

        for (auto key : keys) {
            EXPECT_EQ (trieMap.get (key).has_value (), true);
            EXPECT_EQ (trieMap.get (key).value ().get (), key);
        }

        for (auto key : keys) {
            EXPECT_EQ (trieMap.get (key + "1").has_value (), false);
        }
    }

    EXPECT_EQ (d_memoryMap.size (), 0);
}