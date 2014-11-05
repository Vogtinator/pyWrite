#ifndef FONT_H
#define FONT_H

#include <string>

#include "gl.h"

struct Font {
    Font(std::string n, TEXTURE *bitmap, unsigned char *data, unsigned int data_len) :
        name(n), bitmap(bitmap), data(data), data_len(data_len) {}

    std::string name;
    TEXTURE *bitmap;
    unsigned char *data;
    unsigned int data_len;
};

enum class EFont : unsigned int {
    Normal = 0,
    Monospace = 1,
    Large = 2,
};

extern const unsigned int font_count;

void setFont(EFont f);
EFont getFont();
int drawChar(unsigned char c, COLOR color, TEXTURE &tex, unsigned int x, unsigned int y);
int drawChar(unsigned char c, COLOR color, COLOR background, TEXTURE &tex, unsigned int x, unsigned int y);
void drawStringCenter(const char *str, COLOR color, TEXTURE &tex, unsigned int x, unsigned int y, EFont f);
void drawStringCenter(const char *str, COLOR color, TEXTURE &tex, unsigned int x, unsigned int y);
void drawString(const char *str, COLOR color, TEXTURE &tex, unsigned int x, unsigned int y, EFont f);
void drawString(const char *str, COLOR color, TEXTURE &tex, unsigned int x, unsigned int y);
unsigned int fontHeight();
unsigned int fontHeight(EFont f);
unsigned int fontWidth(unsigned char c);
unsigned int fontWidth(const char *str);
unsigned int fontWidth(const char *str, EFont f);

#endif // FONT_H
