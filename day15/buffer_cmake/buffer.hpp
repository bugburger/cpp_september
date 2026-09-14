#ifndef BUFFER_HPP
#define BUFFER_HPP

#include <cstddef>

class Buffer {
private:
	int *data_;
	std::size_t size_;

public:
	explicit Buffer(std::size_t size);

	~Buffer();

	Buffer(const Buffer& other);

	Buffer& operator = (const Buffer& other);

	Buffer(Buffer&& other) noexcept;

	Buffer& operator = (Buffer&& other) noexcept;

	void set(std::size_t index, int value);

	int get(std::size_t index) const;

	std::size_t size() const;
};

#endif

