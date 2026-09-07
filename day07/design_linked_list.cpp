#include <iostream>

class MyLinkedList {
private:
    struct ListNode {
        int val;
        ListNode* next;

        ListNode(int value)
            : val(value), next(nullptr) {
        }
    };

    ListNode* dummyHead_;
    int size_;

public:
    MyLinkedList()
        : dummyHead_(new ListNode(0)),
          size_(0) {
    }

    ~MyLinkedList() {
        ListNode* cur = dummyHead_;

        while (cur != nullptr) {
            ListNode* nextNode = cur->next;

            delete cur;

            cur = nextNode;
        }

        dummyHead_ = nullptr;
        size_ = 0;
    }

    int get(int index) {
        if (index < 0 || index >= size_) {
            return -1;
        }

        ListNode* cur = dummyHead_->next;

        for (int i = 0; i < index; ++i) {
            cur = cur->next;
        }

        return cur->val;
    }

    void addAtHead(int val) {
        ListNode* newNode = new ListNode(val);

        newNode->next = dummyHead_->next;
        dummyHead_->next = newNode;

        ++size_;
    }

    void addAtTail(int val) {
        ListNode* newNode = new ListNode(val);

        ListNode* cur = dummyHead_;

        while (cur->next != nullptr) {
            cur = cur->next;
        }

        cur->next = newNode;

        ++size_;
    }

    void addAtIndex(int index, int val) {
        if (index > size_) {
            return;
        }

        if (index < 0) {
            index = 0;
        }

        ListNode* newNode = new ListNode(val);

        ListNode* cur = dummyHead_;

        for (int i = 0; i < index; ++i) {
            cur = cur->next;
        }

        newNode->next = cur->next;
        cur->next = newNode;

        ++size_;
    }

    void deleteAtIndex(int index) {
        if (index < 0 || index >= size_) {
            return;
        }

        ListNode* cur = dummyHead_;

        for (int i = 0; i < index; ++i) {
            cur = cur->next;
        }

        ListNode* temp = cur->next;

        cur->next = temp->next;

        delete temp;

        --size_;
    }

    void clear() {
	    ListNode* cur = dummyHead_->next;
	    while (cur != nullptr) {
		    ListNode* newNode = cur->next;
		    delete cur;
		    cur = newNode;
	    }
	    dummyHead_->next = nullptr;
	    size_ = 0;
    }
};

int main() {
    MyLinkedList list;

    list.addAtHead(10);
    list.addAtHead(20);
    list.addAtTail(30);
    list.addAtIndex(1, 99);

    std::cout << "index 0: "
              << list.get(0) << '\n';

    std::cout << "index 1: "
              << list.get(1) << '\n';

    std::cout << "index 2: "
              << list.get(2) << '\n';

    std::cout << "index 3: "
              << list.get(3) << '\n';

    list.deleteAtIndex(1);

    std::cout << "\nAfter delete index 1:\n";

    std::cout << "index 0: "
              << list.get(0) << '\n';

    std::cout << "index 1: "
              << list.get(1) << '\n';

    std::cout << "index 2: "
              << list.get(2) << '\n';

    return 0;
}
