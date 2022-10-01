#pragma once
#ifndef _STRINGENUMERATOR_H
#define _STRINGENUMERATOR_H

#include "IEnumerator.h"
#include <string>

class StringEnumerator: public IEnumerator {
    private:
        std::string _sequence;
        int _index;
        int _size;
    public:
        StringEnumerator(std::string & sequence);
        virtual char Current() const;
        virtual int Index() const;
        virtual bool HasNext() const override;
        virtual bool Next(char & element) override;
};

#endif