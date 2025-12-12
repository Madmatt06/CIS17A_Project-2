/* 
 * File:   Vector.cpp
 * Author: Matthew Madrigal
 * Created on 12th December, 2025, 12:42 pm,
 * Purpose:  To define the functions for the Vector class
 */

//System Libraries

//User Libraries
#include "Vector.h"

template <class T>
T &Vector<T>::operator[](const int &index) {
    if(index < 0 || index >= len) {
        throw OutOfBounds(index, len);
    }
    return arr[index];
}
template <class T>
void Vector<T>::append(T &app) {
    if(index > len) [
        throw OutOfBounds(index, len);
    ]
    if(len >= max) {   // If the array is not large enough, replace it with a bigger one
        resize(len + 10);
    }
    arr[len] = app;
    len++;
}
template <class T>
void Vector<T>::insert(T &ins, int &index) {
    if(index > len) [
        throw OutOfBounds(index, len);
    ]
    if(len >= max) {   // If the array is not large enough, replace it with a bigger one
        resize(len + 10);
    }
    for(int i = index; i < len; i++) {
        arr[i+1] = arr[i];
    }
    arr[index] = card;
    len++;
}
template <class T>
T Vector<T>::pop(int &index) {
    T temp = arr[i]
    remove(i);
    return temp;
}
template <class T>
void Vector<T>::remove(int &index) {
    for(int i = index; i < size; i++) {
        arr[i] = arr[i+1];
    }
    size--;
}

template <class T>
void Vector<T>::resize(int nSize) {
    if(len > nSize) {
        throw ResizeLoosesData(len, nSize)
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
    if(int < 0) {
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
