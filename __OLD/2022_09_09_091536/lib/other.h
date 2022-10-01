#pragma once
#ifndef _OTHER_H
#define _OTHER_H
#include "IEnumerator.h"
#include <iostream>
#include <sstream>
#include <string>

void ToStream(IEnumerator & myEnumerator, std::ostream & out);
std::string ToReadableWhiteSpace(const std::string & str);

template <typename T>
std::string ToString(T value) {
    std::ostringstream outss;
    outss << value;
    return outss.str();
}

#endif