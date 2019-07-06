/*
 * summer.cpp
 *
 *  Created on: 2018年1月23日
 *      Author: Administrator
 */

#include "IntArray.h"

IntArray::IntArray(int len) {
    m_length = len;
}

bool IntArray::construct() {   //第二阶段构造
    bool ret = true;

    m_pointer = new int[m_length]; //在第二阶段构造中申请系统资源

    if (m_pointer) {
        for (int i = 0; i < m_length; i++) {
            m_pointer[i] = 0;
        }
    } else {
        ret = false;
    }

    return ret;
}

IntArray* IntArray::NewInstance(int length) {
    IntArray* ret = new IntArray(length);

    if (!(ret && ret->construct())) {    //在静态成员函数中调用二阶构造函数
        delete ret;              //如果申请系统资源失败那么构造就失败了，要删除之前创建的对象
        ret = 0;
    }

    return ret;
}

int IntArray::length() {
    return m_length;
}

bool IntArray::get(int index, int& value) {
    bool ret = (0 <= index) && (index < length());

    if (ret) {
        value = m_pointer[index];
    }

    return ret;
}

bool IntArray::set(int index, int value) {
    bool ret = (0 <= index) && (index < length());

    if (ret) {
        m_pointer[index] = value;
    }

    return ret;
}

IntArray::~IntArray() {
    delete[] m_pointer;
}




