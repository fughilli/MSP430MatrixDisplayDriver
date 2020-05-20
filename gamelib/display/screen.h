/*
 * screen.h
 *
 *  Created on: Jan 2, 2015
 *      Author: Kevin
 */

#ifndef SCREEN_H_
#define SCREEN_H_

#ifdef __cplusplus

#include <stdint.h>
#include <stdbool.h>

typedef int32_t screen_coord_t;

enum Color_t
{
    BLACK = 0, WHITE = 1, INVERT = 2, NONE = 3
};

enum Bitmap_mode_t
{
    MODE_OVERWRITE,
    MODE_OVERWRITE_INVERT,
    MODE_BLEND,
    MODE_BLEND_INVERT,
    MODE_MASK,
    MODE_MASK_INVERT,
    MODE_INVERT,
    MODE_INVERT_INVERT
};

enum Font_anchor_pos_t
{
    FAP_TOP_LEFT,
    FAP_TOP_RIGHT,
    FAP_TOP_CENTER,
    FAP_LEFT_CENTER,
    FAP_RIGHT_CENTER,
    FAP_BOTTOM_LEFT,
    FAP_BOTTOM_RIGHT,
    FAP_BOTTOM_CENTER,
    FAP_CENTER
};

enum Font_justification_t
{
    FJ_LEFT,
    FJ_RIGHT,
    FJ_CENTER
};

struct Bitmap_t
{
    const uint8_t * data;
    screen_coord_t w, h;

    Bitmap_t()
    {
        data = nullptr;
        w = h = 0;
    }

    Bitmap_t(const uint8_t * _data, screen_coord_t _w, screen_coord_t _h)
    {
        data = _data;
        w = _w;
        h = _h;
    }
};

struct Point_t
{
    screen_coord_t x, y;

    Point_t()
    {
        x = y = 0;
    }

    Point_t(screen_coord_t _x, screen_coord_t _y)
    {
        x = _x;
        y = _y;
    }

    Point_t operator+(const Point_t& other) const
    {
        Point_t ret;
        ret.x = x + other.x;
        ret.y = y + other.y;
        return ret;
    }

    Point_t operator-(const Point_t& other) const
    {
        Point_t ret;
        ret.x = x - other.x;
        ret.y = y - other.y;
        return ret;
    }

    void operator+=(const Point_t& other)
    {
        x += other.x;
        y += other.y;
    }

    void operator-=(const Point_t& other)
    {
        x -= other.x;
        y -= other.y;
    }

    Point_t operator*(int16_t scalar)
    {
        Point_t ret;
        ret.x = x * scalar;
        ret.y = y * scalar;
        return ret;
    }

    bool operator==(const Point_t& other)
    {
        return (x == other.x && y == other.y);
    }

    const static Point_t zero;
};

struct Rect_t
{
    screen_coord_t x, y, w, h;

    Rect_t()
    {
        x = y = w = h = 0;
    }

    Rect_t(screen_coord_t _x, screen_coord_t _y, screen_coord_t _w, screen_coord_t _h)
    {
        x = _x;
        y = _y;
        w = _w;
        h = _h;
    }

    Rect_t operator+(const Point_t& other) const
    {
        return Rect_t(other.x + x, other.y + y, w, h);
    }
    Rect_t operator-(const Point_t& other) const
    {
        return Rect_t(other.x - x, other.y - y, w, h);
    }
    void operator+=(const Point_t& other)
    {
        x += other.x;
        y += other.y;
    }
    void operator-=(const Point_t& other)
    {
        x -= other.x;
        y -= other.y;
    }

    Point_t upperLeft()
    {
        return Point_t(x,y);
    }
    Point_t lowerLeft()
    {
        return Point_t(x,y+h);
    }
    Point_t upperRight()
    {
        return Point_t(x+w,y);
    }
    Point_t lowerRight()
    {
        return Point_t(x+w,y+h);
    }

    Point_t center()
    {
        return Point_t(x+w/2,y+h/2);
    }

    void centerOn(const Point_t& cent)
    {
        operator+=(cent - center());
    }
};

struct Font_t
{
    const Bitmap_t* bitmap;
    uint8_t char_width;
    uint8_t char_height;
    uint8_t char_stride;
    uint8_t char_kerning;
    uint8_t char_vstride;
    uint8_t char_hstride;
    uint8_t (*char_mapping)(uint8_t);

    Font_t(const Bitmap_t* fb, uint8_t w, uint8_t h, uint8_t s, uint8_t k,
           uint8_t (*m)(uint8_t) = Font_t::default_char_mapping)
    {
        bitmap = fb;
        char_width = w;
        char_height = h;
        char_stride = s;
        char_kerning = k;
        char_mapping = m;
        char_vstride = (4 * char_height) / 3;
        char_hstride = char_width + char_kerning;
    }

    static void textSize(const char* str, uint16_t *w, uint16_t *h)
    {
        if(!w || !h || !str)
            return;

        (*w) = 0;
        (*h) = 0;

        uint16_t maxw = 0;
        char nextChar;
        while(1)
        {
            nextChar = *str;

            if(nextChar == '\n' || nextChar == '\0')
            {
                if(maxw > (*w))
                    (*w) = maxw;
                maxw = 0;
                (*h)++;

                if(nextChar == '\0')
                {
                    break;
                }
            }
            else
                maxw++;
            str++;
        }
    }

    void textSizePixels(const char* str, uint16_t *w, uint16_t *h) const
    {
        if(!w || !h || !str)
            return;

        Font_t::textSize(str, w, h);

        if(*w)
        {
            (*w) *= char_hstride;
            (*w) -= char_kerning;
        }

        if(*h)
        {
            (*h) *= char_vstride;
            (*h) -= (char_vstride - char_height);
        }
    }

    static void textSizeLen(const char* str, uint32_t strlen, uint16_t *w,
                            uint16_t *h)
    {
        if(!w || !h || !str)
            return;

        (*w) = 0;
        (*h) = 0;

        uint16_t maxw = 0;
        char nextChar;
        while(strlen--)
        {
            nextChar = *str;

            if(nextChar == '\n' || nextChar == '\0')
            {
                if(maxw > (*w))
                    (*w) = maxw;
                maxw = 0;
                (*h)++;

                if(nextChar == '\0')
                {
                    break;
                }
            }
            else
                maxw++;
            str++;
        }
    }

    void textSizePixelsLen(const char* str, uint32_t strlen, uint16_t *w,
                           uint16_t *h) const
    {
        if(!w || !h || !str)
            return;

        Font_t::textSizeLen(str, strlen, w, h);

        if(*w)
        {
            (*w) *= char_hstride;
            (*w) -= char_kerning;
        }

        if(*h)
        {
            (*h) *= char_vstride;
            (*h) -= (char_vstride - char_height);
        }
    }

    static uint8_t default_char_mapping(uint8_t idx)
    {
        return idx;
    }
};

class BoxSprite_t;

class Screen {
protected:
    screen_coord_t m_width;
    screen_coord_t m_height;

    screen_coord_t m_bytewidth;

    uint8_t* framebuffer;
public:
    Screen(uint8_t* _fbuf, screen_coord_t _w, screen_coord_t _h);

    // Raster dims
    screen_coord_t getWidth() const;
    screen_coord_t getHeight() const;

    // Pixel access
    void setPixel_nbx(screen_coord_t x, screen_coord_t y, Color_t color);
    void setPixel(screen_coord_t x, screen_coord_t y, Color_t color);
    void setPixel_nbx(const Point_t* p, Color_t color);
    void setPixel(const Point_t* p, Color_t color);

    Color_t getPixel_nbx(screen_coord_t x, screen_coord_t y) const;
    Color_t getPixel(screen_coord_t x, screen_coord_t y) const;
    Color_t getPixel_nbx(const Point_t* p) const;
    Color_t getPixel(const Point_t* p) const;

    // Clear
    void clear(Color_t color);

    // Lines
    void hline(screen_coord_t line, screen_coord_t x1, screen_coord_t x2,
               Color_t color);
    void hline_nbx(screen_coord_t line, screen_coord_t x1, screen_coord_t x2,
                   Color_t color);
    void vline(screen_coord_t col, screen_coord_t y1, screen_coord_t y2,
               Color_t color);
    void vline_nbx(screen_coord_t col, screen_coord_t y1, screen_coord_t y2,
                   Color_t color);

    void line(const Point_t* p1, const Point_t* p2, Color_t color);
    void line_nbx(const Point_t* p1, const Point_t* p2, Color_t color,
                  bool draw_nbx = false);

    // Boxes
    void box(screen_coord_t x1, screen_coord_t y1, screen_coord_t x2,
             screen_coord_t y2, Color_t color, Color_t fcolor);
    void box_nbx(screen_coord_t x1, screen_coord_t y1, screen_coord_t x2,
                 screen_coord_t y2, Color_t color, Color_t fcolor);
    void box(const Rect_t * box, Color_t color, Color_t fcolor);

    // Circles
    void circle(screen_coord_t x, screen_coord_t y, screen_coord_t r,
                Color_t color, Color_t fcolor);
    void circle_nbx(screen_coord_t x, screen_coord_t y, screen_coord_t r,
                    Color_t color, Color_t fcolor);
    void circle(const Point_t * pos, screen_coord_t r, Color_t color,
                Color_t fcolor);

    // Blits
    void bitmap(screen_coord_t x, screen_coord_t y, const uint8_t * bitmap,
                screen_coord_t w, screen_coord_t h, Bitmap_mode_t mode);
    void bitmap_nbx(screen_coord_t x, screen_coord_t y, const uint8_t * bitmap,
                    screen_coord_t w, screen_coord_t h, Bitmap_mode_t mode);
    void bitmap(const Bitmap_t * bmp, const Rect_t * srcRect,
                const Rect_t * dest, Bitmap_mode_t mode);
    void bitmap_nbx(const Bitmap_t * bmp, const Rect_t * srcRect,
                    const Rect_t * dest, Bitmap_mode_t mode);
    void bitmap_adj(const Bitmap_t * bmp, const Rect_t * srcRect,
                    const Rect_t * dest, Bitmap_mode_t mode);

    // Text
    void textOption(const Font_t& font, const Point_t& pt, const char* str,
                    Font_anchor_pos_t fapos, Font_justification_t fjust,
                    Bitmap_mode_t mode);
    void textlen(const Font_t& font, const Point_t& pt, const char* str,
                 uint32_t strlen, Bitmap_mode_t mode);
    void text(const Font_t& font, const Point_t& pt, const char* str,
              Bitmap_mode_t mode);
    void text_plus_offset(const Font_t& font, const Point_t& pt,
                          const char* str, Point_t& (*posmod)(uint32_t charnum),
                          Bitmap_mode_t mode);
    void textBox(const char* str, const Font_t& font, const BoxSprite_t& border,
                 const Point_t& pos, uint8_t margin, Bitmap_mode_t bordermode,
                 Bitmap_mode_t textmode);

    // Transform blits
    void bitmap_scaled_nbx(const Bitmap_t * bmp, const Rect_t * srcRect,
                           const Rect_t * destRect, Bitmap_mode_t mode);
    void bitmap_scaled(const Bitmap_t * bmp, const Rect_t * srcRect,
                       const Rect_t * destRect, Bitmap_mode_t mode);

    // Triangles
    void triangleArea(const Point_t* p1, const Point_t* p2, const Point_t* p3,
                      Color_t fcolor);
    void triangle(const Point_t* p1, const Point_t* p2, const Point_t* p3,
                  Color_t color, Color_t fcolor);
    void triangle_bitmap(const Bitmap_t* bmp, const Point_t* src_p1,
                         const Point_t* src_p2, const Point_t* src_p3,
                         const Point_t* dest_p1, const Point_t* dest_p2,
                         const Point_t* dest_p3);

    // Bounds check
    bool bx(const Point_t* p) const;
    bool bx(screen_coord_t x, screen_coord_t y) const;
    bool bx(const Rect_t* r) const;
};

class DispMath {
public:
    static void lerp(const Point_t * a, const Point_t * b, Point_t * x,
                     float arg);
};

extern Screen screen;

#endif

#endif /* SCREEN_H_ */
