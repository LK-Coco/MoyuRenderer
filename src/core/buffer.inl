#include "buffer.h"

namespace MR {

template <typename T>
inline Buffer<T>::Buffer() : data_(nullptr),
                             length_(0) {}

template <typename T>
inline Buffer<T>::Buffer(uint32_t length) : data_(new T[length]),
                                            length_(length) {}

template <typename T>
inline Buffer<T>::Buffer(const Buffer<T>& rhs) {
    data_ = new T[rhs.length_];
    memcpy(data_, rhs.data_, rhs.length_ * sizeof(T));
    length_ = rhs.length_;
}

template <typename T>
inline Buffer<T>& Buffer<T>::operator=(const Buffer<T>& rhs) {
    if (data_ == rhs.data_) return this;
    T* temp = data_;
    data_ = new T[rhs.length_];
    memcpy(data_, rhs.data_, rhs.length_ * sizeof(T));
    delete[] temp;
    length_ = rhs.length_;
}

template <typename T>
inline Buffer<T>::~Buffer() {
    if (data_ != nullptr) {
        delete[] data_;
        length_ = 0;
    }
}

template <typename T>
inline const uint32_t Buffer<T>::get_length() const noexcept {
    return length_;
}

template <typename T>
inline const T* Buffer<T>::get() const noexcept {
    return data_;
}

template <typename T>
inline T* Buffer<T>::get() noexcept {
    return data_;
}

}  // namespace MR