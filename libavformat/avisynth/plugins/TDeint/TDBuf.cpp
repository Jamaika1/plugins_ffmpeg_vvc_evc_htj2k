/*
**                TDeinterlace for AviSynth 2.6 interface
**
**   TDeinterlace is a bi-directionally motion adaptive deinterlacer.
**   It also uses a couple modified forms of ela interpolation which
**   help to reduce "jaggy" edges in places where interpolation must
**   be used. TDeinterlace currently supports 8 bit planar YUV and YUY2 colorspaces.
**
**   Copyright (C) 2004-2007 Kevin Stone, additional work (C) 2020 pinterf
**
**   This program is free software; you can redistribute it and/or modify
**   it under the terms of the GNU General Public License as published by
**   the Free Software Foundation; either version 2 of the License, or
**   (at your option) any later version.
**
**   This program is distributed in the hope that it will be useful,
**   but WITHOUT ANY WARRANTY; without even the implied warranty of
**   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
**   GNU General Public License for more details.
**
**   You should have received a copy of the GNU General Public License
**   along with this program; if not, write to the Free Software
**   Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
*/

#include "TDBuf.h"
#include "internal.h"

// always a 8 bit buffer
TDBuf::TDBuf(int _size, int _width, int _height, int _cp, int planarType) : size(_size)
{
  constexpr int MY_FRAME_ALIGN = FRAME_ALIGN;
  y = u = v = NULL;
  fnum.resize(size);
  // 400: greyscale
  if (planarType == 444 || planarType == 422 || planarType == 420 || planarType == 411 || planarType == 400)
  {
    widthy = _width;
    heighty = _height;
    lpitchy = ((widthy + (MY_FRAME_ALIGN - 1)) / MY_FRAME_ALIGN) * MY_FRAME_ALIGN;
    pitchy = lpitchy * size;
    if (_cp == 3) { // count of planes
      widthuv = planarType == 444 ? _width : planarType == 411 ? _width / 4 : _width / 2;
      heightuv = planarType == 420 ? _height / 2 : _height;
      lpitchuv = ((widthuv + (MY_FRAME_ALIGN - 1)) / MY_FRAME_ALIGN) * MY_FRAME_ALIGN;
      pitchuv = lpitchuv * size;
    }
    else {
      heightuv = widthuv = pitchuv = lpitchuv = 0;
    }
  }
  else
  {
    // YUY2
    widthy = _width * 2; // *2 row size, no chroma plane, interleaved YUYV YUYV...
    heighty = _height;
    lpitchy = ((widthy + (MY_FRAME_ALIGN - 1)) / MY_FRAME_ALIGN) * MY_FRAME_ALIGN;
    pitchy = lpitchy*size;
    heightuv = widthuv = pitchuv = lpitchuv = 0;
  }
  if (size)
  {
    for (int i = 0; i < size; ++i) fnum[i] = -999999999;
    y = (uint8_t*)_aligned_malloc(pitchy*heighty, MY_FRAME_ALIGN);
    if (_cp == 3)
    {
      u = (uint8_t*)_aligned_malloc(pitchuv*heightuv, MY_FRAME_ALIGN);
      v = (uint8_t*)_aligned_malloc(pitchuv*heightuv, MY_FRAME_ALIGN);
    }
  }
  spos = 0;
}

TDBuf::~TDBuf()
{
  if (y) _aligned_free(y);
  if (u) _aligned_free(u);
  if (v) _aligned_free(v);
}

const uint8_t* TDBuf::GetReadPtr(int pos, int plane)
{
  if (plane == 0) return y + pos*lpitchy;
  if (plane == 1) return u + pos*lpitchuv;
  return v + pos*lpitchuv;
}

uint8_t* TDBuf::GetWritePtr(int pos, int plane)
{
  if (plane == 0) return y + pos*lpitchy;
  if (plane == 1) return u + pos*lpitchuv;
  return v + pos*lpitchuv;
}

int TDBuf::GetPitch(int plane)
{
  if (plane == 0) return pitchy;
  return pitchuv;
}

int TDBuf::GetLPitch(int plane)
{
  if (plane == 0) return lpitchy;
  return lpitchuv;
}

int TDBuf::GetHeight(int plane)
{
  if (plane == 0) return heighty;
  return heightuv;
}

int TDBuf::GetWidth(int plane)
{
  if (plane == 0) return widthy;
  return widthuv;
}

int TDBuf::GetPos(int n)
{
  return (spos + n) % size;
}

void TDBuf::resetCacheStart(int n)
{
  for (int i = 0; i < size; ++i)
  {
    if (fnum[i] == n)
    {
      spos = i;
      return;
    }
  }
  for (int i = 0; i < size; ++i)
    fnum[i] = -999999999;
  spos = 0;
}
