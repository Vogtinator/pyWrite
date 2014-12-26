#include "syntaxhighlighter.h"

#include <string>
#include <algorithm>

PythonSyntaxHighlighter python_highlighter;

/* These lists are from http://agorbatchev.typepad.com/pub/sh/3_0_83/scripts/shBrushPython.js,
 * part of Alex Gorbatchev's SyntaxHighlighter suite. */

static std::string keywords = "and assert break class continue def del elif else "
                                "except exec finally for from global if import in is "
                                "lambda not or pass print raise return try yield while ";

static std::string funcs = "abs all any apply basestring bin bool buffer callable "
                            "chr classmethod cmp coerce compile complex delattr dict dir "
                            "divmod enumerate eval execfile file filter float format frozenset "
                            "getattr globals hasattr hash help hex id input int intern "
                            "isinstance issubclass iter len list locals long map max min next "
                            "object oct open ord pow print property range raw_input reduce "
                            "reload repr reversed round set setattr slice sorted staticmethod "
                            "str sum super tuple type type unichr unicode vars xrange zip ";

static std::string special = "None True False self cls class_ ";

constexpr COLOR fromRGB(unsigned int rgb24)
{
    return ((rgb24 >> 16) & 0b11111000) << 8 | ((rgb24 >> 8) & 0b11111100) << 3 | (rgb24 & 0b11111000) >> 3;
}

static const COLOR keyword_color = fromRGB(0x7f6d61),
                    func_color = fromRGB(0x81cef9),
                    special_color = fromRGB(0xebdb8d);

const char *strchrnul(const char *t, char s)
{
    const char *ret = strchr(t, s);
    return ret ? ret : t + strlen(t);
}

COLOR PythonSyntaxHighlighter::highlight(const char *token, unsigned int &length)
{
    //Tokenize the input
    const char *tok_end = strchrnul(token, ' ');
    std::initializer_list<char> separator = {'\t', '\n', '(', ')', ':', ','};
    for(char s : separator)
    {
        const char *tok_end2 = strchrnul(token, s);
        if(tok_end2 < tok_end)
            tok_end = tok_end2;
    }

    length = tok_end - token;

    //Seperator following seperator
    if(length == 0)
    {
        length = 1;
        return 0x0000;
    }

    size_t pos = keywords.find(token, 0, length);
    if(pos != std::string::npos && keywords[pos + length] == ' ')
        return keyword_color;

    pos = funcs.find(token, 0, length);
    if(pos != std::string::npos && funcs[pos + length] == ' ')
        return func_color;

    pos = special.find(token, 0, length);
    if(pos != std::string::npos && special[pos + length] == ' ')
        return special_color;

    return 0x0000;
}
