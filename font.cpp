#include "gl.h"
#include "font.h"

#include "textures/font_large.h"
#include "textures/font_large_dat.h"
#include "textures/font_normal.h"
#include "textures/font_normal_dat.h"
#include "textures/font_mono.h"
#include "textures/font_mono_dat.h"

const unsigned int font_count = 3;
static const font fonts[font_count] = {
    {std::string("Normal"), &font_normal, font_normal_dat, font_normal_dat_len},
    {std::string("Monospace"), &font_mono, font_mono_dat, font_mono_dat_len},
    {std::string("Large"), &font_large, font_large_dat, font_large_dat_len},
};

static const font *current_font = fonts + 1;

inline int drawChar(unsigned char c, COLOR color, TEXTURE &tex, unsigned int x, unsigned int y)
{
    if(c == '\t')
        return fontWidth(c);

    uint32_t width = current_font->data[8];
    uint32_t height = current_font->data[12];

    //current_font->data[16] is the char at the top left
    unsigned int pos = c - current_font->data[16];
    if(c < current_font->data[16] || current_font->data_len - 17 < c)
        pos = 0x8D - current_font->data[16];

    const unsigned int cols = current_font->bitmap->width / current_font->data[8];
    unsigned int pos_x = pos % cols;
    unsigned int pos_y = pos / cols;
    pos_x *= width;
    pos_y *= height;

    //Each character has its specific width
    width = current_font->data[c + 17];

    for(unsigned int x1 = 0; x1 < width; x1++)
        for(unsigned int y1 = 0; y1 < height; y1++)
        {
            if(current_font->bitmap->bitmap[pos_x + x1 + (pos_y + y1) * current_font->bitmap->width] == 0xFFFF)
                tex.bitmap[x + x1 + (y + y1) * tex.width] = color;
        }

    return width;
}

unsigned int fontWidth(unsigned char c)
{
    return 'c' == '\t' ? 4*current_font->data[' ' + 17] : (c < current_font->data_len - 17) ? current_font->data[c + 17] : 0;
}

void drawStringCenter(const char *str, COLOR color, TEXTURE &tex, unsigned int x, unsigned int y, e_font f)
{
    const font *old_font = current_font;

    setFont(f);

    drawStringCenter(str, color, tex, x, y);

    current_font = old_font;
}

void drawStringCenter(const char *str, COLOR color, TEXTURE &tex, unsigned int x, unsigned int y)
{
    unsigned int width = 0;
    const char *ptr = str;
    while(*ptr)
        width += fontWidth(*ptr++);

    x -= width / 2;
    drawString(str, color, tex, x, y);
}

void drawString(const char *str, COLOR color, TEXTURE &tex, unsigned int x, unsigned int y, e_font f)
{
    const font *old_font = current_font;

    setFont(f);

    drawString(str, color, tex, x, y);

    current_font = old_font;
}

void drawString(const char *str, COLOR color, TEXTURE &tex, unsigned int x, unsigned int y)
{
    const char *ptr = str;
    const unsigned int start_x = x;
    while(*ptr && x + fontWidth(*ptr) < SCREEN_WIDTH && y + fontHeight() < SCREEN_HEIGHT)
    {
        if(*ptr == '\n')
        {
            x = start_x;
            y += fontHeight();
        }
        else
            x += drawChar(*ptr, color, tex, x, y);

        ++ptr;
    }
}

unsigned int fontHeight()
{
    return current_font->data[12];
}

void setFont(e_font f)
{
    current_font = fonts + static_cast<unsigned int>(f);
}
