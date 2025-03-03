#pragma once

#include <memory>
#include <optional>
#include <vector>

namespace treeceratops {

using node_id = std::size_t;

template <class T>
struct node_type_ {
  std::optional<node_id> parent;
  std::optional<node_id> prev;
  std::optional<node_id> next;
  T data;
  std::vector<node_id> children;
};

template <class T, class Allocator>
class tree;

template <class T, class Allocator, class Ttree>
class tree_iterator {
 public:
  friend tree<T, Allocator>;
  template <class T1, class Allocator1, class Ttree1>
  friend class tree_iterator;

  tree_iterator() = default;

  tree_iterator(Ttree *tree, node_id node) : tree_{tree}, node_{node} {}

  auto operator*() -> T & { return tree_->data_[node_].data; }
  auto operator*() const -> const T & { return tree_->data_[node_].data; }
  auto operator->() -> T * { return &tree_->data_[node_].data; }
  auto operator->() const -> const T * { return &tree_->data_[node_].data; }

  auto operator[](std::size_t idx) -> tree_iterator {
    return {tree_, tree_->data_[node_].children[idx]};
  }

  auto operator++() -> tree_iterator & {
    if (!tree_->data_[node_].children.empty()) {
      node_ = tree_->data_[node_].children.front();
    } else {
      while (!tree_->data_[node_].next) {
        if (tree_->data_[node_].parent) {
          node_ = *tree_->data_[node_].parent;
        } else {
          tree_ = nullptr;
          node_ = 0;
          return *this;
        }
      }
      node_ = *tree_->data_[node_].next;
    }
    return *this;
  }

  auto operator++(int) -> tree_iterator {
    auto old = *this;
    this->operator++();
    return old;
  }

  template <class T1, class Allocator1, class Ttree1>
  auto operator!=(const tree_iterator<T1, Allocator1, Ttree1> &iter) const
      -> bool {
    return !(tree_ == iter.tree_ && node_ == iter.node_);
  }

  auto parent() -> tree_iterator {
    if (tree_->data_[node_].parent) {
      return tree_iterator{tree_, *tree_->data_[node_].parent};
    }
    return tree_iterator{};
  }

  auto child(std::size_t idx) -> tree_iterator {
    return tree_iterator{tree_, *tree_->data_[node_].children[idx]};
  }

 private:
  Ttree *tree_ = nullptr;
  node_id node_ = 0;
};

template <class T, class Allocator = std::allocator<node_type_<T>>>
class tree {
 public:
  using node_type = node_type_<T>;
  using iterator = tree_iterator<T, Allocator, tree<T, Allocator>>;
  using const_iterator =
      const tree_iterator<T, Allocator, const tree<T, Allocator>>;
  friend iterator;

  // Element Access
  auto at(node_id node) -> T& { return data_.at(node).data; }
  auto operator[](node_id node) -> T& { return data_[node].data; }

  // Iterators
  auto begin() -> iterator { return iterator{this, *root_}; }
  [[nodiscard]] auto begin() const -> const_iterator { return const_iterator{this, *root_}; }
  auto end() -> iterator { return iterator{}; }
  [[nodiscard]] auto end() const -> const_iterator { return const_iterator{}; }

  // Capacity
  auto empty() -> bool{ return root_.has_value(); }
  auto size() -> std::size_t;

  // Modifiers
  void clear() {
    data_.clear();
    root_ = std::nullopt;
  }

  void push_root(const T &value) {
    if (root_) {
      data_[*root_].parent = data_.size();
      data_.push_back({{}, {}, {}, value, {*root_}});
    } else {
      data_.push_back({{}, {}, {}, value, {}});
    }
    root_ = data_.size() - 1;
  }

  void push_child(const iterator &pos, const T &value) {
    node_id node = data_.size();
    if (!data_[pos.node_].children.empty()) {
      data_.push_back(
          {pos.node_, data_[pos.node_].children.back(), {}, value, {}});
      data_[data_[pos.node_].children.back()].next = node;
    } else {
      data_.push_back({pos.node_, {}, {}, value, {}});
    }
    data_[pos.node_].children.push_back(node);
  }

  void make_parent(const iterator &parent_pos, const iterator &child) {
    auto prev = data_[child.node_].prev;
    auto next = data_[child.node_].next;
    auto parent = data_[child.node_].parent;
    if (prev) {
      data_[*prev].next = next;
    }
    if (next) {
      data_[*next].prev = next;
    }
    if (parent) {
      std::erase(data_[*parent].children, child.node_);
    }
    data_[child.node_].next = std::nullopt;
    data_[parent_pos.node_].children.emplace_back(child.node_);
    data_[child.node_].parent = parent_pos.node_;
  }

  auto insert(const_iterator pos, const T &value) -> iterator;

  template <std::size_t Idx, class TIt>
  auto child(const TIt &pos) -> TIt{
    return child_inner<Idx, TIt>(TIt{this, data_[pos.node_].children[0]});
  }

  auto last_child(const iterator &pos) -> iterator{
    return iterator{this, data_[pos.node_].children.back()};
  }

  auto depth(iterator pos) -> int{
    assert(pos.tree_ = this);
    int count = 0;

    while (pos != begin()) {
      pos = pos.parent();
      ++count;
    }

    return count;
  }

 private:
  template <std::size_t Idx, class TIt>
  auto child_inner(const TIt &pos) -> TIt {
    if constexpr (Idx == 0) {
      return pos;
    } else {
      return child_inner<Idx - 1, TIt>(TIt{this, *data_[pos.node_].next});
    }
  }

  std::vector<node_type, Allocator> data_;
  std::optional<node_id> root_;
};

}  // namespace treeceratops
