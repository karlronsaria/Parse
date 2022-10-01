#include "StringEnumerator.h"

StringEnumerator::StringEnumerator(std::string & sequence):
    _sequence(sequence),
    _index(0),
    _size(sequence.size())
    {}

char StringEnumerator::Current() const {
    return _sequence[_index];
}

int StringEnumerator::Index() const {
    return _index;
}

bool StringEnumerator::HasNext() const {
    return _index < _size;
}

bool StringEnumerator::Next(char & element) {
    bool hasNext = HasNext();

    if (hasNext) {
        element = Current();
        _index = _index + 1;
    }

    return hasNext;
}