#include "buffer.hpp"
#include <iostream>
#include <utility>

Buffer::Buffer(std::size_t size)
       	: data_(new int[size]{}), size_(size) {
		std::cout << "Constructor, size = "
		   	  << size_ << std::endl;
}

Buffer::~Buffer() {
	std::cout << "Destructor, size = "
		  << size_ << std::endl;

	delete[] data_;
}

Buffer::Buffer(const Buffer& other)
	: data_(new int[other.size_]), size_(other.size_) {
		for (std::size_t i = 0; i < size_; ++i) {
			data_[i] = other.data_[i];
		}
		std::cout << "Copy Constructor" << std::endl;
}

Buffer& Buffer::operator = (const Buffer& other) {
	std::cout << "Copy assighment"
		  << std::endl;

	if (this != &other) {
		delete[] data_;
		size_ = other.size_;
		data_ = new int[size_];
		for (std::size_t i = 0; i < size_; ++i) {
			data_[i] = other.data_[i];
		}
	}
	return *this;
}

Buffer::Buffer(Buffer&& other) noexcept
	: data_(other.data_), size_(other.size_) {
		other.data_ = nullptr;
		other.size_ = 0;

		std::cout << "Move constructor" << std::endl;
}

Buffer& Buffer::operator = (Buffer&& other) noexcept {
	std::cout << "Move assignment" << std::endl;

	if (this != &other) {
		delete[] data_;
		data_ = other.data_;
		size_ = other.size_;

		other.data_ = nullptr;
		other.size_ = 0;
	}
	return *this;
}

void Buffer::set(std::size_t index, int value) {
    if (index < size_) {
        data_[index] = value;
    }
}

int Buffer::get(std::size_t index) const {
    if (index < size_) {
        return data_[index];
    }

    return 0;
}

std::size_t Buffer::size() const {
    return size_;
}
