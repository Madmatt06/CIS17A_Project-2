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
    Vector(const Vector<T> &);
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
    int size() const {return size;};
    void append(T&);
    void insert(T&, int&);
    T pop(int&);
    void remove(int&);
    const Vector<T> operator=(const Vector<T> &orig)
    
};

template <class T>
T &Vector<T>::operator[](const int &index) {
    if(index < 0 || index >= len) {
        throw OutOfBounds(index, len);
    }
    return arr[index];
}
template <class T>
void Vector<T>::append(T &app) {
    if(len >= max) {   // If the array is not large enough, replace it with a bigger one
        resize(len + 10);
    }
    arr[len] = app;
    len++;
}
template <class T>
void Vector<T>::insert(T &ins, int &index) {
    if(index > len) {
        throw OutOfBounds(index, len);
    }
    if(len >= max) {   // If the array is not large enough, replace it with a bigger one
        resize(len + 10);
    }
    for(int i = index; i < len; i++) {
        arr[i+1] = arr[i];
    }
    arr[index] = ins;
    len++;
}
template <class T>
T Vector<T>::pop(int &index) {
    T temp = arr[index];
    remove(index);
    return temp;
}
template <class T>
void Vector<T>::remove(int &index) {
    for(int i = index; i < len; i++) {
        arr[i] = arr[i+1];
    }
    len--;
}

template <class T>
void Vector<T>::resize(int nSize) {
    if(len > nSize) {
        throw ResizeLoosesData(len, nSize);
    }
    if(nSize == 0) {
        delete []arr;
        arr = nullptr;
        return;
    }
    max += nSize;
    T arrN = new T[max];
    for(int i = 0; i < len; i++) {
        arr[i] = arrN[i];
    }
    delete []arr;
    arr = arrN;
}

template <class T>
Vector<T>::~Vector() {
    if(arr != nullptr) {
        delete []arr;
    }
}

template <class T>
Vector<T>::Vector() {
    max = 10;
    len = 0;
    arr = new T[max];
}
template <class T>
Vector<T>::Vector(int size) {
    if(size < 0) {
        throw OutOfBounds(size, 0);
    }
    if(size == 0) {
        arr = nullptr;
    } else {
        arr = new T[size];
    }
    len = 0;
    max = size;
}

template <class T>
Vector<T>::Vector(const Vector<T> &orig) {
    len = orig.size()
    max = len;
    arr = new T[len];
    for(int i = 0; i < len; i++) {
        arr[i] = orig[i];
    }
}

template <class T>
const Vector<T> Vector<T>::operator=(const Vector<T> &orig) {
    if(this != &orig) {
        if(arr != nullptr) {
            delete []arr;
        }
        len = orig.size()
        max = len;
        arr = new T[len];
        for(int i = 0; i < len; i++) {
            arr[i] = orig[i];
        }
    }
    return *this;
}


#endif /* VECTOR_H */