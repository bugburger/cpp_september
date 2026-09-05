#include <iostream>
#include <cstdio>

class FileHandle {
private:
	std::FILE* file_;

public:
	FileHandle(const FileHandle&) = delete;
        FileHandle& operator=(const FileHandle&) = delete;

	explicit FileHandle(const char* filename) : file_(std::fopen(filename, "w")) {
		if (file_ != nullptr) {
			std::cout << "文件打开成功\n";
		} else {
			std::cout << "文件打开失败\n";
		}
	}
	~FileHandle() {
		if (file_ != nullptr) {
			std::fclose(file_);
			file_ = nullptr;
			std::cout << "文件已关闭\n";
		}
	}

	bool isOpen() const {
		return file_ != nullptr;
	}

	void write(const char* text) {
		if (file_ != nullptr) {
			std::fputs(text, file_);
		}
	}
};

void testEarlyReturn() {
	FileHandle file("raii_output.txt");

	if (!file.isOpen()) {
		return;
	}

	    file.write("第一行：RAII 文件测试\n");

    std::cout << "函数准备提前 return\n";

    return;

    file.write("这一行不会写入文件\n");
}

void testScope() {
	std::cout << "\n进入 testScope 函数\n";
	{
		FileHandle file("scope_output.txt");

		if (file.isOpen()) {
			file.write("测试内部作用域\n");
		}

		std::cout << "即将离开内部作用域\n";
	}

	std::cout << "已经离开内部作用域\n";
}

int main() {
    std::cout << "进入 main 函数\n";

    testEarlyReturn();
    testScope();

    std::cout << "已经回到 main 函数\n";

    return 0;
}
