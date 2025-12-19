

#ifndef TRIEMAP_H
#define TRIEMAP_H
#include <cassert>
#include <concepts>
#include <memory>
#include <unordered_map>
#include <vector>
namespace datastructure {

template <class T>
concept CanbeTrieKey =
    std::is_default_constructible_v<T> && std::equality_comparable<T> && requires (T t) {
        { std::hash<T>{}(t) } -> std::convertible_to<std::size_t>;
    };

template <class Node, class Key, class Value>
concept CanbeTrieNode = !std::is_move_constructible_v<Node> && !std::is_copy_assignable_v<Node> &&
                        std::is_default_constructible_v<Node> && requires (Node n, Key k, Value v) {
                            { n.set (k, std::shared_ptr<Node>{}) } -> std::same_as<void>;
                            { n.get (k) } -> std::same_as<std::shared_ptr<Node>>;
                            { n.isLeaf () } -> std::same_as<bool>;
                            { n.setLeaf (v) } -> std::same_as<void>;
                            { n.leafValue () } -> std::convertible_to<Value>;
                            { n.contains (k) } -> std::same_as<bool>;
                        };

template <CanbeTrieKey Key, typename Value, typename keyAllocator = std::allocator<Key>>
class TrieNode final {
    using ChildPtr = std::shared_ptr<TrieNode>;
    template <class T>
    using RebindAlloc = typename std::allocator_traits<keyAllocator>::template rebind_alloc<T>;

    using TrieChildren = std::unordered_map<Key, ChildPtr, std::hash<Key>, std::equal_to<Key>,
                                            RebindAlloc<std::pair<const Key, ChildPtr>>>;

    TrieChildren _children = {};
    std::optional<Value> _leafValue;

   public:
    explicit TrieNode () = default;
    explicit TrieNode (TrieNode&&) = delete;
    explicit TrieNode (const TrieNode&) = delete;

    void set (const Key& key, const ChildPtr value) { _children[key] = value; }
    ChildPtr get (const Key& key) const {
        assert (_children.find (key) != _children.end ());
        const auto find = _children.find (key);
        return find->second;
    }
    [[nodiscard]] bool isLeaf () const { return _leafValue.has_value (); }
    void setLeaf (const Value& v) { _leafValue = v; }
    Value& leafValue () {
        assert (_leafValue.has_value ());
        return _leafValue.value ();
    }
    bool contains (const Key& key) const { return _children.find (key) != _children.end (); }
};

template <CanbeTrieKey Key, typename Value, typename keyAllocator = std::allocator<Key>>
class TrieMap final {
    template <class T>
        requires CanbeTrieNode<T, Key, Value>
    using ChildPtr = std::shared_ptr<T>;

    template <class T>
    using RebindAlloc = typename std::allocator_traits<keyAllocator>::template rebind_alloc<T>;

    using TrieNode = TrieNode<Key, Value, keyAllocator>;

    ChildPtr<TrieNode> d_head{std::make_shared<TrieNode> ()};

    using NodeAlloc = RebindAlloc<ChildPtr<TrieNode>>;

    NodeAlloc d_nodeAlloc;

   public:
    TrieMap () = default;

    template <std::ranges::input_range Range>
        requires std::convertible_to<std::ranges::range_value_t<Range>, Key>
    void set (Range&& keys, Value&& value) {
        auto node = d_head;
        for (const auto& key : keys) {
            assert (node != nullptr);
            if (node->contains (key)) {
                node = node->get (key);
            } else {
                auto newNode = std::allocate_shared<TrieNode> (d_nodeAlloc);
                node->set (key, newNode);
                node = newNode;
            }
        }
        node->setLeaf (std::forward<Value> (value));
    }

    template <std::ranges::input_range Range>
        requires std::convertible_to<std::ranges::range_value_t<Range>, Key>
    std::optional<std::reference_wrapper<Value>> get (Range&& keys) {
        auto node = d_head;
        for (const auto& key : keys) {
            assert (node != nullptr);
            if (node->contains (key)) {
                node = node->get (key);
            } else {
                return {};
            }
        }
        if (!node->isLeaf ()) {
            return {};
        }

        return {std::reference_wrapper<Value>{node->leafValue ()}};
    }

    template <std::ranges::input_range Range>
        requires std::convertible_to<std::ranges::range_value_t<Range>, Key>
    void erase (const Key& keys) {
        auto node = d_head;
        for (const auto& key : keys) {
            assert (node != nullptr);
            if (node->contains (key)) {
                node = node->get (key);
            } else {
                return;
            }
        }
        if (node->isLeaf ()) {
            node->leafValue ().clear ();
        }
    }

    void clear () {
        std::stack<ChildPtr<TrieNode>> nodeStack;
        nodeStack.push (d_head);
        while (!nodeStack.empty ()) {
            auto node = nodeStack.top ();
            nodeStack.pop ();
            for (const auto& child : node->_children) {
                nodeStack.push (child);
            }
            node->_children.clear ();
        }
        d_head = nullptr;
    }
};

template <CanbeTrieKey Key>
using TrieSet = TrieMap<Key, bool>;

}  // namespace datastructure

#endif  // TRIEMAP_H
