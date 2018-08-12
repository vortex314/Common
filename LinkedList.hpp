#ifndef _LINKED_LIST_HPP_
#define _LINKED_LIST_HPP_

#include <functional>

template <typename T> class Node {
    Node* _next;
    T _object;

  public:
    Node<T>(T object) {
        _object = object;
        _next = 0;
    }
    inline Node<T>* next() { return _next; }

    inline Node<T>* next(Node<T>* n) {
        _next = n;
        return this;
    }
    inline T object() { return _object; }
};

template <typename T> class LinkedList {
    Node<T>* _first;

  public:
    LinkedList<T>() { _first = 0; }

    Node<T>* firstNode() { return _first; };
    Node<T>* nextNode(Node<T>* node) { return node.next(); };
    Node<T>** lastNode() {
        Node<T>* cursor = _first;
        if (_first == 0)
            return &_first;
        while (cursor) {
            if (cursor->_next == 0) {
                return &cursor->_next;
            }
            cursor = cursor->_next;
        }
        return 0;
    }

    void add(T t) {
        if (_first == 0) {
            _first = new Node<T>(t);
        } else {
            Node<T>* cursor = _first;
            while (cursor) {
                if (cursor->next() == 0) {
                    cursor->next(new Node<T>(t));
                    break;
                }
                cursor = cursor->next();
            }
        }
    };

    void forEach(std::function<void(T)> doSomething) {
        Node<T>* cursor;
        for (cursor = firstNode(); (cursor != 0); cursor = cursor->next()) {
            doSomething(cursor->object());
        }
    }

    T findFirst(std::function<bool(T)> match) {
        Node<T>* cursor;
        for (cursor = firstNode(); (cursor != 0); cursor = cursor->next()) {
            if (match(cursor->object()))
                return cursor->object();
        }
        return 0;
    }

    T forEachMatch(std::function<bool(T)> match,
                   std::function<void(T)> doSomething) {
        Node<T>* cursor;
        for (cursor = firstNode(); (cursor != 0); cursor = cursor->next()) {
            if (match(cursor->object()))
                doSomething(cursor->object());
        }
        return 0;
    }
};

#endif
