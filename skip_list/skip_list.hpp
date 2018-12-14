#ifndef __SKIP_LIST_H__
#define __SKIP_LIST_H__

#include <cstddef>
#include <cstdlib>
#include <functional>
#include <iostream>
#include <memory>
#include <optional>
#include <stack>

template <class Key, class T, class Compare = std::less<Key>> class SkipList {
public:
  static const size_t DEFAULT_DEPTH = 5;
  SkipList() : depth_(DEFAULT_DEPTH) {}
  SkipList(size_t depth) : depth_(depth) {}

  void insert(Key key, const T &value) {
    if (!first_)
      return insertFirst(key, value);

    return insertAdditional(key, value);
  }

  void erase(Key key) {
    auto node = findNode(key);
    return markElemDeleted(node);
  }

  std::optional<T> find(Key key) const {
    auto node = findNode(key);
    return node ? std::optional<T>(node->value_) : std::optional<T>();
  }

  void print() const {
    auto level = first_;
    while (level) {
      printLevel(level);
      level = level->below_;
    }
  }

private:
  struct Node;
  using NodeRef = std::shared_ptr<Node>;

  NodeRef findNode(Key key) const {
    auto cur = first_;
    while (cur && !keysEqual(key, cur->key_))
      cur = findNext(cur, key);

    return cur;
  }

  bool keysEqual(Key first, Key second) const {
    return !compare_(first, second) && !compare_(second, first);
  }

  void markElemDeleted(NodeRef node) {
    while (node) {
      node->deleted_ = true;
      node = node->below_;
    }
  }

  void insertFirst(Key key, const T &value) {
    first_.reset(new Node{key, value, false});
    auto cur = first_;
    for (size_t i = 1; i < depth_; i++) {
      auto next = std::make_shared<Node>(*cur);
      cur->below_ = next;
      cur = next;
    }
  }

  void insertAdditional(Key key, const T &value) {
    auto prevs = getPrevs(key);
    auto node = insertNodeAfterNode(prevs.top(), key, value);
    prevs.pop();

    while (prevs.size() && node) {
      node = insertAdditionalNode(prevs.top(), key, value, node);
      prevs.pop();
    }
  }

  NodeRef insertNodeAfterNode(NodeRef node, Key key, const T &value, NodeRef below = NodeRef()) {
    if (!node)
      return NodeRef();

    NodeRef new_node(new Node{key, value, false, node->right_, below});
    node->right_ = new_node;
    return new_node;
  }

  NodeRef insertAdditionalNode(NodeRef elem, Key key, const T &value, NodeRef below) {
    static const double INSERTION_PROB = 0.5;
    int random = std::abs(std::rand()) % 100 + 1;
    if (random < INSERTION_PROB * 100)
      return NodeRef();

    return insertNodeAfterNode(elem, key, value, below);
  }

  NodeRef findRightAndRemoveDeleted(NodeRef cur, Key key) const {
    auto prev = cur;
    while (cur && compare_(cur->key_, key)) {
      if (prev != cur && cur->deleted_)
        prev->right_ = cur->right_;
      else
        prev = cur;

      cur = cur->right_;
    }
    return (cur && keysEqual(cur->key_, key)) ? cur : prev;
  }

  std::stack<NodeRef> getPrevs(Key key) const {
    std::stack<NodeRef> prevs;
    auto cur = first_;
    for (size_t i = 0; i < depth_; i++) {
      prevs.push(findRightAndRemoveDeleted(cur, key));
      cur = cur->below_;
    }
    return prevs;
  }

  NodeRef findNext(NodeRef cur, Key key) const {
    auto right = findRightAndRemoveDeleted(cur, key);
    if (right && right != cur)
      return right;

    return cur ? cur->below_ : cur;
  }

  void printLevel(NodeRef level) const {
    std::cout << "{ ";
    auto &node = level;
    while (node) {
      printNode(node);
      node = node->right_;
      std::cout << " -> ";
    }
    std::cout << " }" << std::endl;
  }

  void printNode(NodeRef node) const {
    std::cout << "key = " << node->key_ << ", "
              << "value = " << node->value_;
    if (node->deleted_)
      std::cout << ", node deleted";
  }

  struct Node {
    Key key_;
    T value_;
    bool deleted_;
    NodeRef right_;
    NodeRef below_;
  };
  NodeRef first_;
  const size_t depth_;
  Compare compare_;
};

#endif // __SKIP_LIST_H__
