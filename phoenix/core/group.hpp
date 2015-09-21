#ifndef NALL_GROUP_HPP
#define NALL_GROUP_HPP

//Group
//vector of unique references

template<typename T> struct Group : protected nall::vector<T*> {
  Group& operator=(const Group& source) { nall::vector<T*>::operator=(source); return *this; }
  Group& operator=(Group&& source) { nall::vector<T*>::operator=(std::move(source)); return *this; }
  template<typename... Args> Group(Args&&... args) { construct(std::forward<Args>(args)...); }

  bool empty() const { return nall::vector<T*>::empty(); }
  unsigned size() const { return nall::vector<T*>::size(); }
  void reset() { nall::vector<T*>::reset(); }

  T& first() const { return *nall::vector<T*>::operator[](0); }

  //return true if at least one item was appended
  template<typename... Args> bool append(T& value, Args&&... args) {
    bool result = append(value);
    return append(std::forward<Args>(args)...) | result;
  }

  bool append(T& value) {
    if(nall::vector<T*>::find(&value)) return false;
    return nall::vector<T*>::append(&value), true;
  }

  //return true if at least one item was removed
  template<typename... Args> bool remove(T& value, Args&&... args) {
    bool result = remove(value);
    return remove(std::forward<Args>(args)...) | result;
  }

  bool remove(T& value) {
    if(auto position = nall::vector<T*>::find(&value)) return nall::vector<T*>::remove(position()), true;
    return false;
  }

  struct iterator : protected nall::vector<T*>::constIterator {
    T& operator*() const { return *nall::vector<T*>::constIterator::operator*(); }
    bool operator!=(const iterator& source) const { return nall::vector<T*>::constIterator::operator!=(source); }
    iterator& operator++() { nall::vector<T*>::constIterator::operator++(); return *this; }
    iterator(const Group& source, unsigned position) : nall::vector<T*>::constIterator(source, position) {}
  };

  const iterator begin() const { return iterator(*this, 0); }
  const iterator end() const { return iterator(*this, size()); }

private:
  void construct() {}
  void construct(const Group& source) { nall::vector<T*>::operator=(source); }
  void construct(Group&& source) { nall::vector<T*>::operator=(std::move(source)); }
  template<typename... Args> void construct(T& value, Args&&... args) {
    append(value);
    construct(std::forward<Args>(args)...);
  }
};

#endif
