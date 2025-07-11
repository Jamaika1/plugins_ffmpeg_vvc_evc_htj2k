/*
*    This file is part of KNLMeansCL,
*    Copyright(C) 2002		thejam79,
*    Copyright(C) 2003		minamina,
*    Copyright(C) 2007		Donald A. Graft,
*    Copyright(C) 2014-2020 Edoardo Brunetti.
*
*    KNLMeansCL is free software: you can redistribute it and/or modify
*    it under the terms of the GNU General Public License as published by
*    the Free Software Foundation, either version 3 of the License, or
*    (at your option) any later version.
*
*    KNLMeansCL is distributed in the hope that it will be useful,
*    but WITHOUT ANY WARRANTY; without even the implied warranty of
*    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
*    GNU General Public License for more details.
*
*    You should have received a copy of the GNU General Public License
*    along with KNLMeansCL. If not, see <http://www.gnu.org/licenses/>.
*/

#include "startchar.h"
#include "font.h"

//////////////////////////////////////////
// Functions
void DrawDigit(uint8_t* dst, int pitch, int x, int y, int num)
{
    uint8_t *dp;
    x = x * 10;
    y = y * 20;
    for (int tx = 0; tx < 10; tx++) {
        for (int ty = 0; ty < 20; ty++) {
            dp = &dst[((x + tx) / 1) + ((y + ty) / 1) * pitch];
            if (font[num][ty] & (1 << (15 - tx))) *dp = 235;
            else *dp = (uint8_t)(*dp >> 1);
        }
    }
}

void DrawString(uint8_t* dst, int pitch, int x, int y, const char *s)
{
    for (int xx = 0; *s; ++s, ++xx) DrawDigit(dst, pitch, x + xx, y, *s - ' ');
}
