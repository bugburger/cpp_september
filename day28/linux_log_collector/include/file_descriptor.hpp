#pragma once

class FileDescriptor {
private:
	int fd_;
	
public:
	explicit FileDescriptor(int fd = -1);

	~FileDescriptor();

	FileDescriptor(const FileDescriptor&) = delete;

	FileDescriptor& operator = (const FileDescriptor&) = delete;

	int get() const;

	bool valid() const;
};
