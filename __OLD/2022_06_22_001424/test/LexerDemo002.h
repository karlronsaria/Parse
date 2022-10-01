#pragma once
#ifndef _LEXERDEMO002_H
#define _LEXERDEMO002_H

#include "../src/lexer.h"

class LexerDemo002 {
    public:
        static int EscapeC(Lexer &);
        static int NextTokenC(Lexer &);
};

#endif