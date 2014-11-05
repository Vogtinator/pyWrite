#ifndef SYNTAXHIGHLIGHTER_H
#define SYNTAXHIGHLIGHTER_H

#include "gl.h"

class SyntaxHighlighter
{
public:
    virtual ~SyntaxHighlighter() {}

    virtual COLOR highlight(const char *token, unsigned int &length) = 0;
};

class PythonSyntaxHighlighter : public SyntaxHighlighter
{
public:
    COLOR highlight(const char *token, unsigned int &length);
};

extern PythonSyntaxHighlighter python_highlighter;

#endif // SYNTAXHIGHLIGHTER_H
