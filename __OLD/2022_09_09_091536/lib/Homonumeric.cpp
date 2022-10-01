#include "Homonumeric.h"

Homonumeric Homonumeric::Integer(int value) {
    Homonumeric temp;
    temp.Mode = Mode::INTEGER;
    temp.Payload.Integer = value;
    return temp;
}

Homonumeric Homonumeric::Float(float value) {
    Homonumeric temp;
    temp.Mode = Mode::FLOAT;
    temp.Payload.Float = value;
    return temp;
}

Homonumeric Homonumeric::Boolean(bool value) {
    Homonumeric temp;
    temp.Mode = Mode::BOOLEAN;
    temp.Payload.Boolean = value;
    return temp;
}

Homonumeric Homonumeric::Character(char value) {
    Homonumeric temp;
    temp.Mode = Mode::CHARACTER;
    temp.Payload.Character = value;
    return temp;
}