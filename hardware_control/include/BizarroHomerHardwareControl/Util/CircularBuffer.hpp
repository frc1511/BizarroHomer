// Copyright (c) 2009-2023 FIRST and other WPILib contributors All rights reserved.
// 
// Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:
// 
//     Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.
//     Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.
//     Neither the name of FIRST, WPILib, nor the names of other WPILib contributors may be used to endorse or promote products derived from this software without specific prior written permission.
// 
// THIS SOFTWARE IS PROVIDED BY FIRST AND OTHER WPILIB CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY NONINFRINGEMENT AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL FIRST OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

#pragma once

#include <cstddef>
#include <vector>

template <class T>
class CircularBuffer {
public:
  explicit CircularBuffer(size_t size) : m_data(size, T{}) {}
  
  CircularBuffer(const CircularBuffer&) = default;
  CircularBuffer& operator=(const CircularBuffer&) = default;
  CircularBuffer(CircularBuffer&&) = default;
  CircularBuffer& operator=(CircularBuffer&&) = default;
  
  class iterator {
  public:
    using iterator_category = std::forward_iterator_tag;
    using value_type = T;
    using difference_type = std::ptrdiff_t;
    using pointer = T*;
    using reference = T&;
    
    iterator(CircularBuffer* buffer, size_t index)
        : m_buffer(buffer), m_index(index) {}
    
    iterator& operator++() {
      ++m_index;
      return *this;
    }
    iterator operator++(int) {
      iterator retval = *this;
      ++(*this);
      return retval;
    }
    bool operator==(const iterator&) const = default;
    reference operator*() { return (*m_buffer)[m_index]; }
    
  private:
    CircularBuffer* m_buffer;
    size_t m_index;
  };
  
  class const_iterator {
  public:
    using iterator_category = std::forward_iterator_tag;
    using value_type = T;
    using difference_type = std::ptrdiff_t;
    using pointer = T*;
    using const_reference = const T&;
    
    const_iterator(const CircularBuffer* buffer, size_t index)
        : m_buffer(buffer), m_index(index) {}
    
    const_iterator& operator++() {
      ++m_index;
      return *this;
    }
    const_iterator operator++(int) {
      const_iterator retval = *this;
      ++(*this);
      return retval;
    }
    bool operator==(const const_iterator&) const = default;
    const_reference operator*() const { return (*m_buffer)[m_index]; }
    
  private:
    const CircularBuffer* m_buffer;
    size_t m_index;
  };
  
  iterator begin() { return iterator(this, 0); }
  iterator end() { return iterator(this, ::CircularBuffer<T>::size()); }
  
  const_iterator begin() const { return const_iterator(this, 0); }
  const_iterator end() const {
    return const_iterator(this, ::CircularBuffer<T>::size());
  }
  
  const_iterator cbegin() const { return const_iterator(this, 0); }
  const_iterator cend() const {
    return const_iterator(this, ::CircularBuffer<T>::size());
  }
  
  size_t size() const { return m_length; }
  
  T& front() { return (*this)[0]; }
  
  const T& front() const { return (*this)[0]; }
  
  T& back() { return m_data[(m_front + m_length - 1) % m_data.size()]; }
  
  const T& back() const {
    return m_data[(m_front + m_length - 1) % m_data.size()];
  }
  
  void push_front(T value) {
    if (m_data.size() == 0) {
      return;
    }
    
    m_front = ModuloDec(m_front);
    
    m_data[m_front] = value;
    
    if (m_length < m_data.size()) {
      m_length++;
    }
  }
  
  void push_back(T value) {
    if (m_data.size() == 0) {
      return;
    }
    
    m_data[(m_front + m_length) % m_data.size()] = value;
    
    if (m_length < m_data.size()) {
      m_length++;
    } else {
      m_front = ModuloInc(m_front);
    }
  }
  
  template <class... Args>
  void emplace_front(Args&&... args) {
    if (m_data.size() == 0) {
      return;
    }
    
    m_front = ModuloDec(m_front);
    
    m_data[m_front] = T{args...};
    
    if (m_length < m_data.size()) {
      m_length++;
    }
  }
  
  template <class... Args>
  void emplace_back(Args&&... args) {
    if (m_data.size() == 0) {
      return;
    }
    
    m_data[(m_front + m_length) % m_data.size()] = T{args...};
    
    if (m_length < m_data.size()) {
      m_length++;
    } else {
      m_front = ModuloInc(m_front);
    }
  }
  
  T pop_front() {
    T& temp = m_data[m_front];
    m_front = ModuloInc(m_front);
    m_length--;
    return temp;
  }
  
  T pop_back() {
    m_length--;
    return m_data[(m_front + m_length) % m_data.size()];
  }
  
  void resize(size_t size) {
    if (size > m_data.size()) {
      size_t insertLocation = (m_front + m_length) % m_data.size();
      if (insertLocation <= m_front) {
        m_front += size - m_data.size();
      }
      m_data.insert(m_data.begin() + insertLocation, size - m_data.size(), 0);
    } else if (size < m_data.size()) {
      size_t elemsToRemove = m_data.size() - size;
      auto frontIter = m_data.begin() + m_front;
      if (m_front < elemsToRemove) {
        m_data.erase(frontIter + size, m_data.end());
        m_data.erase(m_data.begin(), frontIter);
        m_front = 0;
      } else {
        m_data.erase(frontIter - elemsToRemove, frontIter);
        m_front -= elemsToRemove;
      }

      if (m_length > size) {
        m_length = size;
      }
    }
  }
  
  void reset() {
    m_front = 0;
    m_length = 0;
  }
  
  T& operator[](size_t index) {
    return m_data[(m_front + index) % m_data.size()];
  }
  
  const T& operator[](size_t index) const {
    return m_data[(m_front + index) % m_data.size()];
  }
  
private:
  std::vector<T> m_data;
  
  size_t m_front = 0;
  size_t m_length = 0;
  
  size_t ModuloInc(size_t index) { return (index + 1) % m_data.size(); }
  
  size_t ModuloDec(size_t index) {
    if (index == 0) {
      return m_data.size() - 1;
    } else {
      return index - 1;
    }
  }
};
