#pragma once
#ifndef _VECTORBITSET_H
#define _VECTORBITSET_H

#include <bitset>
#include <string>
#include <vector>

template <size_t Bitset_Size = sizeof(int *) * CHAR_BIT>
class vector_bitset {
    private:
        std::vector<std::bitset<Bitset_Size>>
        _list;

        size_t
        _size;

        static std::bitset<Bitset_Size> &
        push_back_bit(
            std::bitset<Bitset_Size> & bits,
            size_t size,
            bool value
        );
    public:
        virtual ~vector_bitset() = default;
        vector_bitset(const vector_bitset &) = default;
        vector_bitset & operator=(const vector_bitset &) = default;
        vector_bitset();
        size_t size() const;
        vector_bitset & push_back(bool value = true);
        bool at(size_t pos) const;
        vector_bitset & set(size_t pos, bool value = true);
        bool operator[](size_t pos) const;
        std::string to_string() const;
};

template <size_t S>
std::bitset<S> &
vector_bitset<S>::push_back_bit(
    std::bitset<S> & bits,
    size_t size,
    bool value
) {
    return bits.set(size, value);
}

template <size_t S>
vector_bitset<S>::vector_bitset():
    _size(0ULL) {}

template <size_t S>
size_t
vector_bitset<S>::size() const {
    return _size;
}

template <size_t S>
vector_bitset<S> &
vector_bitset<S>::push_back(bool value) {
    if (_size == _list.size() * S)
        _list.push_back(std::bitset<S>());

    push_back_bit(
        _list[_size / S],
        _size % S,
        value
    );

    _size++;
    return *this;
}

template <size_t S>
bool
vector_bitset<S>::at(size_t pos) const {
    return _list
        .at(pos / S)
        [pos % S];
}

template <size_t S>
vector_bitset<S> &
vector_bitset<S>::set(size_t pos, bool value) {
    _list
        .at(pos / S)
        .set(pos % S, value);

    return *this;
}

template <size_t S>
bool
vector_bitset<S>::operator[](size_t pos) const {
    return at(pos);
}

template <size_t S>
std::string
vector_bitset<S>::to_string() const {
    std::string temp;

    for (size_t i = 0; i < size(); ++i)
        temp += at(size() - i - 1)
            ? '1' : '0';

    return temp;
}

#endif