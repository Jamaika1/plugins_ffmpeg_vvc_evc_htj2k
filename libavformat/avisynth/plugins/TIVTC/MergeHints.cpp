/*
**                    TIVTC for AviSynth 2.6 interface
**
**   TIVTC includes a field matching filter (TFM) and a decimation
**   filter (TDecimate) which can be used together to achieve an
**   IVTC or for other uses. TIVTC currently supports 8 bit planar YUV and
**   YUY2 colorspaces.
**
**   Copyright (C) 2004-2008 Kevin Stone, additional work (C) 2020 pinterf
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

#include "MergeHints.h"
#include "../../avisynth.h"
#ifdef _WIN32
#include <windows.h>
#endif

MergeHints::MergeHints(PClip _child, PClip _hintClip, bool _debug, IScriptEnvironment *env) :
  GenericVideoFilter(_child), hintClip(_hintClip), debug(_debug)
{
  if (vi.width * vi.height < 64)
    env->ThrowError("MergeHints:  total frame size must be at least 64 pixels!");

  if (vi.BitsPerComponent() > 16)
    env->ThrowError("MergeHints:  only 8-16 bit formats supported!");

  if (!vi.IsYUV())
    env->ThrowError("MergeHints:  only YUV input supported!");
  child->SetCacheHints(CACHE_NOTHING, 0);
  hintClip->SetCacheHints(CACHE_NOTHING, 0);
  if (debug)
  {
    sprintf(buf, "MergeHints:  %s by tritical\n", VERSION);
    OutputDebugString(buf);
  }
}

MergeHints::~MergeHints()
{
  /* nothing to free */
}

PVideoFrame __stdcall MergeHints::GetFrame(int n, IScriptEnvironment *env)
{
  PVideoFrame hnt = hintClip->GetFrame(n, env);
  unsigned int i, magic_number = 0, hint = 0;
  if (vi.ComponentSize() == 1) {
    const uint8_t* hntp = hnt->GetReadPtr(PLANAR_Y);
    for (i = 0; i < 32; ++i) magic_number |= ((*hntp++ & 1) << i);
    for (i = 0; i < 32; ++i) hint |= ((*hntp++ & 1) << i);
  }
  else {
    // 10-16 bits
    const uint16_t* hntp = reinterpret_cast<const uint16_t *>(hnt->GetReadPtr(PLANAR_Y));
    for (i = 0; i < 32; ++i) magic_number |= ((*hntp++ & 1) << i);
    for (i = 0; i < 32; ++i) hint |= ((*hntp++ & 1) << i);
  }
  if (debug)
  {
    sprintf(buf, "MergeHints:  identifier = %#x (%s)  hint = %#x\n", magic_number,
      magic_number == MAGIC_NUMBER_DEADFEED ? "TIVTC" : magic_number == MAGIC_NUMBER_2_DEADBEEF ?
      "Decomb or DGDecode" : "Unknown", hint);
    OutputDebugString(buf);
  }
  PVideoFrame src = child->GetFrame(n, env);
  env->MakeWritable(&src);
  if (vi.ComponentSize() == 1) {
    uint8_t* dstp = src->GetWritePtr(PLANAR_Y);
    for (i = 0; i < 32; ++i)
    {
      *dstp &= ~1;
      *dstp++ |= ((magic_number & (1 << i)) >> i);
    }
    for (i = 0; i < 32; ++i)
    {
      *dstp &= ~1;
      *dstp++ |= ((hint & (1 << i)) >> i);
    }
  }
  else {
    uint16_t* dstp = reinterpret_cast<uint16_t *>(src->GetWritePtr(PLANAR_Y));
    for (i = 0; i < 32; ++i)
    {
      *dstp &= ~1;
      *dstp++ |= ((magic_number & (1 << i)) >> i);
    }
    for (i = 0; i < 32; ++i)
    {
      *dstp &= ~1;
      *dstp++ |= ((hint & (1 << i)) >> i);
    }
  }
  return src;
}

AVSValue __cdecl Create_MergeHints(AVSValue args, void* user_data, IScriptEnvironment* env)
{
  if (!args[0].IsClip() || !args[1].IsClip())
    env->ThrowError("MergeHints:  there must be two input clips specified!");
  return new MergeHints(args[0].AsClip(), args[1].AsClip(), args[2].AsBool(false), env);
}
