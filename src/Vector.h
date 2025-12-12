/* 
 * File:   Vector.h
 * Author: Matthew Madrigal
 * Purpose:  Vector Class
 */

#ifndef VECTOR_H
#define VECTOR_H

template <class T>
class Vector{
private:
    T *arr;
    int len;
    int max;
    void resize(int);

public:
    Vector();
    Vector(int);
    ~Vector();
    class OutOfBounds {
        int index;
        int size;
    };
    class ResizeLoosesData {
        int size;
        int nSize;
    };
    T &operator[](const int &);
    int size() const {return size};
    void append(T&);
    void insert(T&, int&);
    T pop(int&);
    void remove(int&);
    
};

#endif /* VECTOR_H */