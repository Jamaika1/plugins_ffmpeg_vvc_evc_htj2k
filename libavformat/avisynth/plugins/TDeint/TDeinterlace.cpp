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
#include "internal.h"
#include <algorithm>
#include "TDeinterlace.h"
#include "../../avs/alignment.h"

PVideoFrame __stdcall TDeinterlace::GetFrame(int n, IScriptEnvironment* env)
{
  if (n < 0) n = 0;
  else if (mode == 0 && n > nfrms) n = nfrms;
  else if (mode == 1 && n > nfrms2) n = nfrms2;
  PVideoFrame dst;
  bool wdtd = false;

  if (vi.IsPlanar())
    dst = GetFramePlanar(n, env, wdtd);
  else
    dst = GetFrameYUY2(n, env, wdtd);

  if (tshints && map != 1 && map != 2)
  {
    env->MakeWritable(&dst);
    putHint2(vi, dst, wdtd);
  }
  return dst;
}

void TDeinterlace::insertCompStats(int n, int norm1, int norm2, int mtn1, int mtn2)
{
  if (sa.size() > 0)
  {
    int pos = sa_pos * 5;
    sa[pos + 0] = n;
    sa[pos + 1] = norm1;
    sa[pos + 2] = norm2;
    sa[pos + 3] = mtn1;
    sa[pos + 4] = mtn2;
    sa_pos = (sa_pos + 1) % 500;
  }
}

int TDeinterlace::getMatch(int norm1, int norm2, int mtn1, int mtn2)
{
  float c1 = float(std::max(norm1, norm2)) / float(std::max(std::min(norm1, norm2), 1));
  float c2 = float(std::max(mtn1, mtn2)) / float(std::max(std::min(mtn1, mtn2), 1));
  float mr = float(std::max(mtn1, mtn2)) / float(std::max(std::max(norm1, norm2), 1));
  if (slow == 0)
  {
    if (((mtn1 >= 500 || mtn2 >= 500) && (mtn1 * 2 < mtn2 * 1 || mtn2 * 2 < mtn1 * 1)) ||
      ((mtn1 >= 1000 || mtn2 >= 1000) && (mtn1 * 3 < mtn2 * 2 || mtn2 * 3 < mtn1 * 2)) ||
      ((mtn1 >= 2000 || mtn2 >= 2000) && (mtn1 * 5 < mtn2 * 4 || mtn2 * 5 < mtn1 * 4)) ||
      ((mtn1 >= 4000 || mtn2 >= 4000) && c2 > c1))
    {
      if (mtn1 > mtn2) return 1;
      return 0;
    }
  }
  else if (slow == 1)
  {
    if (((mtn1 >= 375 || mtn2 >= 375) && (mtn1 * 3 < mtn2 * 1 || mtn2 * 3 < mtn1 * 1)) ||
      ((mtn1 >= 500 || mtn2 >= 500) && (mtn1 * 2 < mtn2 * 1 || mtn2 * 2 < mtn1 * 1)) ||
      ((mtn1 >= 1000 || mtn2 >= 1000) && (mtn1 * 3 < mtn2 * 2 || mtn2 * 3 < mtn1 * 2)) ||
      ((mtn1 >= 2000 || mtn2 >= 2000) && (mtn1 * 5 < mtn2 * 4 || mtn2 * 5 < mtn1 * 4)) ||
      ((mtn1 >= 4000 || mtn2 >= 4000) && c2 > c1))
    {
      if (mtn1 > mtn2) return 1;
      return 0;
    }
  }
  else
  {
    if (((mtn1 >= 250 || mtn2 >= 250) && (mtn1 * 4 < mtn2 * 1 || mtn2 * 4 < mtn1 * 1)) ||
      ((mtn1 >= 375 || mtn2 >= 375) && (mtn1 * 3 < mtn2 * 1 || mtn2 * 3 < mtn1 * 1)) ||
      ((mtn1 >= 500 || mtn2 >= 500) && (mtn1 * 2 < mtn2 * 1 || mtn2 * 2 < mtn1 * 1)) ||
      ((mtn1 >= 1000 || mtn2 >= 1000) && (mtn1 * 3 < mtn2 * 2 || mtn2 * 3 < mtn1 * 2)) ||
      ((mtn1 >= 2000 || mtn2 >= 2000) && (mtn1 * 5 < mtn2 * 4 || mtn2 * 5 < mtn1 * 4)) ||
      ((mtn1 >= 4000 || mtn2 >= 4000) && c2 > c1))
    {
      if (mtn1 > mtn2) return 1;
      return 0;
    }
  }
  if (mr > 0.005 && std::max(mtn1, mtn2) > 150 && (mtn1 * 2 < mtn2 * 1 || mtn2 * 2 < mtn1 * 1))
  {
    if (mtn1 > mtn2) return 1;
    return 0;
  }
  else
  {
    if (norm1 > norm2) return 1;
    return 0;
  }
}

int TDeinterlace::getHint(const VideoInfo& vi, PVideoFrame& src, unsigned int& storeHint, int& hintField)
{
  if (vi.ComponentSize() == 1)
    return getHint_core<uint8_t>(src, storeHint, hintField);
  else
    return getHint_core<uint16_t>(src, storeHint, hintField);
}

template<typename pixel_t>
int TDeinterlace::getHint_core(PVideoFrame &src, unsigned int &storeHint, int &hintField)
{
  hintField = -1;
  const pixel_t *p = reinterpret_cast<const pixel_t *>(src->GetReadPtr(PLANAR_Y));
  unsigned int i, magic_number = 0, hint = 0;
  storeHint = 0xFFFFFFFF;
  for (i = 0; i < 32; ++i)
  {
    magic_number |= ((*p++ & 1) << i);
  }
  if (magic_number != MAGIC_NUMBER_2_DEADBEEF &&
    magic_number != MAGIC_NUMBER_DEADFEED) return -1;
  for (i = 0; i < 32; ++i)
  {
    hint |= ((*p++ & 1) << i);
  }
  if (hint & 0xFFFF0000) return -1;
  storeHint = hint;
  if (magic_number == MAGIC_NUMBER_2_DEADBEEF)
  {
    storeHint |= 0x00100000;
    if (hint & 0x01) return 0;
    return 1;
  }
  if (hint & 0x08) hintField = 1;
  else hintField = 0;
  if (hint & 0x10) return 1;
  return 0;
}

void TDeinterlace::putHint(const VideoInfo& vi, PVideoFrame& dst, unsigned int hint, int fieldt)
{
  if (vi.NumComponents() == 1)
    putHint_core<uint8_t>(dst, hint, fieldt);
  else
    putHint_core<uint16_t>(dst, hint, fieldt);
}

template<typename pixel_t>
void TDeinterlace::putHint_core(PVideoFrame &dst, unsigned int hint, int fieldt)
{
  int htype = (hint & 0x00100000) ? 0 : 1;
  hint &= ~0x00100000;
  if (hint & 0xFFFF0000) return;
  if (htype == 1) // tfm hint, modify it for tdecimate
  {
    hint &= 0xFFA0;
    if (fieldt == 1) hint |= 0x0E; // top + 'h'
    else hint |= 0x05; // bot + 'l'
  }
  pixel_t *p = reinterpret_cast<pixel_t*>(dst->GetWritePtr(PLANAR_Y));
  unsigned int i;
  for (i = 0; i < 32; ++i)
  {
    *p &= ~1;
    if (htype == 0) *p++ |= ((MAGIC_NUMBER_2_DEADBEEF & (1 << i)) >> i);
    else *p++ |= ((MAGIC_NUMBER_DEADFEED & (1 << i)) >> i);
  }
  for (i = 0; i < 32; ++i)
  {
    *p &= ~1;
    *p++ |= ((hint & (1 << i)) >> i);
  }
}

void TDeinterlace::putHint2(const VideoInfo& vi, PVideoFrame& dst, bool wdtd)
{
  if (vi.NumComponents() == 1)
    putHint2_core<uint8_t>(dst, wdtd);
  else
    putHint2_core<uint16_t>(dst, wdtd);
}

template<typename pixel_t>
void TDeinterlace::putHint2_core(PVideoFrame &dst, bool wdtd)
{
  pixel_t* p = reinterpret_cast<pixel_t*>(dst->GetWritePtr(PLANAR_Y));
  unsigned int i, magic_number = 0, hint = 0;
  for (i = 0; i < 32; ++i)
  {
    magic_number |= ((*p++ & 1) << i);
  }
  if (magic_number == MAGIC_NUMBER_2_DEADBEEF)
  {
    for (i = 0; i < 32; ++i)
    {
      hint |= ((*p++ & 1) << i);
    }
    hint <<= 8;
    hint |= 0x80;
    if (wdtd) hint |= 0x40;
    magic_number = MAGIC_NUMBER_4_DEADBEAD;
  }
  else if (magic_number == MAGIC_NUMBER_DEADFEED)
  {
    for (i = 0; i < 32; ++i)
    {
      hint |= ((*p++ & 1) << i);
    }
    if (wdtd) hint |= 0x40;
  }
  else
  {
    magic_number = MAGIC_NUMBER_3_DEADDEED;
    if (wdtd) hint |= 0x40;
  }
  p = reinterpret_cast<pixel_t*>(dst->GetWritePtr(PLANAR_Y));
  for (i = 0; i < 32; ++i)
  {
    *p &= ~1;
    *p++ |= ((magic_number & (1 << i)) >> i);
  }
  for (i = 0; i < 32; ++i)
  {
    *p &= ~1;
    *p++ |= ((hint & (1 << i)) >> i);
  }
}

// HBD ready because absDiff is OK
void TDeinterlace::InsertDiff(PVideoFrame &p1, PVideoFrame &p2, int n, int pos)
{
  if (db->fnum[pos] == n) return;
  PVideoFrame dummyframe = NULL; // put into td buf @plane 'pos' instead
  absDiff(p1, p2, dummyframe, pos); // HBD ready inside
  db->fnum[pos] = n;
}

void TDeinterlace::stackVertical(PVideoFrame &dst2, PVideoFrame &p1, PVideoFrame &p2, IScriptEnvironment *env)
{
  // bit depth independent
  const int planes[3] = { PLANAR_Y, PLANAR_U, PLANAR_V };
  const int np = vi.IsYUY2() || vi.IsY() ? 1 : 3;
  for (int b = 0; b < np; ++b)
  {
    const int plane = planes[b];
    env->BitBlt(dst2->GetWritePtr(plane), dst2->GetPitch(plane),
      p1->GetReadPtr(plane), p1->GetPitch(plane), p1->GetRowSize(plane),
      p1->GetHeight(plane));
    env->BitBlt(dst2->GetWritePtr(plane) + dst2->GetPitch(plane)*p1->GetHeight(plane),
      dst2->GetPitch(plane), p2->GetReadPtr(plane), p2->GetPitch(plane),
      p2->GetRowSize(plane), p2->GetHeight(plane));
  }
}

TDeinterlace::TDeinterlace(PClip _child, int _mode, int _order, int _field, int _mthreshL,
  int _mthreshC, int _map, const char* _ovr, int _ovrDefault, int _type, bool _debug,
  int _mtnmode, bool _sharp, bool _hints, PClip _clip2, bool _full, int _cthresh,
  bool _chroma, int _MI, bool _tryWeave, int _link, bool _denoise, int _AP,
  int _blockx, int _blocky, int _APType, PClip _edeint, PClip _emask, int _metric,
  int _expand, int _slow, PClip _emtn, bool _tshints, int _opt, IScriptEnvironment* env) :
  GenericVideoFilter(_child),
  mode(_mode), order(_order), field(_field), mthreshL(_mthreshL),
  mthreshC(_mthreshC), map(_map), ovr(_ovr), ovrDefault(_ovrDefault), type(_type),
  debug(_debug), mtnmode(_mtnmode), sharp(_sharp), hints(_hints), clip2(_clip2), full(_full),
  cthresh(_cthresh), chroma(_chroma), MI(_MI), tryWeave(_tryWeave), link(_link),
  denoise(_denoise), AP(_AP), blockx(_blockx), blocky(_blocky), APType(_APType),
  edeint(_edeint), emask(_emask), metric(_metric), expand(_expand), slow(_slow),
  emtn(_emtn), tshints(_tshints), opt(_opt)
{

  has_at_least_v8 = true;
  try { env->CheckVersion(8); } catch (const AvisynthError&) { has_at_least_v8 = false; }

  cpuFlags = env->GetCPUFlags();
  if (opt == 0) cpuFlags = 0;

  int z, w, q, b, i, track, count;
  char linein[1024];
  char *linep;
  FILE *f = NULL;
  cArray = NULL;
  tbuffer = NULL;
  db = NULL;
  const int orig_mode = mode;
  if (mode == 2)
  {
    mode = 1;
    if (type == 2 || mtnmode > 1 || tryWeave)
    {
      sa.resize(5 * 500);
      for (int i = 0; i < 500; ++i) sa[i] = -1;
    }
  }
  if (vi.IsRGB())
    env->ThrowError("TDeint:  RGB data not supported!");
  if (vi.BitsPerComponent() > 16)
    env->ThrowError("TDeint:  Only 8-16 bit clip data supported!");
  if (mode != 0 && mode != 1 && mode != -1 && mode != -2)
    env->ThrowError("TDeint:  mode must be set to -2, -1, 0, or 1!");
  if (order != 0 && order != 1 && order != -1)
    env->ThrowError("TDeint:  order must be set to 0, 1, or -1!");
  if (field != 0 && field != 1 && field != -1)
    env->ThrowError("TDeint:  field must be set to 0, 1, or -1!");
  if (map < 0 || map > 4)
    env->ThrowError("TDeint:  map option must be set to 0, 1, 2, 3, or 4!");
  if (ovrDefault != 0 && ovrDefault != 1)
    env->ThrowError("TDeint:  ovrDefault must be set to either 0 or 1!");
  if (type < 0 || type > 5)
    env->ThrowError("TDeint:  type must be set to either 0, 1, 2, 3, 4, or 5!");
  if (mtnmode < 0 || mtnmode > 3)
    env->ThrowError("TDeint:  mtnmode must be set to either 0, 1, 2, or 3!");
  if (vi.height & 1 || vi.width & 1)
    env->ThrowError("TDeint:  width and height must be multiples of 2!");
  if (link < 0 || link > 3)
    env->ThrowError("TDeint:  link must be set to 0, 1, 2, or 3!");
  if (blockx != 4 && blockx != 8 && blockx != 16 && blockx != 32 && blockx != 64 &&
    blockx != 128 && blockx != 256 && blockx != 512 && blockx != 1024 && blockx != 2048)
    env->ThrowError("TDeint:  illegal blockx size!");
  if (blocky != 4 && blocky != 8 && blocky != 16 && blocky != 32 && blocky != 64 &&
    blocky != 128 && blocky != 256 && blocky != 512 && blocky != 1024 && blocky != 2048)
    env->ThrowError("TDeint:  illegal blocky size!");
  if (APType < 0 || APType > 2)
    env->ThrowError("TDeint:  APType must be set to 0, 1, or 2!");
  if (opt < 0 || opt > 4)
    env->ThrowError("TDeint:  opt must be set to 0, 1, 2, 3, or 4!");
  if (metric != 0 && metric != 1)
    env->ThrowError("TDeint:  metric must be set to 0 or 1!");
  if (expand < 0)
    env->ThrowError("TDeint:  expand must be greater than or equal to 0!");
  if (slow < 0 || slow > 2)
    env->ThrowError("TDeint:  slow must be set to 0, 1, or 2!");
  child->SetCacheHints(CACHE_GENERIC, 5);
  useClip2 = false;
  if ((hints || !full) && mode == 0 && clip2)
  {
    const VideoInfo& vi1 = clip2->GetVideoInfo();
    if (vi1.height != vi.height || vi1.width != vi.width || vi1.BitsPerComponent() != vi.BitsPerComponent())
      env->ThrowError("TDeint:  width and height and bit depth of clip2 must equal that of the input clip!");
    if (vi.IsRGB())
      env->ThrowError("TDeint: no RGB format allowed!");
    if (!vi.IsSameColorspace(vi1))
      env->ThrowError("TDeint:  colorspace of clip2 doesn't match that of the input clip!");
    if (vi.num_frames != vi1.num_frames)
      env->ThrowError("TDeint:  number of frames in clip2 doesn't match that of the input clip!");
    useClip2 = true;
  }
  if (edeint)
  {
    const VideoInfo& vi1 = edeint->GetVideoInfo();
    if (vi1.height != vi.height || vi1.width != vi.width || vi1.BitsPerComponent() != vi.BitsPerComponent())
      env->ThrowError("TDeint:  width and height and bit depth of edeint clip must equal that of the input clip!");
    if (vi.IsRGB())
      env->ThrowError("TDeint: no RGB format allowed (edeint)!");
    if (!vi.IsSameColorspace(vi1))
      env->ThrowError("TDeint:  colorspace of edeint clip doesn't match that of the input clip!");
    if ((mode == 0 && vi.num_frames != vi1.num_frames) ||
      (mode == 1 && vi.num_frames * 2 != vi1.num_frames))
      env->ThrowError("TDeint:  number of frames in edeint clip doesn't match that of the input clip!");
    edeint->SetCacheHints(CACHE_NOTHING, 0);
  }
  if (emask)
  {
    const VideoInfo& vi1 = emask->GetVideoInfo();
    // mask clip is always 8 bits.
    if (vi1.height != vi.height || vi1.width != vi.width)
      env->ThrowError("TDeint:  width and height of emask clip must be equal that of the input clip!");
    if (vi1.BitsPerComponent() != 8)
      env->ThrowError("TDeint:  mask clip must be of 8 bits!");
    if (vi.IsRGB())
      env->ThrowError("TDeint: no RGB format allowed (edeint)!");
    // check format, independent of bit depth
    VideoInfo vi8 = vi;
    vi8.pixel_type = (vi8.pixel_type & ~VideoInfo::CS_Sample_Bits_Mask) | VideoInfo::CS_Sample_Bits_8;
    if (!vi8.IsSameColorspace(vi1))
      env->ThrowError("TDeint:  colorspace of emask clip doesn't match that of the input clip!");
    if ((mode == 0 && vi.num_frames != vi1.num_frames) ||
      (mode == 1 && vi.num_frames * 2 != vi1.num_frames))
      env->ThrowError("TDeint:  number of frames in emask clip doesn't match that of the input clip!");
    emask->SetCacheHints(CACHE_NOTHING, 0);
  }
  if (emtn)
  {
    const VideoInfo& vi1 = emtn->GetVideoInfo();
    if (vi1.height != vi.height || vi1.width != vi.width || vi1.BitsPerComponent() != vi.BitsPerComponent())
      env->ThrowError("TDeint:  width and height and bit depth of emtn clip must equal that of the input clip!");
    if (vi.IsRGB())
      env->ThrowError("TDeint: no RGB format allowed (emtn)!");
    if (!vi.IsSameColorspace(vi1))
      env->ThrowError("TDeint:  colorspace of emtn clip doesn't match that of the input clip!");
    if (vi.num_frames != vi1.num_frames)
      env->ThrowError("TDeint:  number of frames in emtn clip doesn't match that of the input clip!");
    emtn->SetCacheHints(CACHE_GENERIC, 5);
  }

  // like in FrameDiff
  sa_pos = 0;
  blockx_half = blockx >> 1;
  blocky_half = blocky >> 1;
  blockx_shift = blockx == 4 ? 2 : blockx == 8 ? 3 : blockx == 16 ? 4 : blockx == 32 ? 5 :
    blockx == 64 ? 6 : blockx == 128 ? 7 : blockx == 256 ? 8 : blockx == 512 ? 9 :
    blockx == 1024 ? 10 : 11;
  blocky_shift = blocky == 4 ? 2 : blocky == 8 ? 3 : blocky == 16 ? 4 : blocky == 32 ? 5 :
    blocky == 64 ? 6 : blocky == 128 ? 7 : blocky == 256 ? 8 : blocky == 512 ? 9 :
    blocky == 1024 ? 10 : 11;
  if (((!full && mode == 0) || tryWeave) && mode >= 0)
  {
    constexpr int ALIGN = FRAME_ALIGN;
    cArray = (int *)_aligned_malloc((((vi.width + blockx_half) >> blockx_shift) + 1)*(((vi.height + blocky_half) >> blocky_shift) + 1) * 4 * sizeof(int), ALIGN);
    if (cArray == NULL) env->ThrowError("TDeint:  malloc failure!");
  }

  // YUY2: n/a
  const int planarType = vi.Is444() ? 444 : vi.Is422() ? 422 : vi.IsYV411() ? 411 : vi.Is420() ? 420 : vi.IsY() ? 400 : 0;

  // info: TDBuf is always a byte buffer
  db = new TDBuf((mtnmode & 1) ? 7 : mode == 1 ? 4 : 3, vi.width, vi.height, vi.IsPlanar() && !vi.IsY() ? 3 : 1, planarType);
  if (vi.IsYUY2())
  {
    blockx_half *= 2;
    ++blockx_shift;
  }

  if (slow > 0)
  {
    constexpr int ALIGN = FRAME_ALIGN;
    if (vi.IsPlanar())
    {
      // tbuffer is 8 or 16 bits wide
      const int pixelsize = vi.ComponentSize();
      tpitchy = AlignNumber(vi.width * pixelsize, ALIGN);
      if (!vi.IsY()) {
        const int widthUV = vi.width >> vi.GetPlaneWidthSubsampling(PLANAR_U);
        tpitchuv = AlignNumber(widthUV * pixelsize, ALIGN);
      }
      else
        tpitchuv = 0; // n/a
    }
    else {
      // YUY2
      tpitchy = AlignNumber(vi.width << 1, ALIGN);
    }

    // Fix in 1.4: map>=3: stacked vertically, double height, TDHelper calls back into
    // subtractFields1 and subtractFields2 with double height output, allocate double heigt for tbuffer
    if (orig_mode == 2 && map >= 3) // v1.4
      tbuffer = (uint8_t*)_aligned_malloc((vi.height >> 1) * tpitchy * 2, ALIGN);
    else
      tbuffer = (uint8_t*)_aligned_malloc((vi.height >> 1) * tpitchy, ALIGN);
    if (tbuffer == NULL)
      env->ThrowError("TDeinterlace:  malloc failure (tbuffer)!");
  }
  vi.SetFieldBased(false);
  nfrms = nfrms2 = vi.num_frames - 1;
  accumPn = accumNn = 0;
  accumPm = accumNm = 0;
  rmatch = -1;
  cthresh6 = cthresh * 6;
  passHint = 0xFFFFFFFF;
  autoFO = false;
  if (mode < 0)
  {
    vi.height *= 2;
    field = 1;
  }
  if (order == -1) autoFO = true;
  if (mode == 1)
  {
    vi.num_frames *= 2;
    nfrms2 = vi.num_frames - 1;
    vi.SetFPS(vi.fps_numerator * 2, vi.fps_denominator);
  }
  else if (field == -1)
  {
    // telecide matches off the bottom field so we want field=0 in that case.
    // tfm can match off top or bottom, but it will indicate which in its hints
    // and field is adjusted appropriately then... so we use field=0 by default
    // if hints=true.  Otherwise, if hints=false, we default to field = order.
    if (hints) field = 0;
    else field = order;
  }
  vi_saved = vi; // because map>2 doubles the height

  vi_mask = vi; // prepare mask format: always 8 bits
  vi_mask.pixel_type = (vi_mask.pixel_type & ~VideoInfo::CS_Sample_Bits_Mask) | VideoInfo::CS_Sample_Bits_8;

  if (map > 2) vi.height *= 2; // vertically stacked output

  order_origSaved = order;
  field_origSaved = field;
  mthreshL_origSaved = mthreshL;
  mthreshC_origSaved = mthreshC;
  type_origSaved = type;

  if (debug)
  {
    sprintf(buf, "TDeint:  %s (%s) by tritical\n", TDEINT_VERSION, TDEINT_DATE);
    OutputDebugString(buf);
    sprintf(buf, "TDeint:  mode = %d (%s)\n", mode, mode == 0 ? "normal - same rate" :
      mode == 1 ? "bob - double rate" : mode == -2 ? "upsize - ELA" : "upsize - ELA-2");
    OutputDebugString(buf);
  }
  if (*ovr && mode >= 0)
  {
    countOvr = i = 0;
    if ((f = fopen(ovr, "r")) != NULL)
    {
      while (fgets(linein, 1024, f) != 0)
      {
        if (linein[0] == 0 || linein[0] == '\n' || linein[0] == '\r' || linein[0] == ';' ||
          linein[0] == '#') continue;
        linep = linein;
        while (*linep != '-' && *linep != '+' && *linep != 0) linep++;
        if (*linep == 0) ++countOvr;
        else if (*(linep + 1) == '-' || *(linep + 1) == '+')
        {
          linep = linein;
          while (*linep != ',' && *linep != 0) linep++;
          if (*linep == ',')
          {
            sscanf(linein, "%d,%d", &z, &w);
            if (z<0 || z>nfrms || w<0 || w>nfrms || w < z)
            {
              fclose(f);
              f = NULL;
              env->ThrowError("TDeint: ovr input error (invalid frame range)!");
            }
            countOvr += (w - z + 1);
          }
          else
          {
            fclose(f);
            f = NULL;
            env->ThrowError("TDeint:  ovr input error (invalid entry)!");
          }
        }
        else ++countOvr;
      }
      fclose(f);
      f = NULL;
      if (countOvr <= 0) return;
      ++countOvr;
      countOvr *= 4;
      input.resize(countOvr);
      memset(input.data(), 255, countOvr * sizeof(int));
      if ((f = fopen(ovr, "r")) != NULL)
      {
        while (fgets(linein, 80, f) != NULL)
        {
          if (linein[0] == 0 || linein[0] == '\n' || linein[0] == '\r' ||
            linein[0] == ';' || linein[0] == '#') continue;
          linep = linein;
          while (*linep != ',' && *linep != 0 && *linep != ' ') linep++;
          if (*linep == ',')
          {
            sscanf(linein, "%d,%d", &z, &w);
            if (w == 0) w = nfrms;
            if (z<0 || z>nfrms || w<0 || w>nfrms || w < z)
            {
              input.clear();
              fclose(f);
              f = NULL;
              env->ThrowError("TDeint: ovr input error (invalid frame range)!");
            }
            linep = linein;
            while (*linep != ' ' && *linep != 0) linep++;
            if (*linep != 0)
            {
              linep++;
              if (*linep == 'f' || *linep == 'o' || *linep == 'l' || *linep == 'c' || *linep == 't')
              {
                q = *linep;
                linep += 2;
                if (*linep == 0)
                {
                  input.clear();
                  f = NULL;
                  env->ThrowError("TDeint:  ovr input error (no change value specified)!");
                }
                sscanf(linep, "%d", &b);
                if (q == 102 && b != 0 && b != 1)
                {
                  input.clear();
                  fclose(f);
                  f = NULL;
                  env->ThrowError("TDeint:  ovr input error (bad field value)!");
                }
                else if (q == 111 && b != 0 && b != 1)
                {
                  input.clear();
                  fclose(f);
                  f = NULL;
                  env->ThrowError("TDeint:  ovr input error (bad order value)!");
                }
                else if (q == 116 && (b < 0 || b > 5))
                {
                  input.clear();
                  fclose(f);
                  f = NULL;
                  env->ThrowError("TDeint:  ovr input error (bad type value)!");
                }
                input[i] = q; ++i;
                input[i] = z; ++i;
                input[i] = w; ++i;
                input[i] = b; ++i;
              }
              else if (*linep == '+' || *linep == '-')
              {
                if (*(linep + 1) == '+' || *(linep + 1) == '-')
                {
                  track = z; count = 0;
                  while ((*linep == '+' || *linep == '-') && (track <= w))
                  {
                    q = *linep;
                    input[i] = q; ++i;
                    input[i] = track; ++i;
                    input[i] = track; ++i; ++i;
                    ++count; ++track;
                    linep++;
                  }
                  while (track <= w)
                  {
                    input[i] = input[i - (count * 4)]; ++i;
                    input[i] = track; ++i;
                    input[i] = track; ++i; ++i;
                    ++track;
                  }
                }
                else
                {
                  q = *linep;
                  input[i] = q; ++i;
                  input[i] = z; ++i;
                  input[i] = w; ++i; ++i;
                }
              }
              else
              {
                input.clear();
                fclose(f);
                f = NULL;
                env->ThrowError("TDeint:  ovr input error (bad specifier)!");
              }
            }
            else
            {
              input.clear();
              fclose(f);
              f = NULL;
              env->ThrowError("TDeint:  ovr input error (no space after frame range)!");
            }
          }
          else if (*linep == ' ')
          {
            sscanf(linein, "%d", &z);
            if (z<0 || z>nfrms)
            {
              input.clear();
              fclose(f);
              f = NULL;
              env->ThrowError("TDeint: ovr input error (out of range frame #)!");
            }
            linep = linein;
            while (*linep != ' ' && *linep != 0) linep++;
            if (*linep != 0)
            {
              linep++;
              q = *linep;
              input[i] = q; ++i;
              input[i] = z; ++i;
              input[i] = z; ++i;
              if (*linep == 'f' || *linep == 'o' || *linep == 'l' || *linep == 'c' || *linep == 't')
              {
                linep += 2;
                if (*linep == 0)
                {
                  input.clear();
                  fclose(f);
                  f = NULL;
                  env->ThrowError("TDeint:  ovr input error (no change value specified)!");
                }
                sscanf(linep, "%d", &b);
                if (q == 102 && b != 0 && b != 1)
                {
                  input.clear();
                  fclose(f);
                  f = NULL;
                  env->ThrowError("TDeint:  ovr input error (bad field value)!");
                }
                else if (q == 111 && b != 0 && b != 1)
                {
                  input.clear();
                  fclose(f);
                  f = NULL;
                  env->ThrowError("TDeint:  ovr input error (bad order value)!");
                }
                else if (q == 116 && (b < 0 || b > 5))
                {
                  input.clear();
                  fclose(f);
                  f = NULL;
                  env->ThrowError("TDeint:  ovr input error (bad type value)!");
                }
                input[i] = b; ++i;
              }
              else if (*linep == '+' || *linep == '-') ++i;
              else
              {
                input.clear();
                fclose(f);
                f = NULL;
                env->ThrowError("TDeint:  ovr input error (bad specifier)!");
              }
            }
            else
            {
              input.clear();
              fclose(f);
              f = NULL;
              env->ThrowError("TDeint:  ovr input error (no space after frame number)!");
            }
          }
          else
          {
            input.clear();
            fclose(f);
            f = NULL;
            env->ThrowError("TDeint:  ovr input error (invalid line)!");
          }
        }
        fclose(f);
        f = NULL;
      }
      else
      {
        input.clear();
        env->ThrowError("TDeint: ovr input error (cannot open file)!");
      }
    }
    else env->ThrowError("TDeint: ovr input error (cannot open file)!");
  }
}

TDeinterlace::~TDeinterlace()
{
  if (db) delete db;
  if (cArray != NULL) _aligned_free(cArray);
  if (tbuffer) _aligned_free(tbuffer);
}

AVSValue __cdecl Create_TDeinterlace(AVSValue args, void* user_data, IScriptEnvironment* env)
{
  int mode = args[1].AsInt(0);
  if (mode < -2 || mode > 2)
    env->ThrowError("TDeint:  mode must be set to -2, -1, 0, 1, or 2!");
  int order = -1;
  int field = -1;
  int mthreshL = 6;
  int mthreshC = 6;
  int map = 0;
  const char* ovr = "";
  int ovrDefault = 0;
  int type = 2;
  bool debug = false;
  int mtnmode = 1;
  bool sharp = true;
  bool hints = false;
  bool full = true;
  int cthresh = 6;

  bool chroma = args[17].AsBool(false);
  if (args[0].AsClip()->GetVideoInfo().IsY())
    chroma = false;

  int MI = 64;
  bool tryWeave = false;
  int link = 2;
  bool denoise = false;
  int AP = -1;
  int blockx = 16, blocky = 16;
  int APType = 1;
  if (args[0].IsClip())
  {
    unsigned int temp;
    int tfieldHint;
    if (!args[13].IsBool())
    {
      const VideoInfo& vi = args[0].AsClip()->GetVideoInfo();
      PVideoFrame frame = args[0].AsClip()->GetFrame(0, env);
      if (TDeinterlace::getHint(vi, frame, temp, tfieldHint) != -1)
        hints = true;
    }
  }
  PClip v;
  if (args[14].IsClip())
  {
    v = args[14].AsClip();
    try
    {
      v = env->Invoke("InternalCache", v).AsClip();
      v->SetCacheHints(CACHE_GENERIC, 5);
    }
    catch (IScriptEnvironment::NotFound) {}
  }
  TDeinterlace *tdptr = new TDeinterlace(args[0].AsClip(), mode, args[2].AsInt(order),
    args[3].AsInt(field), args[4].AsInt(mthreshL), args[5].AsInt(mthreshC), args[6].AsInt(map),
    args[7].AsString(ovr), args[8].AsInt(ovrDefault), args[9].AsInt(type), args[10].AsBool(debug),
    args[11].AsInt(mtnmode), args[12].AsBool(sharp), args[13].AsBool(hints), args[14].IsClip() ? v : NULL,
    args[15].AsBool(full), args[16].AsInt(cthresh), chroma, args[18].AsInt(MI),
    args[19].AsBool(tryWeave), args[20].AsInt(link), args[21].AsBool(denoise), args[22].AsInt(AP),
    args[23].AsInt(blockx), args[24].AsInt(blocky), args[25].AsInt(APType),
    args[26].IsClip() ? args[26].AsClip() : NULL, args[27].IsClip() ? args[27].AsClip() : NULL,
    args[29].AsInt(0), args[30].AsInt(0), args[31].AsInt(1), args[32].IsClip() ? args[32].AsClip() : NULL,
    args[33].AsBool(false), args[34].AsInt(4), env);
  AVSValue ret = tdptr;
  if (mode == 2)
  {
    try
    {
      ret = env->Invoke("InternalCache", ret.AsClip()).AsClip();
      ret.AsClip()->SetCacheHints(CACHE_GENERIC, 3);
    }
    catch (IScriptEnvironment::NotFound) {}

    ret = new TDHelper(ret.AsClip(), args[2].AsInt(order), args[3].AsInt(field),
      args[28].AsFloat(-2.0), args[10].AsBool(debug), args[34].AsInt(4), tdptr->sa,
      args[31].AsInt(1), tdptr, env);

  }
  return ret;
}

AVSValue __cdecl Create_TSwitch(AVSValue args, void* user_data, IScriptEnvironment* env);

/* New 2.6 requirement!!! */
// Declare and initialise server pointers static storage.
//const AVS_Linkage *AVS_linkage = 0;

//extern "C" __declspec(dllexport) const char* __stdcall
//AvisynthPluginInit3(IScriptEnvironment* env, const AVS_Linkage* const vectors) {

  /* New 2.6 requirment!!! */
  // Save the server pointers.
  /*AVS_linkage = vectors;
  env->AddFunction("TDeint", "c[mode]i[order]i[field]i[mthreshL]i[mthreshC]i[map]i[ovr]s" \
    "[ovrDefault]i[type]i[debug]b[mtnmode]i[sharp]b[hints]b[clip2]c[full]b[cthresh]i" \
    "[chroma]b[MI]i[tryWeave]b[link]i[denoise]b[AP]i[blockx]i[blocky]i[APType]i[edeint]c" \
    "[emask]c[blim]f[metric]i[expand]i[slow]i[emtn]c[tshints]b[opt]i", Create_TDeinterlace, 0);
  env->AddFunction("TSwitch", "c[c1]c[c2]c[debug]b", Create_TSwitch, 0);
  return 0;
}*/
