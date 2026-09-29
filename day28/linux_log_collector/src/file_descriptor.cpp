#include "file_descriptor.hpp"

#include <unistd.h>

FileDescriptor::FileDescriptor(int fd) : fd_(fd) {}

FileDescriptor::~FileDescriptor() {
	if (fd_ >= 0) {
		close(fd_);
	}
}

int FileDescriptor::get() const {
	return fd_;
}

bool FileDescriptor::valid() const {
	return fd_ >= 0;
}
