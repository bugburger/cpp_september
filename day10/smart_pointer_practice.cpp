#include <iostream>
#include <memory>
#include <string>

using namespace std;

class Logger {
public:
    Logger() {
        cout << "Logger created" << endl;
    }

    ~Logger() {
        cout << "Logger destroyed" << endl;
    }

    void log(const string& message) {
        cout << "[LOG] " << message << endl;
    }
};

class Connection {
public:
    Connection() {
        cout << "Connection created" << endl;
    }

    ~Connection() {
        cout << "Connection destroyed" << endl;
    }

    void send() {
        cout << "Sending data..." << endl;
    }
};


class Application {
private:
    // TODO 1:
    // Application 独占 Logger
    // 应该使用什么智能指针？
 	unique_ptr<Logger> logger_;   

public:
    Application() : logger_(make_unique<Logger>()) {
        // TODO 2:
        // 创建 Logger
    }

    void run() {
        logger_->log("Application is running");
    }
};


class Server {
private:
    // TODO 3:
    // Server 与 Worker 共同拥有 Connection
    shared_ptr<Connection> connection_;
    

public:
    Server(shared_ptr<Connection> conn)
        : connection_(conn) {
    }

    void work() {
        connection_->send();
    }
};


class Worker {
private:
    // TODO 4:
    // Worker 与 Server 共享 Connection
    shared_ptr<Connection> connection_;

public:
    Worker(shared_ptr<Connection> conn)
        : connection_(conn) {
    }
};


class Monitor {
private:
    // TODO 5:
    // Monitor 只观察 Connection
    weak_ptr<Connection> connection_;

public:
    Monitor(shared_ptr<Connection> conn)
        : connection_(conn) {
    }

    void check() {
        // TODO 6:
        // 检查 Connection 是否还存在
        // 如果存在，调用 send()
        // 如果不存在，输出：
        // Connection no longer exists
	shared_ptr<Connection> temp = connection_.lock();
	if (temp) {
		temp->send();
	}
	else {
		cout << "Connection no longer exists" << endl;
	}
    }
};


int main() {

    cout << "===== unique_ptr =====" << endl;

    {
        Application app;
        app.run();
    }


    cout << endl;
    cout << "===== shared_ptr + weak_ptr =====" << endl;

    Monitor* monitor = nullptr;

    {
        auto connection = make_shared<Connection>();

        cout << "count = "
             << connection.use_count()
             << endl;

        Server server(connection);

        cout << "count after Server = "
             << connection.use_count()
             << endl;

        Worker worker(connection);

        cout << "count after Worker = "
             << connection.use_count()
             << endl;

        monitor = new Monitor(connection);

        cout << "count after Monitor = "
             << connection.use_count()
             << endl;

        monitor->check();
    }

    cout << "Connection owners destroyed" << endl;

    monitor->check();

    delete monitor;

    return 0;
}
