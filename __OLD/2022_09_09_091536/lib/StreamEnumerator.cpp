#include "StreamEnumerator.h"

StreamEnumerator::StreamEnumerator(std::istream & sequence):
    _sequence(sequence),
    _index(0) {}

char StreamEnumerator::Current() const {
    return _current;
}

int StreamEnumerator::Index() const {
    return _index;
}

bool StreamEnumerator::HasNext() const {
    return (bool)_sequence;
}

bool StreamEnumerator::Next(char & element) {
    _sequence.get(_current);
    _index = _index + 1;
    element = _current;
    return HasNext();
}