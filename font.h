#ifndef FONT_H
#define FONT_H

#include <string>

#include "gl.h"

struct font {
    font(std::string n, TEXTURE *bitmap, unsigned char *data, unsigned int data_len) :
        name(n), bitmap(bitmap), data(data), data_len(data_len) {}

    std::string name;
    TEXTURE *bitmap;
    unsigned char *data;
    unsigned int data_len;
};

enum struct e_font {
    NORMAL = 0,
    MONOSPACE = 1,
    LARGE = 2,
};

extern const unsigned int font_count;

void setFont(e_font f);
int drawChar(unsigned char c, COLOR color, TEXTURE &tex, unsigned int x, unsigned int y);
void drawStringCenter(const char *str, COLOR color, TEXTURE &tex, unsigned int x, unsigned int y, e_font f);
void drawStringCenter(const char *str, COLOR color, TEXTURE &tex, unsigned int x, unsigned int y);
void drawString(const char *str, COLOR color, TEXTURE &tex, unsigned int x, unsigned int y, e_font f);
void drawString(const char *str, COLOR color, TEXTURE &tex, unsigned int x, unsigned int y);
unsigned int fontHeight();
unsigned int fontWidth(unsigned char c);

#endif // FONT_H
