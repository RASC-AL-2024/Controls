#pragma once

#include <new>
#include <stdint.h>

namespace std {

template <class T>
struct remove_const {
  typedef T type;
};
template <class T>
struct remove_const<const T> {
  typedef T type;
};

} // namespace std

namespace abc {

template <typename T>
struct Storage {
  alignas(T) uint8_t storage[sizeof(T)];

  template <typename U>
  U* pointer() {
    return reinterpret_cast<U*>(storage);
  }

  template <typename U>
  const U* pointer() const {
    return reinterpret_cast<const U*>(storage);
  }

  T* default_pointer() { return pointer<T>(); }
  const T* default_pointer() const { return pointer<T>(); }
};

template <typename T, size_t capacity_>
class Vector {
public:
  // This is class only safe for trivial types for now
  static_assert(capacity_ > 0);

  Vector() = default;
  Vector(const Vector& other) { append(other); }
  ~Vector() { clear(); }
  Vector& operator=(const Vector& other) {
    clear();
    append(other);
    return *this;
  }

  size_t size() const { return size_; }
  size_t capacity() const { return capacity_; }
  bool empty() const { return size_ == 0; }
  bool full() const { return size_ == capacity_; }

  void push(const T& x) {
    new (data_[size_].default_pointer()) T{x};
    ++size_;
  }
  void pop() {
    --size_;
    data_[size_].default_pointer()->~T();
  }

  const T& front() const { return this->operator[](0); }
  T& front() { return this->operator[](0); }
  const T& back() const { return this->operator[](size_ - 1); }
  T& back() { return this->operator[](size_ - 1); }

  const T& operator[](size_t i) const { return *data_[i].default_pointer(); }
  T& operator[](size_t i) { return *data_[i].default_pointer(); }

  T* data() { return &front(); }
  const T* data() const { return &front(); }

  void clear() {
    while (!empty())
      pop();
  }

  void erase(size_t i) {
    for (; (i + 1) < size(); ++i) {
      data_[i].default_pointer()->~T();
      new (data_[i].default_pointer()) T(*data_[i + 1].default_pointer());
    }
    pop();
  }

  // Iterators
  T* begin() { return data_[0].default_pointer(); }
  const T* begin() const { return data_[0].default_pointer(); }
  T* end() { return data_[0].default_pointer() + size_; }
  const T* end() const { return data_[0].default_pointer() + size_; }

  // Compare (TODO switch to <=>)

  template <size_t other_capacity>
  bool operator==(const Vector<T, other_capacity>& other) const {
    if (size() != other.size())
      return false;
    for (size_t i = 0; i < size(); ++i)
      if (operator[](i) != other[i])
        return false;
    return true;
  }

private:
  size_t size_ = 0;
  // TODO: Could keep a pointer to this in a base class to avoid needing to
  // template on capacity in functions that use Vector
  Storage<typename std::remove_const<T>::type> data_[capacity_]{};
};

template <typename T>
class Optional {
public:
  Optional() : is_some{false} {}
  Optional(const Optional& other) : is_some{other.is_some} {
    if (is_some)
      construct(other.value());
  }
  Optional(const T& value) { construct(value); }
  ~Optional() { reset(); }
  Optional& operator=(const Optional& other) {
    reset();
    if (other.is_some) {
      construct(other.value());
    }
    return *this;
  }
  Optional& operator=(const T& value) {
    reset();
    construct(value);
    return *this;
  }
  void reset() {
    if (is_some) {
      value().~T();
      is_some = false;
    }
  }
  explicit operator bool() const { return is_some; }
  bool has_value() const { return is_some; }
  T& value() { return *reinterpret_cast<T*>(data); }
  const T& value() const { return *reinterpret_cast<const T*>(data); }
  const T& value_or(const T& other) const { return is_some ? value() : other; }

  const T* operator->() const { return &value(); }
  T* operator->() { return &value(); }

  T take() {
    T x = value();
    reset();
    return x;
  }

private:
  void construct(const T& value) {
    is_some = true;
    new (reinterpret_cast<T*>(data)) T(value);
  }

  alignas(T) uint8_t data[sizeof(T)];
  bool is_some;
};

template <typename T, typename U>
struct Pair {
  T first;
  U second;

  bool operator==(const Pair<T, U>& other) const {
    return first == other.first && second == other.second;
  }
};

template <typename K, typename V, size_t capacity_>
class Map {
public:
  using KV = abc::Pair<K, V>;

  Map() = default;
  void insert(const K& k, const V& v) { kvs.push({k, v}); }

  const V& get(const K& k) const { return kvs[index(k).value()].second; }
  V& get(const K& k) { return kvs[index(k).value()].second; }
  abc::Optional<V> maybe_get(const K& k) {
    auto idx = index(k);
    if (!idx)
      return {};
    return {kvs[idx.value()].second};
  }

  V& operator[](const K& k) {
    auto idx = index(k);
    if (idx)
      return kvs[idx.value()].second;
    insert(k, {});
    return get(k);
  }
  bool contains(const K& k) const { return index(k).has_value(); }

  void erase(const K& k) {
    auto idx = index(k);
    if (idx) {
      kvs.erase(idx.value());
    }
  }

  size_t size() const { return kvs.size(); }
  size_t capacity() const { return capacity_; }
  bool empty() const { return size() == 0; }
  bool full() const { return kvs.full(); }

  void clear() { kvs.clear(); }

  KV* begin() { return kvs.begin(); }
  const KV* begin() const { return kvs.begin(); }
  KV* end() { return kvs.end(); }
  const KV* end() const { return kvs.end(); }

private:
  Optional<size_t> index(const K& k) const {
    for (size_t i = 0; i < kvs.size(); ++i)
      if (kvs[i].first == k)
        return {i};
    return {};
  }

  Vector<KV, capacity_> kvs{};
};

} // namespace abc
