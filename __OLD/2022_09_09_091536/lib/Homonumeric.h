#pragma once
#ifndef _HOMONUMERIC_H
#define _HOMONUMERIC_H

class Homonumeric {
    public:
        enum Mode {
            INTEGER,
            FLOAT,
            BOOLEAN,
            CHARACTER
        };

        union Definition {
            int Integer;
            float Float;
            bool Boolean;
            char Character;
        };

        Mode Mode;
        Definition Payload;

        static Homonumeric Integer(int value = 0);
        static Homonumeric Float(float value = 0.0);
        static Homonumeric Boolean(bool value = false);
        static Homonumeric Character(char);
};

#endif