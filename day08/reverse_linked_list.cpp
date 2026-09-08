#include <iostream>

struct ListNode {
    int val;
    ListNode* next;

    explicit ListNode(int value)
        : val(value), next(nullptr) {
    }
};

ListNode* reverseList(ListNode* head) {
    ListNode* preNode = nullptr;
    ListNode* cur = head;

    while (cur != nullptr) {
        ListNode* nextNode = cur->next;

        cur->next = preNode;

        preNode = cur;
        cur = nextNode;
    }

    return preNode;
}

void printList(const ListNode* head) {
    const ListNode* cur = head;

    while (cur != nullptr) {
        std::cout << cur->val << " -> ";
        cur = cur->next;
    }

    std::cout << "nullptr\n";
}

void destroyList(ListNode* head) {
    ListNode* cur = head;

    while (cur != nullptr) {
        ListNode* nextNode = cur->next;
        delete cur;
        cur = nextNode;
    }
}

int main() {
    ListNode* head = new ListNode(1);
    head->next = new ListNode(2);
    head->next->next = new ListNode(3);
    head->next->next->next = new ListNode(4);
    head->next->next->next->next = new ListNode(5);

    std::cout << "Before reverse:\n";
    printList(head);

    head = reverseList(head);

    std::cout << "After reverse:\n";
    printList(head);

    destroyList(head);
    head = nullptr;

    return 0;
}
