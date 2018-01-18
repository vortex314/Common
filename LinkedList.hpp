#ifndef _LINKED_LIST_HPP_
#define _LINKED_LIST_HPP_

template <typename T>
class LinkedList
{
  public:
    static T *_first;
    //    uid_t _id;
    //    const char *_label;
    T *_next = 0;

    static T *first() { return _first; };
    T *next() { return _next; };
    static T **lastLink()
    {
        T *cursor = _first;
        if (_first == 0)
            return &_first;
        while (cursor)
        {
            if (cursor->_next == 0)
            {
                return &cursor->_next;
            }
            cursor = cursor->_next;
        }
        return 0;
    }
    void add(T *t)
    {
        //        INFO(" adding 0x%X _first: %X  ", t, _first);
        if (_first == 0)
        {
            _first = t;
        }
        else
        {
            T *cursor = _first;
            //           INFO(" cursor : %X  _next : %X", cursor, cursor->_next);
            while (cursor)
            {
                if (cursor->_next == 0)
                {
                    cursor->_next = t;
                    break;
                }
                cursor = cursor->_next;
            }
        }
    };
};

template <typename T>
T *LinkedList<T>::_first = 0;
#endif