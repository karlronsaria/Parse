#pragma once
#ifndef _IENUMERATOR_H
#define _IENUMERATOR_H

class IEnumerator {
    public:
        virtual ~IEnumerator() = default;
        virtual int Index() const = 0;
        virtual bool HasNext() const = 0;
        virtual bool Next(char & element) = 0;
};

#endif