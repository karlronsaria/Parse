#include "other.h"

void ToStream(IEnumerator & myEnumerator, std::ostream & out) {
    char element;

    while (myEnumerator.Next(element)) {
        out << '[' << element << "]\n";
    }
}

std::string ToReadableWhiteSpace(const std::string & str) {
    std::string outStr;

    for (int i = 0; i < str.length(); ++i) {
        switch (str[i]) {
            case '\t':
                outStr += "\\t";
                break;
            case '\n':
                outStr += "\\n";
                break;
            default:
                outStr += str[i];
                break;
        }
    }

    return outStr;
}
