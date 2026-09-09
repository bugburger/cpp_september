#include <iostream>

struct ListNode {
    int val;
    ListNode* next;

    ListNode()
        : val(0), next(nullptr) {
    }

    explicit ListNode(int x)
        : val(x), next(nullptr) {
    }

    ListNode(int x, ListNode* nextNode)
        : val(x), next(nextNode) {
    }
};

class Solution {
public:
    ListNode* swapPairs(ListNode* head) {
        ListNode dummyHead(0);
        dummyHead.next = head;

        ListNode* cur = &dummyHead;

        while (cur->next != nullptr &&
               cur->next->next != nullptr) {

            ListNode* first = cur->next;
            ListNode* second = first->next;
            ListNode* nextNode = second->next;

            second->next = first;
            cur->next = second;
            first->next = nextNode;

            cur = first;
        }

        return dummyHead.next;
    }
};

void printList(const ListNode* head) {
    const ListNode* cur = head;

    while (cur != nullptr) {
        std::cout << cur->val
                  << " -> ";

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
    head->next->next->next->next =
        new ListNode(5);

    std::cout << "Before swap:\n";
    printList(head);

    Solution solution;
    head = solution.swapPairs(head);

    std::cout << "After swap:\n";
    printList(head);

    destroyList(head);
    head = nullptr;

    return 0;
}
