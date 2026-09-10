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
    int getLength(const ListNode* head) const {
        int length = 0;

        while (head != nullptr) {
            ++length;
            head = head->next;
        }

        return length;
    }

    // 解法一：先计算链表长度
    ListNode* removeNthFromEndByLength(
        ListNode* head,
        int n) const {

        ListNode dummyHead(0, head);
        ListNode* previous = &dummyHead;
        int length = getLength(head);

        for (int i = 0; i < length - n; ++i) {
            previous = previous->next;
        }

        ListNode* nodeToDelete = previous->next;
        previous->next = nodeToDelete->next;
        delete nodeToDelete;

        return dummyHead.next;
    }

    // 解法二：快慢双指针
    ListNode* removeNthFromEnd(
        ListNode* head,
        int n) const {

        ListNode dummyHead(0, head);
        ListNode* slow = &dummyHead;
        ListNode* fast = &dummyHead;

        // fast 与 slow 保持 n + 1 步的距离
        for (int i = 0; i <= n; ++i) {
            fast = fast->next;
        }

        while (fast != nullptr) {
            fast = fast->next;
            slow = slow->next;
        }

        ListNode* nodeToDelete = slow->next;
        slow->next = nodeToDelete->next;
        delete nodeToDelete;

        return dummyHead.next;
    }
};

ListNode* createList(int begin, int end) {
    ListNode dummyHead;
    ListNode* tail = &dummyHead;

    for (int value = begin; value <= end; ++value) {
        tail->next = new ListNode(value);
        tail = tail->next;
    }

    return dummyHead.next;
}

void printList(const ListNode* head) {
    while (head != nullptr) {
        std::cout << head->val << " -> ";
        head = head->next;
    }

    std::cout << "nullptr\n";
}

void destroyList(ListNode* head) {
    while (head != nullptr) {
        ListNode* nextNode = head->next;
        delete head;
        head = nextNode;
    }
}

int main() {
    Solution solution;

    ListNode* firstList = createList(1, 5);

    std::cout << "Before removing by length:\n";
    printList(firstList);

    firstList =
        solution.removeNthFromEndByLength(firstList, 2);

    std::cout << "After removing the 2nd node from the end:\n";
    printList(firstList);

    destroyList(firstList);

    ListNode* secondList = createList(1, 5);

    std::cout << "\nBefore removing with two pointers:\n";
    printList(secondList);

    secondList =
        solution.removeNthFromEnd(secondList, 2);

    std::cout << "After removing the 2nd node from the end:\n";
    printList(secondList);

    destroyList(secondList);

    return 0;
}
