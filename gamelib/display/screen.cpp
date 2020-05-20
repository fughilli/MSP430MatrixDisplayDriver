/*
 * screen.c
 *
 *  Created on: Jan 2, 2015
 *      Author: Kevin
 */

#ifdef __cplusplus

#include "screen.h"

// For NULL:
#include <stdlib.h>

const Point_t Point_t::zero = Point_t();

Screen::Screen(uint8_t* _fbuf, screen_coord_t _w, screen_coord_t _h)
{
    framebuffer = _fbuf;

    if(m_width % 8)
        m_width = (((m_width / 8) + 1)*8);

    m_width = _w;
    m_height = _h;

    m_bytewidth = m_width/8;
}

void Screen::clear(Color_t color)
{
    screen_coord_t i = 0;
    if (color != INVERT)
    {
        for (; i < (m_bytewidth * m_height); i++)
        {
            framebuffer[i] = ((color == BLACK) ? (0x00) : (0xFF));
        }
    }
    else
    {
        for (; i < (m_bytewidth * m_height); i++)
        {
            framebuffer[i] = ~framebuffer[i];
        }
    }
}

Color_t Screen::getPixel_nbx(screen_coord_t x, screen_coord_t y) const
{
    return ((framebuffer[(x / 8) + y * (m_bytewidth)] & (0x80 >> (x % 8))) ?
            (WHITE) : (BLACK));
}

Color_t Screen::getPixel(screen_coord_t x, screen_coord_t y) const
{
    if (x < 0 || x >= m_width || y < 0 || y >= m_height)
        return NONE;

    return getPixel_nbx(x, y);
}

void Screen::setPixel_nbx(screen_coord_t x, screen_coord_t y, Color_t color)
{

    switch (color)
    {
    case BLACK:
        framebuffer[(x / 8) + y * (m_bytewidth)] &= ~(0x80 >> (x % 8));
        break;
    case WHITE:
        framebuffer[(x / 8) + y * (m_bytewidth)] |= (0x80 >> (x % 8));
        break;
    case INVERT:
        framebuffer[(x / 8) + y * (m_bytewidth)] ^= (0x80 >> (x % 8));
        break;
    default:
        return;
    }
}

void Screen::setPixel(screen_coord_t x, screen_coord_t y, Color_t color)
{
    if (x < 0 || x >= m_width || y < 0 || y >= m_height)
        return;

    setPixel_nbx(x, y, color);
}

void Screen::setPixel_nbx(const Point_t* p, Color_t color)
{
    setPixel_nbx(p->x, p->y, color);
}

void Screen::setPixel(const Point_t* p, Color_t color)
{
    setPixel(p->x, p->y, color);
}

Color_t Screen::getPixel_nbx(const Point_t* p) const
{
    return getPixel_nbx(p->x, p->y);
}

Color_t Screen::getPixel(const Point_t* p) const
{
    return getPixel(p->x, p->y);
}


screen_coord_t Screen::getWidth() const
{
    return m_width;
}

screen_coord_t Screen::getHeight() const
{
    return m_height;
}

void Screen::bitmap(screen_coord_t x, screen_coord_t y, const uint8_t * _bitmap,
                    screen_coord_t w, screen_coord_t h, Bitmap_mode_t mode)
{
    Rect_t src;
    Rect_t dest;
    Bitmap_t bmp;

    dest.x = x;
    dest.y = y;

    src.x = 0;
    src.y = 0;
    src.w = w;
    src.h = h;

    bmp.data = _bitmap;
    bmp.w = w;
    bmp.h = h;

    bitmap(&bmp, &src, &dest, mode);
}

void Screen::bitmap_nbx(screen_coord_t x, screen_coord_t y,
                        const uint8_t * bitmap, screen_coord_t w,
                        screen_coord_t h, Bitmap_mode_t mode)
{
    Rect_t src;
    Rect_t dest;
    Bitmap_t bmp;

    dest.x = x;
    dest.y = y;

    src.x = 0;
    src.y = 0;
    src.w = w;
    src.h = h;

    bmp.data = bitmap;
    bmp.w = w;
    bmp.h = h;

    bitmap_nbx(&bmp, &src, &dest, mode);
}

void Screen::bitmap_adj(const Bitmap_t * _bmp, const Rect_t * _srcRect,
                        const Rect_t * _dest, Bitmap_mode_t mode)
{
    Bitmap_t bmp = *_bmp;
    Rect_t srcRect = *_srcRect;
    Rect_t dest = *_dest;

    if (bmp.w <= 0 || bmp.h <= 0)
    {
        return;
    }

    if (srcRect.w < 0)
        srcRect.w = bmp.w;
    if (srcRect.h < 0)
        srcRect.h = bmp.h;

    if (srcRect.x < 0)
    {
        srcRect.w += srcRect.x;
        srcRect.x = 0;
    }

    if (srcRect.y < 0)
    {
        srcRect.h += srcRect.y;
        srcRect.y = 0;
    }

    if (dest.x < 0)
    {
        srcRect.w += dest.x;
        srcRect.x -= dest.x;
        dest.x = 0;
    }

    if (dest.y < 0)
    {
        srcRect.h += dest.y;
        srcRect.y -= dest.y;
        dest.y = 0;
    }

    // If this condition is satisfied, nothing needs to happen...
    if (srcRect.w <= 0 || srcRect.h <= 0 || dest.x > m_width ||
        dest.y > m_height)
    {
        return;
    }

    // Check bounds on source rectangle
    if ((srcRect.h + srcRect.y) > bmp.h)
    {
        srcRect.h = bmp.h - srcRect.y;
    }
    if ((srcRect.w + srcRect.x) > bmp.w)
    {
        srcRect.w = bmp.w - srcRect.x;
    }

    // Check bounds on destination point
    if ((dest.y + srcRect.h) > m_height)
    {
        srcRect.h = m_height - dest.y;
    }
    if ((dest.x + srcRect.w) > m_width)
    {
        srcRect.w = m_width - dest.x;
    }

    bitmap_nbx(&bmp, &srcRect, &dest, mode);
}

void Screen::bitmap(const Bitmap_t * bmp, const Rect_t * srcRect,
                    const Rect_t * dest, Bitmap_mode_t mode)
{
    // If this condition is satisfied, nothing needs to happen...
    if (srcRect->w == 0 || srcRect->h == 0)
    {
        return;
    }

    if (bmp->w <= 0 || bmp->h <= 0)
    {
        return;
    }

    // Check bounds on source rectangle
    if (((srcRect->h + srcRect->y) > bmp->h)
            || ((srcRect->w + srcRect->x) > bmp->w) || srcRect->x < 0
            || srcRect->y < 0)
    {
        return;
    }

    // Check bounds on destination point
    if (dest->x < 0 || dest->y < 0
            || ((dest->x + srcRect->w) > m_width)
            || ((dest->y + srcRect->h) > m_height))
    {
        return;
    }

    bitmap_nbx(bmp, srcRect, dest, mode);
}

void Screen::bitmap_nbx(const Bitmap_t * bmp, const Rect_t * srcRect,
                        const Rect_t * dest, Bitmap_mode_t mode)
{
    /*
     * How wide is the bitmap in bytes?
     */
    int32_t bmpByteWidth = (bmp->w % 8) ? (bmp->w / 8 + 1) : (bmp->w / 8);

    /*
     * Generate the destination left and right side bitmasks (for preserving
     * the old contents around the bitmap)
     */
    uint8_t dest_lEdgeMask = (uint16_t) (0xFF00) >> (dest->x % 8);
    uint8_t dest_rEdgeMask = 0xFF >> ((srcRect->w + dest->x) % 8);

    if (dest_rEdgeMask == 0xFF)
    {
        dest_rEdgeMask = 0;
    }

    /*
     * How many bits is the left edge of the src bitmap offset from a byte
     * boundary?
     */
    int8_t srcBitOffset = srcRect->x % 8;
    int8_t destBitOffset = dest->x % 8;

    // Offset from bmp->data for the first byte in each row
    int32_t bmpRowOffset = bmpByteWidth * srcRect->y;

    // Offset from framebuffer for the first byte in each row
    int32_t fbRowOffset = m_bytewidth * dest->y;

    // Indices; row, and byte in src
    int32_t srcRectRowIndex, bmpColByteIndex;

    // Offset from left edge of bmp to first byte of srcRect, in bytes
    int32_t bmpFirstByteRowOffset = srcRect->x / 8;

    // Offset from left edge of framebuffer to first byte of dest, in bytes
    int32_t fbFirstByteRowOffset = dest->x / 8;

    int32_t numDestCols = (((dest->x + srcRect->w) % 8) ?
                           ((dest->x + srcRect->w) / 8 + 1) :
                           ((dest->x + srcRect->w) / 8)) - (dest->x / 8);

    /*
     * Place to store two consecutive bytes during copy operation (to solve
     * spanning problem)
     */
    uint32_t stagingArea;

    // only 8 bits of contents are important
    uint8_t remainder;

    // Iterate through all of the rows in srcRect
    for (srcRectRowIndex = 0; srcRectRowIndex < srcRect->h; srcRectRowIndex++)
    {
        remainder = 0;
        // Iterate through all of the byte columns in srcRect
        for (bmpColByteIndex = 0; (bmpColByteIndex < numDestCols);
                bmpColByteIndex++)
        {
            uint32_t srcIndex, destIndex;
            srcIndex = bmpRowOffset + bmpFirstByteRowOffset + bmpColByteIndex;
            destIndex = fbRowOffset + fbFirstByteRowOffset + bmpColByteIndex;
            stagingArea = bmp->data[srcIndex];
            stagingArea <<= 8;

            // If there is another byte
            if (bmpColByteIndex + 1 != bmpRowOffset)
                stagingArea |= bmp->data[srcIndex + 1];

            if (destBitOffset > srcBitOffset)
                stagingArea >>= destBitOffset - srcBitOffset;
            else
                stagingArea <<= srcBitOffset - destBitOffset;

            uint8_t tempRemainder = stagingArea;

            stagingArea >>= 8;

            stagingArea |= remainder;

            remainder = tempRemainder;

            if (mode == MODE_OVERWRITE_INVERT || mode == MODE_BLEND_INVERT ||
                mode == MODE_MASK_INVERT || mode == MODE_INVERT_INVERT)
                stagingArea = ~stagingArea;

            // Compensate for target regions less than 8 bits wide
            if (srcRect->w < 8 && (((dest->x + srcRect->w) % 8) >= srcRect->w))
                    dest_lEdgeMask |= dest_rEdgeMask;

            switch (mode)
            {
            case MODE_OVERWRITE:
            case MODE_OVERWRITE_INVERT:
                if (bmpColByteIndex == 0)
                {
                    framebuffer[destIndex] = (framebuffer[destIndex] &
                                              dest_lEdgeMask) |
                                             (stagingArea & ~dest_lEdgeMask);
                }
                else if (bmpColByteIndex == numDestCols - 1)
                {
                    framebuffer[destIndex] = (framebuffer[destIndex] &
                                              dest_rEdgeMask) |
                                             (stagingArea & ~dest_rEdgeMask);
                }
                else
                {
                    framebuffer[destIndex] = (stagingArea & 0xFF);
                }
                break;
            case MODE_BLEND:
            case MODE_BLEND_INVERT:
                if (bmpColByteIndex == 0)
                {
                    framebuffer[destIndex] = (framebuffer[destIndex]) |
                                             (stagingArea & ~dest_lEdgeMask);
                }
                else if (bmpColByteIndex == numDestCols - 1)
                {
                    framebuffer[destIndex] = (framebuffer[destIndex]) |
                                             (stagingArea & ~dest_rEdgeMask);
                }
                else
                {
                    framebuffer[destIndex] |= (stagingArea & 0xFF);
                }
                break;
            case MODE_MASK:
            case MODE_MASK_INVERT:
                if (bmpColByteIndex == 0)
                {
                    framebuffer[destIndex] =
                        (framebuffer[destIndex])
                        & ((stagingArea & ~dest_lEdgeMask)
                           | dest_lEdgeMask);
                }
                else if (bmpColByteIndex == numDestCols - 1)
                {
                    framebuffer[destIndex] =
                        (framebuffer[destIndex])
                        & ((stagingArea & ~dest_rEdgeMask)
                           | dest_rEdgeMask);
                }
                else
                {
                    framebuffer[destIndex] &= (stagingArea & 0xFF);
                }
                break;
            case MODE_INVERT:
            case MODE_INVERT_INVERT:
                if (bmpColByteIndex == 0)
                {
                    framebuffer[destIndex] = (framebuffer[destIndex]) ^
                                             (stagingArea & ~dest_lEdgeMask);
                }
                else if (bmpColByteIndex == numDestCols - 1)
                {
                    framebuffer[destIndex] = (framebuffer[destIndex]) ^
                                             (stagingArea & ~dest_rEdgeMask);
                }
                else
                {
                    framebuffer[destIndex] ^= (stagingArea & 0xFF);
                }
                break;
            }
        }
        fbRowOffset += m_bytewidth;
        bmpRowOffset += bmpByteWidth;
    }
}

/*
 * Advances the ptr pointed to by st to the next line in str and sets len
 * accordingly; if st hits end of line, len = 0 and this function returns false,
 * otherwise returns true.
 */
static bool nextline(const char* str, char const** st, uint32_t* len)
{
    const char* idx = (*st);
    (*len) = 0;
    // While the current character is not a newline, nor end of line
    while((*idx) != '\n' && (*idx))
    {
        // Increment the character index
        idx++;
    }
    // We have hit either newline or end of line

    // If we hit end of line, return false
    if(!(*idx))
    {
        return false;
    }

    // Advance past the newline
    idx++;
    (*st) = idx;

    // If we hit a newline, find the end
    while(idx[(*len)] != '\n' && idx[(*len)])
    {
        (*len)++;
    }
    return true;
}

static uint32_t linelen(const char* str)
{
    uint32_t ret = 0;
    while(str[ret] != '\n' && str[ret])
    {
        ret++;
    }
    return ret;
}

void Screen::textOption(const Font_t& font,
                const Point_t& pt,
                const char* str,
                Font_anchor_pos_t fapos,
                Font_justification_t fjust,
                Bitmap_mode_t mode)
{
    if(*str == 0)
        return;

    uint16_t maxw, maxh;
    int16_t linewidth, lineheight;
    font.textSizePixels(str, &maxw, &maxh);

    Point_t drawpt = pt;

    switch(fapos)
    {
    case FAP_TOP_LEFT:
    {

    }
    break;
    case FAP_TOP_RIGHT:
    {
        drawpt -= Point_t(maxw, 0);
    }
    break;
    case FAP_TOP_CENTER:
    {
        drawpt -= Point_t(maxw/2, 0);
    }
    break;
    case FAP_LEFT_CENTER:
    {
        drawpt -= Point_t(0, maxh/2);
    }
    break;
    case FAP_RIGHT_CENTER:
    {
        drawpt -= Point_t(maxw, maxh/2);
    }
    break;
    case FAP_BOTTOM_LEFT:
    {
        drawpt -= Point_t(0, maxh);
    }
    break;
    case FAP_BOTTOM_RIGHT:
    {
        drawpt -= Point_t(maxw, maxh);
    }
    break;
    case FAP_BOTTOM_CENTER:
    {
        drawpt -= Point_t(maxw/2, maxh);
    }
    break;
    case FAP_CENTER:
    {
        drawpt -= Point_t(maxw/2, maxh/2);
    }
    break;
    }

    // If the text is left justified, just draw it
    if(fjust == FJ_LEFT)
    {
        screen.text(font, drawpt, str, mode);
        return;
    }
    // If the text is right justified or center justified,
    // shift the drawpt right
    if(fjust == FJ_RIGHT || fjust == FJ_CENTER)
    {
        if(fjust == FJ_RIGHT)
            drawpt += Point_t(maxw, 0);
        if(fjust == FJ_CENTER)
            drawpt += Point_t(maxw/2, 0);

        const char* lstart = str;
        uint32_t len = linelen(str);
        lineheight = 0;

        do
        {
            linewidth = (len * font.char_hstride) - font.char_kerning;
            if(fjust == FJ_RIGHT)
                screen.textlen(font, drawpt + Point_t(-linewidth, lineheight), lstart, len, mode);
            if(fjust == FJ_CENTER)
                screen.textlen(font, drawpt + Point_t(-(linewidth/2), lineheight), lstart, len, mode);
            lineheight += font.char_vstride;
        }
        while(nextline(str, &lstart, &len));
        return;
    }
}

void Screen::text(const Font_t& font, const Point_t& pt, const char* str, Bitmap_mode_t mode)
{
    textlen(font, pt, str, 0xFFFFFFFF, mode);
}

void Screen::textlen(const Font_t& font, const Point_t& pt, const char* str, uint32_t strlen, Bitmap_mode_t mode)
{
    Rect_t srcRect, destRect;

    srcRect.w = destRect.w = font.char_width;
    srcRect.h = destRect.h = font.char_height;

    srcRect.y = 0;
    destRect.y = pt.y;
    destRect.x = pt.x;

    char c;
    while((c = *str) && (strlen--))
    {
        if(c == '\n')
        {
            destRect.y += font.char_vstride;
            destRect.x = pt.x;
            str++;
            continue;
        }
        uint8_t idx;
        if((idx = font.char_mapping(c)))
        {
            srcRect.x = (idx*font.char_stride);
            bitmap_adj(font.bitmap, &srcRect, &destRect, mode);
        }
        destRect.x += (font.char_width + font.char_kerning);
        str++;
    }
}

void Screen::text_plus_offset(const Font_t& font, const Point_t& pt, const char* str, Point_t& (*posmod)(uint32_t charnum), Bitmap_mode_t mode)
{
    Rect_t srcRect, destRect, modDestRect;

    srcRect.w = destRect.w = font.char_width;
    srcRect.h = destRect.h = font.char_height;



    srcRect.y = 0;
    destRect.y = pt.y;
    destRect.x = pt.x;

    uint32_t cnum = 0;

    char c;
    while((c = *str))
    {
        if(c == '\n')
        {
            destRect.y += (font.char_height * 4 / 3);
            destRect.x = pt.x;
            cnum = 0;
            str++;
            continue;
        }
        uint8_t idx;
        if((idx = font.char_mapping(c)))
        {
            Point_t& pos = posmod(cnum);

            modDestRect.x = destRect.x + pos.x;
            modDestRect.y = destRect.y + pos.y;

            srcRect.x = (idx*font.char_stride);
            bitmap_adj(font.bitmap, &srcRect, &modDestRect, mode);
        }
        destRect.x += (font.char_width + font.char_kerning);
        str++;
        cnum++;
    }
}

void Screen::hline_nbx(screen_coord_t line, screen_coord_t x1, screen_coord_t x2, Color_t color)
{
    uint8_t l_mask, r_mask;

    if(x1 < 0)
        x1 = 0;
    if(x2 >= m_width)
        x2 = m_width - 1;

    if (x1 == x2)
        setPixel_nbx(x1, line, color);
    else
    {
        l_mask = 0xff >> (x1 & 7);
        x1 = (x1 + m_width * line) / 8;
        r_mask = ~(0xff >> ((x2 & 7) + 1));
        x2 = (x2 + m_width * line) / 8;
        if (x1 == x2)
        {
            l_mask = l_mask & r_mask;
            r_mask = 0;
        }
        switch (color)
        {
        case WHITE:
            framebuffer[x1++] |= l_mask;
            while (x1 < x2)
                framebuffer[x1++] = 0xff;
            framebuffer[x1] |= r_mask;
            break;
        case BLACK:
            framebuffer[x1++] &= ~l_mask;
            while (x1 < x2)
                framebuffer[x1++] = 0;
            framebuffer[x1] &= ~r_mask;
            break;
        case INVERT:
            framebuffer[x1++] ^= l_mask;
            while (x1 < x2)
                framebuffer[x1++] ^= 0xff;
            framebuffer[x1] ^= r_mask;
            break;
        case NONE:
            return;
        }
    }
}

void Screen::hline(screen_coord_t line, screen_coord_t x1, screen_coord_t x2, Color_t color)
{

    if (x1 > x2)
    {
        screen_coord_t temp = x1;
        x1 = x2;
        x2 = temp;
    }

    if (x1 >= m_width || x2 < 0 || line < 0 || line >= m_height)
        return;

    hline_nbx(line, x1, x2, color);
}

void Screen::vline_nbx(screen_coord_t col, screen_coord_t y1, screen_coord_t y2, Color_t color)
{
    uint8_t v_mask = 0x80 >> (col % 8);

    screen_coord_t rowOffset = col / 8;

    while (y1 <= y2)
    {
        switch (color)
        {
        case WHITE:
            framebuffer[y1 * m_bytewidth + rowOffset] |= v_mask;
            break;
        case BLACK:
            framebuffer[y1 * m_bytewidth + rowOffset] &= ~v_mask;
            break;
        case INVERT:
            framebuffer[y1 * m_bytewidth + rowOffset] ^= v_mask;
            break;
        case NONE:
            return;
        }
        y1++;
    }
}

void Screen::vline(screen_coord_t col, screen_coord_t y1, screen_coord_t y2, Color_t color)
{
    if (y1 > y2)
    {
        screen_coord_t temp = y1;
        y1 = y2;
        y2 = temp;
    }

    if (y1 < 0 || y2 > m_height || col < 0 || col >= m_width
            || color == NONE)
        return;

    vline_nbx(col, y1, y2, color);
}

void Screen::line(const Point_t* p1, const Point_t* p2, Color_t color)
{
    if(!(bx(p1) && bx(p2)))
        return;

    line_nbx(p1, p2, color, true);
}

void Screen::line_nbx(const Point_t* p1, const Point_t* p2, Color_t color, bool draw_nbx)
{
    if ((p1->x == p2->x) && (p1->y == p2->y))
    {
        setPixel_nbx(p1, color);
    }
    else if (p1->x == p2->x)
    {
        vline(p1->x, p1->y, p2->y, color);
    }
    else if (p1->y == p2->y)
    {
        hline(p1->y, p1->x, p2->x, color);
    }
    else
    {
        screen_coord_t wx_1 = p1->x;
        screen_coord_t wx_2 = p2->x;
        screen_coord_t wy_1 = p1->y;
        screen_coord_t wy_2 = p2->y;

        screen_coord_t temp;

        bool flag = abs(wy_2 - wy_1) > abs(wx_2 - wx_1);
        if (flag)
        {
            temp = wx_1;
            wx_1 = wy_1;
            wy_1 = temp;

            temp = wx_2;
            wx_2 = wy_2;
            wy_2 = temp;
        }

        if (wx_1 > wx_2)
        {
            temp = wx_1;
            wx_1 = wx_2;
            wx_2 = temp;

            temp = wy_1;
            wy_1 = wy_2;
            wy_2 = temp;
        }

        screen_coord_t dx = wx_2 - wx_1;
        screen_coord_t dy = abs(wy_2 - wy_1);
        screen_coord_t err = dx / 2;
        screen_coord_t ystep;

        if (wy_1 < wy_2) ystep = 1;
        else ystep = -1;

        for (; wx_1<=wx_2; wx_1++)
        {
            if (flag)
                if(draw_nbx)
                    setPixel_nbx(wy_1, wx_1, color);
                else
                    setPixel(wy_1, wx_1, color);
            else if(draw_nbx)
                setPixel_nbx(wx_1, wy_1, color);
            else
                setPixel(wx_1, wy_1, color);

            err -= dy;
            if (err < 0)
            {
                wy_1 += ystep;
                err += dx;
            }
        }
    }
}

void Screen::box(screen_coord_t x1, screen_coord_t y1, screen_coord_t x2, screen_coord_t y2, Color_t color,
                 Color_t fcolor)
{
    if (x1 > x2)
    {
        screen_coord_t temp = x1;
        x1 = x2;
        x2 = temp;
    }

    if (x1 < 0 || x1 > m_width)
        return;

    if (y1 > y2)
    {
        screen_coord_t temp = y1;
        y1 = y2;
        y2 = temp;
    }

    if (y1 < 0 || y1 > m_height)
        return;

    box_nbx(x1, y1, x2, y2, color, fcolor);
}

void Screen::box_nbx(screen_coord_t x1, screen_coord_t y1, screen_coord_t x2, screen_coord_t y2, Color_t color, Color_t fcolor)
{
    if (color != NONE)
    {
        hline_nbx(y1, x1, x2, color);
        if (y1 != y2)
        {
            hline_nbx(y2, x1, x2, color);
        }

        if (y2 - y1 > 2)
        {
            vline_nbx(x1, y1 + 1, y2 - 1, color);
            if (x1 != x2)
            {
                vline_nbx(x2, y1 + 1, y2 - 1, color);
            }
        }
    }
    if (fcolor != NONE)
    {
        while (y1 <= y2)
        {
            hline_nbx(y1, x1, x2, fcolor);
            y1++;
        }
    }
}

void Screen::box(const Rect_t * _box, Color_t color, Color_t fcolor)
{
    if (_box != NULL)
    {
        box(_box->x, _box->y, _box->x + _box->w - 1, _box->y + _box->h - 1, color, fcolor);
    }
}

void Screen::circle_nbx(screen_coord_t x, screen_coord_t y, screen_coord_t r, Color_t color,
                        Color_t fcolor)
{
    int32_t f = 1 - r;
    int32_t ddF_x = 1;
    int32_t ddF_y = -2 * r;
    int32_t x0 = 0;
    int32_t y0 = r;
    uint8_t pyy = y0, pyx = x0;

    //there is a fill color
    if (fcolor != NONE)
        hline_nbx(y, x - r, x + r, fcolor);

    setPixel_nbx(x, y + r, color);
    setPixel_nbx(x, y - r, color);
    setPixel_nbx(x + r, y, color);
    setPixel_nbx(x - r, y, color);

    while (x0 < y0)
    {
        if (f >= 0)
        {
            y0--;
            ddF_y += 2;
            f += ddF_y;
        }
        x0++;
        ddF_x += 2;
        f += ddF_x;

        //there is a fill color
        if (fcolor != NONE)
        {
            //prevent double draws on the same rows
            if (pyy != y0)
            {
                hline_nbx(y + y0, x - x0, x + x0, fcolor);
                hline_nbx(y - y0, x - x0, x + x0, fcolor);
            }
            if (pyx != x0 && x0 != y0)
            {
                hline_nbx(y + x0, x - y0, x + y0, fcolor);
                hline_nbx(y - x0, x - y0, x + y0, fcolor);
            }
            pyy = y0;
            pyx = x0;
        }
        if (color != NONE)
        {
            setPixel_nbx(x + x0, y + y0, color);
            setPixel_nbx(x - x0, y + y0, color);
            setPixel_nbx(x + x0, y - y0, color);
            setPixel_nbx(x - x0, y - y0, color);
            setPixel_nbx(x + y0, y + x0, color);
            setPixel_nbx(x - y0, y + x0, color);
            setPixel_nbx(x + y0, y - x0, color);
            setPixel_nbx(x - y0, y - x0, color);
        }
    }
}

void Screen::circle(screen_coord_t x, screen_coord_t y, screen_coord_t r, Color_t color,
                    Color_t fcolor)
{
    int32_t f = 1 - r;
    int32_t ddF_x = 1;
    int32_t ddF_y = -2 * r;
    int32_t x0 = 0;
    int32_t y0 = r;
    uint8_t pyy = y0, pyx = x0;

    //there is a fill color
    if (fcolor != NONE)
        hline(y, x - r, x + r, fcolor);

    setPixel(x, y + r, color);
    setPixel(x, y - r, color);
    setPixel(x + r, y, color);
    setPixel(x - r, y, color);

    while (x0 < y0)
    {
        if (f >= 0)
        {
            y0--;
            ddF_y += 2;
            f += ddF_y;
        }
        x0++;
        ddF_x += 2;
        f += ddF_x;

        //there is a fill color
        if (fcolor != NONE)
        {
            //prevent double draws on the same rows
            if (pyy != y0)
            {
                hline(y + y0, x - x0, x + x0, fcolor);
                hline(y - y0, x - x0, x + x0, fcolor);
            }
            if (pyx != x0 && x0 != y0)
            {
                hline(y + x0, x - y0, x + y0, fcolor);
                hline(y - x0, x - y0, x + y0, fcolor);
            }
            pyy = y0;
            pyx = x0;
        }
        if (color != NONE)
        {
            setPixel(x + x0, y + y0, color);
            setPixel(x - x0, y + y0, color);
            setPixel(x + x0, y - y0, color);
            setPixel(x - x0, y - y0, color);
            setPixel(x + y0, y + x0, color);
            setPixel(x - y0, y + x0, color);
            setPixel(x + y0, y - x0, color);
            setPixel(x - y0, y - x0, color);
        }
    }
}

void Screen::circle(const Point_t * pos, screen_coord_t r, Color_t color, Color_t fcolor)
{
    if (pos != NULL)
    {
        circle(pos->x, pos->y, r, color, fcolor);
    }
}

void DispMath::lerp(const Point_t * a, const Point_t * b, Point_t * x, float arg)
{
    x->x = a->x + ((b->x - a->x) * arg);
    x->y = a->y + ((b->y - a->y) * arg);
}

bool Screen::bx(const Point_t* p) const
{
    return !(p->x < 0 || p->y < 0 || p->x >= m_width || p->y >= m_height);
}

bool Screen::bx(screen_coord_t x, screen_coord_t y) const
{
    return !(x < 0 || y < 0 || x >= m_width || y >= m_height);
}

bool Screen::bx(const Rect_t* r) const
{
    return (bx(r->x, r->y) || bx(r->x + r->w, r->y + r->h));
}

#define BMP_PIX(__x, __y) (((bmp->data[(bmp_byte_width * (__y)) + ((__x)/8)] << ((__x) % 8)) & 0x80) == 0x80)

void Screen::bitmap_scaled_nbx(const Bitmap_t * bmp, const Rect_t * srcRect, const Rect_t * destRect, Bitmap_mode_t mode)
{
    int32_t bmp_byte_width = bmp->w/8 + ((bmp->w % 8) ? 1 : 0);

    for(int32_t j = 0; j < destRect->h; j++)
    {
        for(int32_t i = 0; i < destRect->w; i++)
        {
            uint8_t pixel = BMP_PIX(srcRect->x + ((i*srcRect->w)/destRect->w), srcRect->y + ((j*srcRect->h)/destRect->h));
            //BMP_PIX(srcRect->x + i, srcRect->y + j);
            switch(mode)
            {
            case MODE_BLEND:
                setPixel_nbx(destRect->x + i, destRect->y + j, pixel ? WHITE : NONE);
                break;
            case MODE_MASK:
                setPixel_nbx(destRect->x + i, destRect->y + j, pixel ? NONE : BLACK);
                break;
            case MODE_OVERWRITE:
                setPixel_nbx(destRect->x + i, destRect->y + j, pixel ? WHITE : BLACK);
                break;
            case MODE_INVERT:
                setPixel_nbx(destRect->x + i, destRect->y + j, pixel ? INVERT : NONE);
                break;

            case MODE_BLEND_INVERT:
                setPixel_nbx(destRect->x + i, destRect->y + j, pixel ? NONE : WHITE);
                break;
            case MODE_MASK_INVERT:
                setPixel_nbx(destRect->x + i, destRect->y + j, pixel ? BLACK : NONE);
                break;
            case MODE_OVERWRITE_INVERT:
                setPixel_nbx(destRect->x + i, destRect->y + j, pixel ? BLACK : WHITE);
                break;
            case MODE_INVERT_INVERT:
                setPixel_nbx(destRect->x + i, destRect->y + j, pixel ? NONE : INVERT);
                break;
            }
        }
    }
}

void Screen::bitmap_scaled(const Bitmap_t * bmp, const Rect_t * srcRect, const Rect_t * destRect, Bitmap_mode_t mode)
{
    if(!bx(destRect))
        return;

    int32_t bmp_byte_width = bmp->w/8 + ((bmp->w % 8) ? 1 : 0);

    Point_t destPix;

    for(int32_t j = 0; j < destRect->h; j++)
    {
        for(int32_t i = 0; i < destRect->w; i++)
        {
            destPix.x = destRect->x + i;
            destPix.y = destRect->y + j;

            if(!bx(&destPix))
                continue;

            uint8_t pixel = BMP_PIX(srcRect->x + ((i*srcRect->w)/destRect->w), srcRect->y + ((j*srcRect->h)/destRect->h));
            //BMP_PIX(srcRect->x + i, srcRect->y + j);
            switch(mode)
            {
            case MODE_BLEND:
                setPixel(&destPix, pixel ? WHITE : NONE);
                break;
            case MODE_MASK:
                setPixel(&destPix, pixel ? NONE : BLACK);
                break;
            case MODE_OVERWRITE:
                setPixel(&destPix, pixel ? WHITE : BLACK);
                break;
            case MODE_INVERT:
                setPixel(&destPix, pixel ? INVERT : NONE);
                break;

            case MODE_BLEND_INVERT:
                setPixel(&destPix, pixel ? NONE : WHITE);
                break;
            case MODE_MASK_INVERT:
                setPixel(&destPix, pixel ? BLACK : NONE);
                break;
            case MODE_OVERWRITE_INVERT:
                setPixel(&destPix, pixel ? BLACK : WHITE);
                break;
            case MODE_INVERT_INVERT:
                setPixel(&destPix, pixel ? NONE : INVERT);
                break;
            }
        }
    }
}

#undef BMP_PIX

void Screen::triangleArea(const Point_t* p1, const Point_t* p2, const Point_t* p3, Color_t fcolor)
{
    int16_t wx4 = p1->x;
    int16_t wy4 = p1->y;
    int16_t wx5 = p1->x;
    int16_t wy5 = p1->y;

    bool changed1 = false;
    bool changed2 = false;

    int16_t dx1 = abs(p2->x - p1->x);
    int16_t dy1 = abs(p2->y - p1->y);

    int16_t dx2 = abs(p3->x - p1->x);
    int16_t dy2 = abs(p3->y - p1->y);

    int16_t signx1 = (p2->x >= p1->x) ? +1 : -1;
    int16_t signx2 = (p3->x >= p1->x) ? +1 : -1;

    int16_t signy1 = (p2->y >= p1->y) ? +1 : -1;
    int16_t signy2 = (p3->y >= p1->y) ? +1 : -1;

    screen_coord_t temp;

    if (dy1 > dx1)     // swap values
    {
        temp = dx1;
        dx1 = dy1;
        dy1 = temp;
        changed1 = true;
    }

    if (dy2 > dx2)     // swap values
    {
        temp = dx2;
        dx2 = dy2;
        dy2 = temp;
        changed2 = true;
    }

    int16_t e1 = 2 * dy1 - dx1;
    int16_t e2 = 2 * dy2 - dx2;

    Point_t a, b;

    for (int i = 0; i <= dx1; i++)
    {
        a.x = wx4;
        a.y = wy4;

        b.x = wx5;
        b.y = wy5;
        line(&a, &b, fcolor);

        if(dx1)
            while (e1 >= 0)
            {
                if (changed1) wx4 += signx1;
                else wy4 += signy1;
                e1 = e1 - 2 * dx1;
            }

        if (changed1) wy4 += signy1;
        else wx4 += signx1;

        e1 = e1 + 2 * dy1;

        while (wy5 != wy4)
        {
            if(dx2)
                while (e2 >= 0)
                {
                    if (changed2) wx5 += signx2;
                    else wy5 += signy2;
                    e2 = e2 - 2 * dx2;
                }

            if (changed2) wy5 += signy2;
            else wx5 += signx2;

            e2 = e2 + 2 * dy2;
        }
    }
}

void Screen::triangle(const Point_t* _p1, const Point_t* _p2, const Point_t* _p3, Color_t color, Color_t fcolor)
{
    Point_t p1 = *_p1;
    Point_t p2 = *_p2;
    Point_t p3 = *_p3;

    if(color == NONE)
        return;

    if(fcolor != NONE)
    {
        bool b = true;

        // Graham Scan + Andrew's Monotone Chain Algorithm
        // Sort by ascending y
        while (b)
        {
            screen_coord_t temp;
            b = false;
            if ((b == false) && (p1.y > p2.y))
            {
                temp = p1.x;
                p1.x = p2.x;
                p2.x = temp;

                temp = p1.y;
                p1.y = p2.y;
                p2.y = temp;
                b = true;
            }
            if ((b == false) && (p2.y > p3.y))
            {
                temp = p3.x;
                p3.x = p2.x;
                p2.x = temp;

                temp = p3.y;
                p3.y = p2.y;
                p2.y = temp;
                b = true;
            }
        }

        if (p2.y == p3.y)
        {
            triangleArea(&p1, &p2, &p3, fcolor);

        }
        else if (p1.y == p2.y)
        {
            triangleArea(&p3, &p1, &p2, fcolor);

        }
        else
        {
            Point_t p4;
            p4.x = (screen_coord_t)( (int32_t)p1.x + (p2.y - p1.y) * (p3.x - p1.x) / (p3.y - p1.y) );
            p4.y = p2.y;

            triangleArea(&p1, &p2, &p4, fcolor);
            triangleArea(&p3, &p2, &p4, fcolor);
        }
    }

    line(_p1, _p2, color);
    line(_p2, _p3, color);
    line(_p3, _p1, color);
}


#endif
