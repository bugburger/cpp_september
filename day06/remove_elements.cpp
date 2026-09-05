#include <iostream>
#include <vector>

struct ListNode {
    int val;
    ListNode* next;

    ListNode()
        : val(0), next(nullptr) {
    }

    explicit ListNode(int value)
        : val(value), next(nullptr) {
    }

    ListNode(int value, ListNode* nextNode)
        : val(value), next(nextNode) {
    }
};

class Solution {
public:
    ListNode* removeElements(ListNode* head, int val) {
        // 删除链表开头连续等于 val 的节点
        while (head != nullptr && head->val == val) {
            ListNode* nodeToDelete = head;
            head = head->next;
            delete nodeToDelete;
        }

        ListNode* current = head;

        // 删除链表中间和结尾等于 val 的节点
        while (current != nullptr &&
               current->next != nullptr) {
            if (current->next->val == val) {
                ListNode* nodeToDelete = current->next;

                current->next = nodeToDelete->next;

                delete nodeToDelete;
            } else {
                current = current->next;
            }
        }

        return head;
    }
};

ListNode* createList(const std::vector<int>& values) {
    ListNode dummy;
    ListNode* tail = &dummy;

    for (int value : values) {
        tail->next = new ListNode(value);
        tail = tail->next;
    }

    return dummy.next;
}

void printList(const ListNode* head) {
    std::cout << '[';

    const ListNode* current = head;

    while (current != nullptr) {
        std::cout << current->val;

        if (current->next != nullptr) {
            std::cout << ", ";
        }

        current = current->next;
    }

    std::cout << "]\n";
}

void destroyList(ListNode* head) {
    while (head != nullptr) {
        ListNode* nextNode = head->next;

        delete head;

        head = nextNode;
    }
}

int main() {
    ListNode* head =
        createList({6, 6, 1, 2, 6, 3, 6, 6});

    std::cout << "删除前：";
    printList(head);

    Solution solution;
    head = solution.removeElements(head, 6);

    std::cout << "删除后：";
    printList(head);

    // 释放链表中剩余的节点
    destroyList(head);
    head = nullptr;

    return 0;
}
