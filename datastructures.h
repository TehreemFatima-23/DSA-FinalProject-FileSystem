#ifndef DATASTRUCTURES_H
#define DATASTRUCTURES_H

#include <QString>

// Dynamic Array
template <typename T>
class DynamicArray {
private:
    T* data;
    int cap;
    int len;

    void resize() {
        cap *= 2;
        T* newData = new T[cap];
        for (int i = 0; i < len; i++)
            newData[i] = data[i];

        delete[] data;
        data = newData;
    }

public:
    DynamicArray(int initial = 10) {
        cap = initial;
        len = 0;
        data = new T[cap];
    }

    ~DynamicArray() {
        delete[] data;
    }

    void push_back(const T& value) {
        if (len == cap) resize();
        data[len++] = value;
    }

    void removeLast() {
        if (len > 0) len--;
    }

    // NEW
    void insertAt(int index, const T& value) {
        if (index < 0 || index > len) return;
        if (len == cap) resize();

        for (int i = len; i > index; i--)
            data[i] = data[i - 1];

        data[index] = value;
        len++;
    }

    // NEW
    void removeAt(int index) {
        if (index < 0 || index >= len) return;

        for (int i = index; i < len - 1; i++)
            data[i] = data[i + 1];

        len--;
    }

    void clear() { len = 0; }

    int size() const { return len; }

    T& operator[](int index) { return data[index]; }
    const T& operator[](int index) const { return data[index]; }
};

// Stack
template <typename T>
class Stack {
private:
    struct Node {
        T data;
        Node* next;
    };

    Node* topNode;

public:
    Stack() : topNode(nullptr) {}

    bool isEmpty() const { return topNode == nullptr; }

    void push(const T& value) {
        Node* n = new Node{value, topNode};
        topNode = n;
    }

    T pop() {
        if (isEmpty()) return T();

        Node* temp = topNode;
        T val = temp->data;
        topNode = topNode->next;
        delete temp;
        return val;
    }

    T top() const {
        if (isEmpty()) return T();
        return topNode->data;
    }
};

// Queue
template <typename T>
class Queue {
private:
    struct Node {
        T data;
        Node* next;
    };

    Node* frontNode;
    Node* rearNode;

public:
    Queue() : frontNode(nullptr), rearNode(nullptr) {}

    bool isEmpty() const { return frontNode == nullptr; }

    void enqueue(const T& value) {
        Node* n = new Node{value, nullptr};

        if (!rearNode) {
            frontNode = rearNode = n;
            return;
        }

        rearNode->next = n;
        rearNode = n;
    }

    T dequeue() {
        if (isEmpty()) return T();

        Node* temp = frontNode;
        T val = temp->data;
        frontNode = frontNode->next;

        if (!frontNode) rearNode = nullptr;

        delete temp;
        return val;
    }

    T front() const {
        if (isEmpty()) return T();
        return frontNode->data;
    }
};

#endif