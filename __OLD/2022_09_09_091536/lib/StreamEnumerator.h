#pragma once
#ifndef _STREAMENUMERATOR_H
#define _STREAMENUMERATOR_H

#include "IEnumerator.h"
#include <iostream>

class StreamEnumerator: public IEnumerator {
    private:
        std::istream & _sequence;
        char _current;
        int _index;
    public:
        StreamEnumerator(std::istream & sequence);
        virtual char Current() const;
        virtual int Index() const;
        virtual bool HasNext() const override;
        virtual bool Next(char & element) override;
};

#endif