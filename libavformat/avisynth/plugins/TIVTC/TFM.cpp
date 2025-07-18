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

#include <cstring>
#include "TFM.h"
#include "TFMasm.h"
#include "TCommonASM.h"
#include "../../avs/alignment.h"
#include "info.h"

enum _FieldBased {
    Progressive = 0,
    BottomFieldFirst = 1,
    TopFieldFirst = 2
};

PVideoFrame __stdcall TFM::GetFrame(int n, IScriptEnvironment* env)
{
  if (n < 0) n = 0;
  else if (n > nfrms) n = nfrms;
  PVideoFrame prv = child->GetFrame(n > 0 ? n - 1 : 0, env);
  PVideoFrame src = child->GetFrame(n, env);
  PVideoFrame nxt = child->GetFrame(n < nfrms ? n + 1 : nfrms, env);
  PVideoFrame dst = has_at_least_v8 ? env->NewVideoFrameP(vi, &src) : env->NewVideoFrame(vi);
  PVideoFrame tmp = env->NewVideoFrame(vi);
  int dfrm = -20, tfrm = -20;
  int mmatch1, nmatch1, nmatch2, mmatch2, fmatch, tmatch;
  int combed = -1, tcombed = -1, xblocks = -20;
  bool d2vfilm = false, d2vmatch = false, isSC = true;
  int mics[5] = { -20, -20, -20, -20, -20 };
  int blockN[5] = { -20, -20, -20, -20, -20 };
  order = order_origSaved;
  mode = mode_origSaved;
  field = field_origSaved;
  PP = PP_origSaved;
  MI = MI_origSaved;
  getSettingOvr(n); // process overrides

  if (order == -1) {
    // frame property precedence
    if (has_at_least_v8) {
      const AVSMap* props = env->getFramePropsRO(src);
      int err;

      int64_t field_based = env->propGetInt(props, "_FieldBased", 0, &err);
      if (err) { // prop not present
        order = child->GetParity(n) ? 1 : 0; // fallback to traditional Avisynth
        /* VapourSynth port case: strict frame prop dependency
          env->ThrowError("TFM: Couldn't find the '_FieldBased' frame property. The 'order' parameter must be used.");
          In Avisynth we have Parity info
        */
      }
      else {
        /// Pretend it's top field first when it says progressive?
        order = (field_based == TopFieldFirst || field_based == Progressive);
      }
    }
    else {
      order = child->GetParity(n) ? 1 : 0; // no frame prop support
    }
  }
  if (field == -1) field = order;
  int frstT = field^order ? 2 : 0;
  int scndT = (mode == 2 || mode == 6) ? (field^order ? 3 : 4) : (field^order ? 0 : 2);
  if (debug)
  {
    sprintf(buf, "TFM:  ----------------------------------------\n");
    OutputDebugString(buf);
  }
  if (getMatchOvr(n, fmatch, combed, d2vmatch,
    flags == 5 ? checkSceneChange(prv, src, nxt, n) : false))
  {
    createWeaveFrame(dst, prv, src, nxt, fmatch, dfrm, env, vi);
    if (PP > 0 && combed == -1)
    {
      if (checkCombed(dst, n, vi, fmatch, blockN, xblocks, mics, false, chroma, cthresh))
      {
        if (d2vmatch)
        {
          d2vmatch = false;
          for (int j = 0; j < 5; ++j)
            mics[j] = -20;
          goto d2vCJump;
        }
        else combed = 2;
      }
      else combed = 0;
    }
    d2vfilm = d2vduplicate(fmatch, combed, n);
    if (micout > 0)
    {
      for (int i = 0; i < 5; ++i)
      {
        if (mics[i] == -20 && (i < 3 || micout > 1))
        {
          createWeaveFrame(tmp, prv, src, nxt, i, tfrm, env, vi);
          checkCombed(tmp, n, vi, i, blockN, xblocks, mics, true, chroma, cthresh);
        }
      }
    }
    fileOut(fmatch, combed, d2vfilm, n, mics[fmatch], mics);
    if (display) writeDisplay(dst, vi, n, fmatch, combed, true, blockN[fmatch], xblocks,
      d2vmatch, mics, prv, src, nxt, env);
    if (debug)
    {
      char buft[20];
      if (mics[fmatch] < 0) sprintf(buft, "N/A");
      else sprintf(buft, "%d", mics[fmatch]);
      sprintf(buf, "TFM:  frame %d  - final match = %c %s  MIC = %s  (OVR)\n", n, MTC(fmatch),
        d2vmatch ? "(D2V)" : "", buft);
      OutputDebugString(buf);
      if (micout > 0)
      {
        if (micout > 1)
          sprintf(buf, "TFM:  frame %d  - mics: p = %d  c = %d  n = %d  b = %d  u = %d\n",
            n, mics[0], mics[1], mics[2], mics[3], mics[4]);
        else
          sprintf(buf, "TFM:  frame %d  - mics: p = %d  c = %d  n = %d\n",
            n, mics[0], mics[1], mics[2]);
        OutputDebugString(buf);
      }
      sprintf(buf, "TFM:  frame %d  - mode = %d  field = %d  order = %d  d2vfilm = %c\n", n, mode, field, order,
        d2vfilm ? 'T' : 'F');
      OutputDebugString(buf);
      if (combed != -1)
      {
        if (combed == 1) sprintf(buf, "TFM:  frame %d  - CLEAN FRAME  (forced!)\n", n);
        else if (combed == 5) sprintf(buf, "TFM:  frame %d  - COMBED FRAME  (forced!)\n", n);
        else if (combed == 0) sprintf(buf, "TFM:  frame %d  - CLEAN FRAME\n", n);
        else sprintf(buf, "TFM:  frame %d  - COMBED FRAME\n", n);
        OutputDebugString(buf);
      }
    }
    if (usehints || PP >= 2) {
#ifdef DONT_USE_FRAMEPROPS
      putHint(vi, dst, fmatch, combed, d2vfilm);
#else
      if(has_at_least_v8)
        putFrameProperties(dst, fmatch, combed, d2vfilm, mics, env);
      else
        putHint(vi, dst, fmatch, combed, d2vfilm);
#endif
    }
    lastMatch.frame = n;
    lastMatch.match = fmatch;
    lastMatch.field = field;
    lastMatch.combed = combed;
    return dst;
  }
d2vCJump:
  if (mode == 6)
  {
    int thrdT = field^order ? 0 : 2;
    int frthT = field^order ? 4 : 3;
    tcombed = 0;
    if (!slow) fmatch = compareFields(prv, src, nxt, 1, frstT, nmatch1, nmatch2, mmatch1, mmatch2, vi, n);
    else fmatch = compareFieldsSlow(prv, src, nxt, 1, frstT, nmatch1, nmatch2, mmatch1, mmatch2, vi, n);
    if (micmatching > 0)
      checkmm(fmatch, 1, frstT, dst, dfrm, tmp, tfrm, prv, src, nxt, env, vi, n, blockN, xblocks, mics);
    createWeaveFrame(dst, prv, src, nxt, fmatch, dfrm, env, vi);
    if (checkCombed(dst, n, vi, fmatch, blockN, xblocks, mics, false, chroma, cthresh))
    {
      tcombed = 2;
      if (ubsco) isSC = checkSceneChange(prv, src, nxt, n);
      if (isSC) createWeaveFrame(tmp, prv, src, nxt, scndT, tfrm, env, vi);
      if (isSC && !checkCombed(tmp, n, vi, scndT, blockN, xblocks, mics, false, chroma, cthresh))
      {
        fmatch = scndT;
        tcombed = 0;
        copyFrame(dst, tmp, vi);
        dfrm = fmatch;
      }
      else
      {
        createWeaveFrame(tmp, prv, src, nxt, thrdT, tfrm, env, vi);
        if (!checkCombed(tmp, n, vi, thrdT, blockN, xblocks, mics, false, chroma, cthresh))
        {
          fmatch = thrdT;
          tcombed = 0;
          copyFrame(dst, tmp, vi);
          dfrm = fmatch;
        }
        else
        {
          if (isSC) createWeaveFrame(tmp, prv, src, nxt, frthT, tfrm, env, vi);
          if (isSC && !checkCombed(tmp, n, vi, frthT, blockN, xblocks, mics, false, chroma, cthresh))
          {
            fmatch = frthT;
            tcombed = 0;
            copyFrame(dst, tmp, vi);
            dfrm = fmatch;
          }
        }
      }
    }
    if (combed == -1 && PP > 0) combed = tcombed;
  }
  else if (mode == 7)
  {
    if (debug && lastMatch.frame != n && n != 0)
    {
      sprintf(buf, "TFM:  mode 7 - non-linear access detected!\n");
      OutputDebugString(buf);
    }
    combed = 0;
    bool combed1 = false, combed2 = false;
    if (!slow) fmatch = compareFields(prv, src, nxt, 1, frstT, nmatch1, nmatch2, mmatch1, mmatch2, vi, n);
    else fmatch = compareFieldsSlow(prv, src, nxt, 1, frstT, nmatch1, nmatch2, mmatch1, mmatch2, vi, n);
    createWeaveFrame(dst, prv, src, nxt, 1, dfrm, env, vi);
    combed1 = checkCombed(dst, n, vi, 1, blockN, xblocks, mics, false, chroma, cthresh);
    createWeaveFrame(dst, prv, src, nxt, frstT, dfrm, env, vi);
    combed2 = checkCombed(dst, n, vi, frstT, blockN, xblocks, mics, false, chroma, cthresh);
    if (!combed1 && !combed2)
    {
      createWeaveFrame(dst, prv, src, nxt, fmatch, dfrm, env, vi);
      if (field == 0) mode7_field = 1;
      else mode7_field = 0;
    }
    else if (!combed2 && combed1)
    {
      createWeaveFrame(dst, prv, src, nxt, frstT, dfrm, env, vi);
      mode7_field = 1;
      fmatch = frstT;
    }
    else if (!combed1 && combed2)
    {
      createWeaveFrame(dst, prv, src, nxt, 1, dfrm, env, vi);
      mode7_field = 0;
      fmatch = 1;
    }
    else
    {
      createWeaveFrame(dst, prv, src, nxt, 1, dfrm, env, vi);
      combed = 2;
      field = mode7_field;
      fmatch = 1;
    }
  }
  else
  {
    if (!slow)
      fmatch = compareFields(prv, src, nxt, 1, frstT, nmatch1, nmatch2, mmatch1, mmatch2, vi, n);
    else
      fmatch = compareFieldsSlow(prv, src, nxt, 1, frstT, nmatch1, nmatch2, mmatch1, mmatch2, vi, n);
    if (micmatching > 0)
      checkmm(fmatch, 1, frstT, dst, dfrm, tmp, tfrm, prv, src, nxt, env, vi, n, blockN, xblocks, mics);
    createWeaveFrame(dst, prv, src, nxt, fmatch, dfrm, env, vi);
    if (mode > 3 || (mode > 0 && checkCombed(dst, n, vi, fmatch, blockN, xblocks, mics, false, chroma, cthresh)))
    {
      if (mode < 4) tcombed = 2;
      if (mode != 2)
      {
        if (!slow)
          tmatch = compareFields(prv, src, nxt, fmatch, scndT, nmatch1, nmatch2, mmatch1, mmatch2, vi, n);
        else
          tmatch = compareFieldsSlow(prv, src, nxt, fmatch, scndT, nmatch1, nmatch2, mmatch1, mmatch2, vi, n);
        if (micmatching > 0)
          checkmm(tmatch, fmatch, scndT, dst, dfrm, tmp, tfrm, prv, src, nxt, env, vi, n, blockN, xblocks, mics);
        createWeaveFrame(dst, prv, src, nxt, fmatch, dfrm, env, vi);
      }
      else tmatch = scndT;
      if (tmatch == scndT)
      {
        if (mode > 3)
        {
          fmatch = tmatch;
          createWeaveFrame(dst, prv, src, nxt, fmatch, dfrm, env, vi);
        }
        else if (mode != 2 || !ubsco || checkSceneChange(prv, src, nxt, n))
        {
          createWeaveFrame(tmp, prv, src, nxt, tmatch, tfrm, env, vi);
          if (!checkCombed(tmp, n, vi, tmatch, blockN, xblocks, mics, false, chroma, cthresh))
          {
            fmatch = tmatch;
            tcombed = 0;
            copyFrame(dst, tmp, vi);
            dfrm = fmatch;
          }
        }
      }
      if ((mode == 3 && tcombed == 2) || (mode == 5 && checkCombed(dst, n, vi, fmatch, blockN, xblocks, mics, false, chroma, cthresh)))
      {
        tcombed = 2;
        if (!ubsco || checkSceneChange(prv, src, nxt, n))
        {
          if (!slow)
            tmatch = compareFields(prv, src, nxt, 3, 4, nmatch1, nmatch2, mmatch1, mmatch2, vi, n);
          else
            tmatch = compareFieldsSlow(prv, src, nxt, 3, 4, nmatch1, nmatch2, mmatch1, mmatch2, vi, n);
          if (micmatching > 0)
            checkmm(tmatch, 3, 4, dst, dfrm, tmp, tfrm, prv, src, nxt, env, vi, n, blockN, xblocks, mics);
          createWeaveFrame(tmp, prv, src, nxt, tmatch, tfrm, env, vi);
          if (!checkCombed(tmp, n, vi, tmatch, blockN, xblocks, mics, false, chroma, cthresh))
          {
            fmatch = tmatch;
            tcombed = 0;
            copyFrame(dst, tmp, vi);
            dfrm = fmatch;
          }
          else
            createWeaveFrame(dst, prv, src, nxt, fmatch, dfrm, env, vi);
        }
      }
      if (mode == 5 && tcombed == -1) tcombed = 0;
    }
    if ((mode == 1 || mode == 2 || mode == 3) && tcombed == -1) tcombed = 0;
    if (combed == -1 && PP > 0) combed = tcombed;
    if (PP > 0 && combed == -1)
    {
      if (checkCombed(dst, n, vi, fmatch, blockN, xblocks, mics, false, chroma, cthresh)) combed = 2;
      else combed = 0;
    }
    if (dfrm != fmatch)
      env->ThrowError("TFM:  internal error (dfrm!=fmatch).  Please report this.\n");
  }
  if (micout > 0 || (micmatching > 0 && mics[fmatch] > 15 && mode != 7 && !(micmatching == 2 && (mode == 0 || mode == 4))
    && (!mmsco || checkSceneChange(prv, src, nxt, n))))
  {
    for (int i = 0; i < 5; ++i)
    {
      if (mics[i] == -20 && (i < 3 || micout > 1 || micmatching > 0))
      {
        createWeaveFrame(tmp, prv, src, nxt, i, tfrm, env, vi);
        checkCombed(tmp, n, vi, i, blockN, xblocks, mics, true, chroma, cthresh);
      }
    }
    if (micmatching > 0 && mode != 7 && mics[fmatch] > 15 &&
      (!mmsco || checkSceneChange(prv, src, nxt, n)))
    {
      int i, j, temp1, temp2, order1[5], order2[5] = { 0, 1, 2, 3, 4 };
      for (i = 0; i < 5; ++i) order1[i] = mics[i];
      for (i = 1; i < 5; ++i)
      {
        j = i;
        temp1 = order1[j];
        temp2 = order2[j];
        while (j > 0 && order1[j - 1] > temp1)
        {
          order1[j] = order1[j - 1];
          order2[j] = order2[j - 1];
          --j;
        }
        order1[j] = temp1;
        order2[j] = temp2;
      }
      if (micmatching == 1)
      {
      othertest:
        if (order1[0] * 3 < order1[1] && abs(order1[0] - order1[1]) > 15 &&
          order1[0] < MI && order2[0] != fmatch &&
          (((field^order) && (order2[0] == 1 || order2[0] == 2 || order2[0] == 3)) ||
          (!(field^order) && (order2[0] == 0 || order2[0] == 1 || order2[0] == 4))))
        {
          bool xfield = (field^order) == 0 ? false : true;
          int lmatch = lastMatch.frame == n - 1 ? lastMatch.match : -20;
          if (!((order2[0] == 4 && lmatch == 0 && !xfield && (order2[1] == 0 || order2[2] == 0)) ||
            (order2[0] == 3 && lmatch == 2 && xfield && (order2[1] == 2 || order2[2] == 2))))
          {
            micChange(n, fmatch, order2[0], dst, prv, src, nxt, env, vi,
              fmatch, combed, dfrm);
          }
        }
        if (order1[0] * 4 < order1[1] && abs(order1[0] - order1[1]) > 30 &&
          order1[0] < MI && order1[1] >= MI && order2[0] != fmatch)
        {
          micChange(n, fmatch, order2[0], dst, prv, src, nxt, env, vi,
            fmatch, combed, dfrm);
        }
      }
      else if (micmatching == 2 || micmatching == 3)
      {
        int try1 = field^order ? 2 : 0, try2, minm, mint, try3, try4;
        if (mode == 1) // p/c + n
        {
          try2 = try1 == 2 ? 0 : 2;
          minm = std::min(mics[1], mics[try1]);
          if (mics[try2] * 3 < minm && mics[try2] < MI && abs(mics[try2] - minm) >= 30 && try2 != fmatch)
            micChange(n, fmatch, try2, dst, prv, src, nxt, env, vi,
              fmatch, combed, dfrm);
        }
        else if (mode == 2) // p/c + u
        {
          try2 = try1 == 2 ? 3 : 4;
          minm = std::min(mics[1], mics[try1]);
          if (mics[try2] * 3 < minm && mics[try2] < MI && abs(mics[try2] - minm) >= 30 && try2 != fmatch)
            micChange(n, fmatch, try2, dst, prv, src, nxt, env, vi,
              fmatch, combed, dfrm);
        }
        else if (mode == 3) // p/c + n + u/b
        {
          try2 = try1 == 2 ? 0 : 2;
          minm = std::min(mics[1], mics[try1]);
          mint = std::min(mics[3], mics[4]);
          try3 = try1 == 2 ? (mint == mics[3] ? 3 : 4) : (mint == mics[4] ? 4 : 3);
          if (mics[try2] * 3 < minm && mics[try2] < MI && abs(mics[try2] - minm) >= 30 && try2 != fmatch &&
            fmatch != 3 && fmatch != 4)
          {
            micChange(n, fmatch, try2, dst, prv, src, nxt, env, vi,
              fmatch, combed, dfrm);
            minm = mics[try2];
          }
          else if (fmatch == try2) minm = std::min(mics[try2], minm);
          if (mint * 3 < minm && mint < MI && abs(mint - minm) >= 30 && fmatch != 3 && fmatch != 4)
            micChange(n, fmatch, try3, dst, prv, src, nxt, env, vi,
              fmatch, combed, dfrm);
        }
        else if (mode == 5) // p/c/n + u/b
        {
          minm = std::min(mics[0], std::min(mics[1], mics[2]));
          mint = std::min(mics[3], mics[4]);
          try3 = try1 == 2 ? (mint == mics[3] ? 3 : 4) : (mint == mics[4] ? 4 : 3);
          if (mint * 3 < minm && mint < MI && abs(mint - minm) >= 30 && fmatch != 3 && fmatch != 4)
            micChange(n, fmatch, try3, dst, prv, src, nxt, env, vi,
              fmatch, combed, dfrm);
        }
        else if (mode == 6) // p/c + u + n + b
        {
          try2 = try1 == 2 ? 3 : 4;
          try3 = try1 == 2 ? 0 : 2;
          try4 = try2 == 3 ? 4 : 3;
          minm = std::min(mics[1], mics[try1]);
          if (mics[try2] * 3 < minm && mics[try2] < MI && abs(mics[try2] - minm) >= 30 && fmatch != try2 &&
            fmatch != try3 && fmatch != try4)
          {
            micChange(n, fmatch, try2, dst, prv, src, nxt, env, vi,
              fmatch, combed, dfrm);
            minm = mics[try2];
          }
          else if (fmatch == try2) minm = std::min(mics[try2], minm);
          if (mics[try3] * 3 < minm && mics[try3] < MI && abs(mics[try3] - minm) >= 30 && fmatch != try3 &&
            fmatch != try4)
          {
            micChange(n, fmatch, try3, dst, prv, src, nxt, env, vi,
              fmatch, combed, dfrm);
            minm = mics[try3];
          }
          else if (fmatch == try3) minm = std::min(mics[try3], minm);
          if (mics[try4] * 3 < minm && mics[try4] < MI && abs(mics[try4] - minm) >= 30 && fmatch != try4)
            micChange(n, fmatch, try4, dst, prv, src, nxt, env, vi,
              fmatch, combed, dfrm);
        }
        if (micmatching == 3) { goto othertest; }
      }
    }
  }
  d2vfilm = d2vduplicate(fmatch, combed, n);
  fileOut(fmatch, combed, d2vfilm, n, mics[fmatch], mics);
  if (display) writeDisplay(dst, vi, n, fmatch, combed, false, blockN[fmatch], xblocks,
    d2vmatch, mics, prv, src, nxt, env);
  if (debug)
  {
    char buft[20];
    if (mics[fmatch] < 0) sprintf(buft, "N/A");
    else sprintf(buft, "%d", mics[fmatch]);
    sprintf(buf, "TFM:  frame %d  - final match = %c  MIC = %s\n", n, MTC(fmatch), buft);
    OutputDebugString(buf);
    if (micout > 0 || (micmatching > 0 && mics[0] != -20 && mics[1] != -20 && mics[2] != -20
      && mics[3] != -20 && mics[4] != -20))
    {
      if (micout > 1 || micmatching > 0)
        sprintf(buf, "TFM:  frame %d  - mics: p = %d  c = %d  n = %d  b = %d  u = %d\n",
          n, mics[0], mics[1], mics[2], mics[3], mics[4]);
      else
        sprintf(buf, "TFM:  frame %d  - mics: p = %d  c = %d  n = %d\n",
          n, mics[0], mics[1], mics[2]);
      OutputDebugString(buf);
    }
    sprintf(buf, "TFM:  frame %d  - mode = %d  field = %d  order = %d  d2vfilm = %c\n", n, mode, field, order,
      d2vfilm ? 'T' : 'F');
    OutputDebugString(buf);
    if (combed != -1)
    {
      if (combed == 1) sprintf(buf, "TFM:  frame %d  - CLEAN FRAME  (forced!)\n", n);
      else if (combed == 5) sprintf(buf, "TFM:  frame %d  - COMBED FRAME  (forced!)\n", n);
      else if (combed == 0) sprintf(buf, "TFM:  frame %d  - CLEAN FRAME\n", n);
      else sprintf(buf, "TFM:  frame %d  - COMBED FRAME\n", n);
      OutputDebugString(buf);
    }
  }
  if (usehints || PP >= 2) {
#ifdef DONT_USE_FRAMEPROPS
      putHint(vi, dst, fmatch, combed, d2vfilm);
#else
      if(has_at_least_v8)
        putFrameProperties(dst, fmatch, combed, d2vfilm, mics, env);
      else
        putHint(vi, dst, fmatch, combed, d2vfilm);
#endif
  }
  lastMatch.frame = n;
  lastMatch.match = fmatch;
  lastMatch.field = field;
  lastMatch.combed = combed;
  return dst;
}

void TFM::checkmm(int &cmatch, int m1, int m2, PVideoFrame &dst, int &dfrm, PVideoFrame &tmp, int &tfrm,
  PVideoFrame &prv, PVideoFrame &src, PVideoFrame &nxt, IScriptEnvironment *env, const VideoInfo &vi, int n,
  int *blockN, int &xblocks, int *mics)
{
  if (cmatch != m1)
  {
    int tx = m1;
    m1 = m2;
    m2 = tx;
  }
  if (dfrm == m1)
    checkCombed(dst, n, vi, m1, blockN, xblocks, mics, false, chroma, cthresh);
  else if (tfrm == m1)
    checkCombed(tmp, n, vi, m1, blockN, xblocks, mics, false, chroma, cthresh);
  else
  {
    if (tfrm != m2)
    {
      createWeaveFrame(tmp, prv, src, nxt, m1, tfrm, env, vi);
      checkCombed(tmp, n, vi, m1, blockN, xblocks, mics, false, chroma, cthresh);
    }
    else
    {
      createWeaveFrame(dst, prv, src, nxt, m1, dfrm, env, vi);
      checkCombed(dst, n, vi, m1, blockN, xblocks, mics, false, chroma, cthresh);
    }
  }
  if (mics[m1] < 30)
    return;
  if (dfrm == m2)
    checkCombed(dst, n, vi, m2, blockN, xblocks, mics, false, chroma, cthresh);
  else if (tfrm == m2)
    checkCombed(tmp, n, vi, m2, blockN, xblocks, mics, false, chroma, cthresh);
  else
  {
    if (tfrm != m1)
    {
      createWeaveFrame(tmp, prv, src, nxt, m2, tfrm, env, vi);
      checkCombed(tmp, n, vi, m2, blockN, xblocks, mics, false, chroma, cthresh);
    }
    else
    {
      createWeaveFrame(dst, prv, src, nxt, m2, dfrm, env, vi);
      checkCombed(dst, n, vi, m2, blockN, xblocks, mics, false, chroma, cthresh);
    }
  }
  if ((mics[m2] * 3 < mics[m1] || (mics[m2] * 2 < mics[m1] && mics[m1] > MI)) &&
    abs(mics[m2] - mics[m1]) >= 30 && mics[m2] < MI)
  {
    if (debug)
    {
      sprintf(buf, "TFM:  frame %d  - micmatching override:  %c (%d) to %c (%d)\n", n,
        MTC(m1), mics[m1], MTC(m2), mics[m2]);
      OutputDebugString(buf);
    }
    cmatch = m2;
  }
}

void TFM::micChange(int n, int m1, int m2, PVideoFrame &dst, PVideoFrame &prv,
  PVideoFrame &src, PVideoFrame &nxt, IScriptEnvironment *env, const VideoInfo &vi, int &fmatch,
  int &combed, int &cfrm)
{
  if (debug)
  {
    sprintf(buf, "TFM:  frame %d  - micmatching override:  %c to %c\n", n,
      MTC(m1), MTC(m2));
    OutputDebugString(buf);
  }
  fmatch = m2;
  combed = 0;
  createWeaveFrame(dst, prv, src, nxt, m2, cfrm, env, vi);
}

void TFM::writeDisplay(PVideoFrame &dst, const VideoInfo& vi_disp, int n, int fmatch, int combed, bool over,
  int blockN, int xblocks, bool d2vmatch, int *mics, PVideoFrame &prv,
  PVideoFrame &src, PVideoFrame &nxt, IScriptEnvironment *env)
{
  if (combed > 1 && PP > 1) return; // TFMPP will display things instead
  if (combed > 1 && PP == 1 && blockN != -20)
  {
    drawBox(dst, blockx, blocky, blockN, xblocks, vi_disp);
  }
  sprintf(buf, "TFM %s by tritical ", TFM_VERSION);
  Draw(dst, 0, 0, buf, vi_disp);
  if (PP > 0)
    sprintf(buf, "order = %d  field = %d  mode = %d  MI = %d ", order, field, mode, MI);
  else
    sprintf(buf, "order = %d  field = %d  mode = %d ", order, field, mode);
  Draw(dst, 0, 1, buf, vi_disp);
  if (!over && !d2vmatch) sprintf(buf, "frame: %d  match = %c %s", n, MTC(fmatch),
    ((ubsco || mmsco || flags == 5) && checkSceneChange(prv, src, nxt, n)) ? " (SC) " : "");
  else if (d2vmatch) sprintf(buf, "frame: %d  match = %c (D2V) %s", n, MTC(fmatch),
    ((ubsco || mmsco || flags == 5) && checkSceneChange(prv, src, nxt, n)) ? " (SC) " : "");
  else sprintf(buf, "frame: %d  match = %c (OVR) %s", n, MTC(fmatch),
    ((ubsco || mmsco || flags == 5) && checkSceneChange(prv, src, nxt, n)) ? " (SC) " : "");
  Draw(dst, 0, 2, buf, vi_disp);
  int i = 3;
  if (micout > 0 || (micmatching > 0 && mics[0] != -20 && mics[1] != -20 && mics[2] != -20
    && mics[3] != -20 && mics[4] != -20))
  {
    if (micout == 1 && mics[0] != -20 && mics[1] != -20 && mics[2] != -20 && micmatching == 0)
    {
      sprintf(buf, "MICS:  p = %d  c = %d  n = %d ", mics[0], mics[1], mics[2]);
      Draw(dst, 0, i, buf, vi_disp);
      ++i;
    }
    else if ((micout == 2 && mics[0] != -20 && mics[1] != -20 && mics[2] != -20 &&
      mics[3] != -20 && mics[4] != -20) || micmatching > 0)
    {
      sprintf(buf, "MICS:  p = %d  c = %d  n = %d ", mics[0], mics[1], mics[2]);
      Draw(dst, 0, i, buf, vi_disp);
      ++i;
      sprintf(buf, "       b = %d  u = %d ", mics[3], mics[4]);
      Draw(dst, 0, i, buf, vi_disp);
      ++i;
    }
  }
  if (combed != -1)
  {
    if (combed == 1) sprintf(buf, "PP = %d  CLEAN FRAME (forced!) ", PP);
    else if (combed == 5) sprintf(buf, "PP = %d  COMBED FRAME  (forced!) ", PP);
    else if (combed == 0) sprintf(buf, "PP = %d  CLEAN FRAME ", PP);
    else sprintf(buf, "PP = %d  COMBED FRAME ", PP);
    if (mics[fmatch] >= 0)
    {
      char buft[20];
      sprintf(buft, " MIC = %d ", mics[fmatch]);
      strcat(buf, buft);
    }
    Draw(dst, 0, i, buf, vi_disp);
    ++i;
  }
  if (d2vpercent >= 0.0)
  {
    sprintf(buf, "%3.1f%s FILM (D2V) ", d2vpercent, "%");
    Draw(dst, 0, i, buf, vi_disp);
  }
  /* VS: debug text is not drawn on screen but put into frame property. Kept for reference
  AVSMap *props = env->getFramePropsRW(dst);
  env->propSetData(props, PROP_TFMDisplay, text.c_str(), text.size(), paReplace);
  */
}

// override from ovr file
void TFM::getSettingOvr(int n)
{
  if (setArray.size() == 0) return;
  for (int x = 0; x < (int)setArray.size(); x += 4)
  {
    if (n >= setArray[x + 1] && n <= setArray[x + 2])
    {
      if (setArray[x] == 111) order = setArray[x + 3]; // o
      else if (setArray[x] == 109) mode = setArray[x + 3]; // m
      else if (setArray[x] == 102) field = setArray[x + 3]; // f
      else if (setArray[x] == 80) PP = setArray[x + 3]; // P
      else if (setArray[x] == 105) MI = setArray[x + 3]; // i
    }
  }
}

bool TFM::getMatchOvr(int n, int &match, int &combed, bool &d2vmatch, bool isSC)
{
  bool combedset = false;
  d2vmatch = false;
  if (ovrArray.size() && ovrArray[n] != 255)
  {
    int value = ovrArray[n], temp;
    temp = value & 0x00000020;
    if (temp == 0 && PP > 0)
    {
      if (value & 0x00000010) combed = 5;
      else combed = 1;
      combedset = true;
    }
    temp = value & 0x00000007;
    if (temp >= 0 && temp <= 6)
    {
      match = temp;
      if (field != fieldO)
      {
        if (match == 0) match = 3;
        else if (match == 2) match = 4;
        else if (match == 3) match = 0;
        else if (match == 4) match = 2;
      }
      if (match == 5) { combed = 5; match = 1; field = 0; }
      else if (match == 6) { combed = 5; match = 1; field = 1; }
      return true;
    }
  }
  if (flags != 0 && flags != 3 && d2vfilmarray.size() && (d2vfilmarray[n] & D2VARRAY_MATCH_MASK))
  {
    int ct = (flags == 4 || (flags == 5 && isSC)) ? -1 : 0;
    int temp = d2vfilmarray[n];
    if ((flags == 1 || flags == 4 || flags == 5) && !(temp&(0x1 << 6))) return false;
    temp = (temp&D2VARRAY_MATCH_MASK) >> 2;
    if (temp != 1 && temp != 2) return false;
    if (temp == 1) { match = 1; combed = combedset ? combed : ct; }
    else if (temp == 2) { match = field^order ? 2 : 0; combed = combedset ? combed : ct; }
    d2vmatch = true;
    return true;
  }
  return false;
}

bool TFM::d2vduplicate(int match, int combed, int n)
{
  if (d2vfilmarray.size() == 0 || d2vfilmarray[n] == 0) return false;
  if (n - 1 != lastMatch.frame)
    lastMatch.field = lastMatch.frame = lastMatch.combed = lastMatch.match = -20;
  if ((d2vfilmarray[n] & D2VARRAY_DUP_MASK) == 0x3) // indicates possible top field duplicate
  {
    if (lastMatch.field == 1)
    {
      if ((lastMatch.combed > 1 || lastMatch.match != 3) && field == 1 &&
        (match != 4 || combed > 1)) return true;
      else if ((lastMatch.combed > 1 || lastMatch.match != 3) && field == 0 &&
        combed < 2 && match != 2) return true;
    }
    else if (lastMatch.field == 0)
    {
      if (lastMatch.combed < 2 && lastMatch.match != 0 && field == 1 &&
        (match != 4 || combed > 1)) return true;
      else if (lastMatch.combed < 2 && lastMatch.match != 0 && field == 0 &&
        combed < 2 && match != 2) return true;
    }
  }
  else if ((d2vfilmarray[n] & D2VARRAY_DUP_MASK) == 0x1) // indicates possible bottom field duplicate
  {
    if (lastMatch.field == 1)
    {
      if (lastMatch.combed < 2 && lastMatch.match != 0 && field == 0 &&
        (match != 4 || combed > 1)) return true;
      else if (lastMatch.combed < 2 && lastMatch.match != 0 && field == 1 &&
        combed < 2 && match != 2) return true;
    }
    else if (lastMatch.field == 0)
    {
      if ((lastMatch.combed > 1 || lastMatch.match != 3) && field == 0 &&
        (match != 4 || combed > 1)) return true;
      else if ((lastMatch.combed > 1 || lastMatch.match != 3) && field == 1 &&
        combed < 2 && match != 2) return true;
    }
  }
  return false;
}

void TFM::fileOut(int match, int combed, bool d2vfilm, int n, int MICount, int mics[5])
{
  if (moutArray.size() && MICount != -1) moutArray[n] = MICount;
  if (micout > 0 && moutArrayE.size())
  {
    int sn = micout == 1 ? 3 : 5;
    for (int i = 0; i < sn; ++i)
      moutArrayE[n*sn + i] = mics[i];
  }
  if (outArray.size() == 0) return;
  if (output.size() || outputC.size())
  {
    if (field != fieldO)
    {
      if (match == 0) match = 3;
      else if (match == 2) match = 4;
      else if (match == 3) match = 0;
      else if (match == 4) match = 2;
    }
    if (match == 1 && combed > 1 && field == 0) match = 5;
    else if (match == 1 && combed > 1 && field == 1) match = 6;
    unsigned char hint = 0;
    hint |= match;
    if (combed > 1) hint |= FILE_COMBED;
    else if (combed >= 0) hint |= FILE_NOTCOMBED;
    if (d2vfilm) hint |= FILE_D2V;
    hint |= FILE_ENTRY;
    outArray[n] = hint;
  }
}


bool TFM::checkCombed(PVideoFrame &src, int n, const VideoInfo &vi, int match,
  int *blockN, int &xblocksi, int *mics, bool ddebug, bool chroma, int cthresh)
{
  if (vi.IsYUY2())
    return checkCombedYUY2(src, n, match, blockN, xblocksi, mics, ddebug, chroma, cthresh);
  else if (vi.IsY())
    return checkCombedPlanar(vi, src, n, match, blockN, xblocksi, mics, ddebug, false, cthresh);
  else if (vi.IsPlanar())
    return checkCombedPlanar(vi, src, n, match, blockN, xblocksi, mics, ddebug, chroma, cthresh);
  return false;
}

int TFM::compareFields(PVideoFrame& prv, PVideoFrame& src, PVideoFrame& nxt, int match1,
  int match2, int& norm1, int& norm2, int& mtn1, int& mtn2, const VideoInfo& vi, int n)
{
  if (vi.ComponentSize() == 1)
    return compareFields_core<uint8_t>(prv, src, nxt, match1, match2, norm1, norm2, mtn1, mtn2, vi, n);
  else
    return compareFields_core<uint16_t>(prv, src, nxt, match1, match2, norm1, norm2, mtn1, mtn2, vi, n);
}


template<typename pixel_t>
int TFM::compareFields_core(PVideoFrame &prv, PVideoFrame &src, PVideoFrame &nxt, int match1,
  int match2, int &norm1, int &norm2, int &mtn1, int &mtn2, const VideoInfo &vi, int n)
{
  (void)n;

  const int bits_per_pixel = vi.BitsPerComponent();

  int ret;
  int y0a, y1a; // exclusion regio

  const int stop = vi.IsYUY2() || vi.IsY() || !mChroma ? 1 : 3;
  const int incl = vi.IsYUY2() && !mChroma ? 2 : 1;  // pixel increments: 2 if YUY2 with no-chroma option otherwise 1

  uint64_t accumPc = 0, accumNc = 0;
  uint64_t accumPm = 0, accumNm = 0;
  norm1 = norm2 = mtn1 = mtn2 = 0;

  const int planes[3] = { PLANAR_Y, PLANAR_U, PLANAR_V };
  for (int b = 0; b < stop; ++b)
  {
    const int plane = planes[b];

    uint8_t *mapp = map->GetPtr(b);
    int map_pitch = map->GetPitch(b);

    const pixel_t* prvp = reinterpret_cast<const pixel_t*>(prv->GetReadPtr(plane));
    const int prv_pitch = prv->GetPitch(plane) / sizeof(pixel_t);

    const pixel_t* srcp = reinterpret_cast<const pixel_t*>(src->GetReadPtr(plane));
    const int src_pitch = src->GetPitch(plane) / sizeof(pixel_t);

    const int Width = src->GetRowSize(plane) / sizeof(pixel_t);
    const int Height = src->GetHeight(plane);

    const pixel_t* nxtp = reinterpret_cast<const pixel_t*>(nxt->GetReadPtr(plane));
    const int nxt_pitch = nxt->GetPitch(plane) / sizeof(pixel_t);

    const int startx = vi.IsYUY2() ? 16 : 8 >> vi.GetPlaneWidthSubsampling(plane);
    const int stopx = Width - startx;

    const pixel_t* prvpf = nullptr, * curf = nullptr, * nxtpf = nullptr;
    int prvf_pitch = 0, curf_pitch = 0, nxtf_pitch = 0;

    curf_pitch = src_pitch << 1;
    // exclusion area limits from parameters
    if (b == 0)
    {
      y0a = y0;
      y1a = y1;
    }
    else
    {
      const int ysubsampling = vi.GetPlaneHeightSubsampling(plane);
      y0a = y0 >> ysubsampling;
      y1a = y1 >> ysubsampling;
    }
    const bool noBandExclusion = (y0a == y1a);
    if (y0a >= 2) y0a = y0a - 2; // v18: real limit, since y goes only till Height-2
    if (y1a <= Height - 2) y1a = y1a + 2; // v18: real limit, since y goes only from 2

    if (match1 < 3)
    {
      curf = srcp + ((3 - field)*src_pitch);
      mapp = mapp + ((field == 1 ? 1 : 2)*map_pitch);
    }
    if (match1 == 0)
    {
      prvf_pitch = prv_pitch << 1;
      prvpf = prvp + ((field == 1 ? 1 : 2)*prv_pitch);
    }
    else if (match1 == 1)
    {
      prvf_pitch = src_pitch << 1;
      prvpf = srcp + ((field == 1 ? 1 : 2)*src_pitch);
    }
    else if (match1 == 2)
    {
      prvf_pitch = nxt_pitch << 1;
      prvpf = nxtp + ((field == 1 ? 1 : 2)*nxt_pitch);
    }
    else if (match1 == 3)
    {
      curf = srcp + ((2 + field)*src_pitch);
      prvf_pitch = prv_pitch << 1;
      prvpf = prvp + ((field == 1 ? 2 : 1)*prv_pitch);
      mapp = mapp + ((field == 1 ? 2 : 1)*map_pitch);
    }
    else if (match1 == 4)
    {
      curf = srcp + ((2 + field)*src_pitch);
      prvf_pitch = nxt_pitch << 1;
      prvpf = nxtp + ((field == 1 ? 2 : 1)*nxt_pitch);
      mapp = mapp + ((field == 1 ? 2 : 1)*map_pitch);
    }
    if (match2 == 0)
    {
      nxtf_pitch = prv_pitch << 1;
      nxtpf = prvp + ((field == 1 ? 1 : 2)*prv_pitch);
    }
    else if (match2 == 1)
    {
      nxtf_pitch = src_pitch << 1;
      nxtpf = srcp + ((field == 1 ? 1 : 2)*src_pitch);
    }
    else if (match2 == 2)
    {
      nxtf_pitch = nxt_pitch << 1;
      nxtpf = nxtp + ((field == 1 ? 1 : 2)*nxt_pitch);
    }
    else if (match2 == 3)
    {
      nxtf_pitch = prv_pitch << 1;
      nxtpf = prvp + ((field == 1 ? 2 : 1)*prv_pitch);
    }
    else if (match2 == 4)
    {
      nxtf_pitch = nxt_pitch << 1;
      nxtpf = nxtp + ((field == 1 ? 2 : 1)*nxt_pitch);
    }

    const pixel_t* prvnf = prvpf + prvf_pitch;
    const pixel_t* curpf = curf - curf_pitch;
    const pixel_t* curnf = curf + curf_pitch;
    const pixel_t* nxtnf = nxtpf + nxtf_pitch;

    map_pitch <<= 1;
    uint8_t* mapn = mapp + map_pitch;

    // back to byte pointers
    if ((match1 >= 3 && field == 1) || (match1 < 3 && field != 1))
      buildDiffMapPlane2<pixel_t>(
        reinterpret_cast<const uint8_t*>(prvpf - prvf_pitch),
        reinterpret_cast<const uint8_t*>(nxtpf - nxtf_pitch),
        mapp - map_pitch,
        prvf_pitch * sizeof(pixel_t),
        nxtf_pitch * sizeof(pixel_t),
        map_pitch, Height >> 1, Width, bits_per_pixel);
    else
      buildDiffMapPlane2<pixel_t>(
        reinterpret_cast<const uint8_t*>(prvnf - prvf_pitch),
        reinterpret_cast<const uint8_t*>(nxtnf - nxtf_pitch),
        mapn - map_pitch,
        prvf_pitch * sizeof(pixel_t),
        nxtf_pitch * sizeof(pixel_t),
        map_pitch, Height >> 1, Width, bits_per_pixel);

    const int Const23 = 23 << (bits_per_pixel - 8);
    const int Const42 = 42 << (bits_per_pixel - 8);

    // TFM 874
    for (int y = 2; y < Height - 2; y += 2) {
      if ((y < y0a) || noBandExclusion || (y > y1a))  // exclusion area check
      {
        for (int x = startx; x < stopx; x += incl)
        {
          int eax = (mapp[x] << 2) + mapn[x];
          if ((eax & 0xFF) == 0)
            continue;

          int a_curr = curpf[x] + (curf[x] << 2) + curnf[x];
          int a_prev = 3 * (prvpf[x] + prvnf[x]);
          int diff_p_c = abs(a_prev - a_curr);
          if (diff_p_c > Const23) {
            accumPc += diff_p_c;
            if (diff_p_c > Const42 && ((eax & 10) != 0))
              accumPm += diff_p_c;
          }
          int a_next = 3 * (nxtpf[x] + nxtnf[x]);
          int diff_n_c = abs(a_next - a_curr);
          if (diff_n_c > Const23) {
            accumNc += diff_n_c;
            if (diff_n_c > Const42 && ((eax & 10) != 0))
              accumNm += diff_n_c;
          }
        }
      } // if

      mapp += map_pitch;
      prvpf += prvf_pitch;
      curpf += curf_pitch;
      prvnf += prvf_pitch;
      curf += curf_pitch;
      nxtpf += nxtf_pitch;
      curnf += curf_pitch;
      nxtnf += nxtf_pitch;
      mapn += map_pitch;
    }

#if 0
    // TFM 874
    __asm
    {
      push ebx // pf170421

      mov y, 2
      yloop:
      mov ecx, y0a
        mov edx, y1a
        cmp ecx, edx
        je xloop_pre
        mov eax, y
        cmp eax, ecx
        jl xloop_pre
        cmp eax, edx
        jle end_yloop
        xloop_pre :
      mov esi, incl
        mov ebx, startx
        mov edi, mapp
        mov edx, mapn
        mov ecx, stopx
        xloop :
      movzx eax, BYTE PTR[edi + ebx]
        shl eax, 2
        add al, BYTE PTR[edx + ebx]
        jnz b1
        add ebx, esi
        cmp ebx, ecx
        jl xloop
        jmp end_yloop
        b1 :
      mov edx, curf
        mov edi, curpf
        movzx ecx, BYTE PTR[edx + ebx]
        movzx esi, BYTE PTR[edi + ebx]
        shl ecx, 2
        mov edx, curnf
        add ecx, esi
        mov edi, prvpf
        movzx esi, BYTE PTR[edx + ebx]
        movzx edx, BYTE PTR[edi + ebx]
        add ecx, esi
        mov edi, prvnf
        movzx esi, BYTE PTR[edi + ebx]
        add edx, esi
        mov edi, edx
        add edx, edx
        sub edi, ecx
        add edx, edi
        jge b2
        neg edx
        b2 :
      cmp edx, 23
        jle p1
        add accumPc, edx
        cmp edx, 42
        jle p1
        test eax, 10
        jz p1
        add accumPm, edx
        p1 :
      mov edi, nxtpf
        mov esi, nxtnf
        movzx edx, BYTE PTR[edi + ebx]
        movzx edi, BYTE PTR[esi + ebx]
        add edx, edi
        mov esi, edx
        add edx, edx
        sub esi, ecx
        add edx, esi
        jge b3
        neg edx
        b3 :
      cmp edx, 23
        jle p2
        add accumNc, edx
        cmp edx, 42
        jle p2
        test eax, 10
        jz p2
        add accumNm, edx
        p2 :
      mov esi, incl
        mov ecx, stopx
        mov edi, mapp
        add ebx, esi
        mov edx, mapn
        cmp ebx, ecx
        jl xloop
        end_yloop :
      mov esi, Height
        mov eax, prvf_pitch
        mov ebx, curf_pitch
        mov ecx, nxtf_pitch
        mov edi, map_pitch
        sub esi, 2
        add y, 2
        add mapp, edi
        add prvpf, eax
        add curpf, ebx
        add prvnf, eax
        add curf, ebx
        add nxtpf, ecx
        add curnf, ebx
        add nxtnf, ecx
        add mapn, edi
        cmp y, esi
        jl yloop

        pop ebx // pf170421
    }
#endif
  }

  // High bit depth: I chose to scale back to 8 bit range.
  // Or else we should treat them as int64 and act upon them outside
  const double factor = 1.0 / (1 << (bits_per_pixel - 8));

  norm1 = (int)((accumPc / 6.0 * factor) + 0.5);
  norm2 = (int)((accumNc / 6.0 * factor) + 0.5);
  mtn1 = (int)((accumPm / 6.0 * factor) + 0.5);
  mtn2 = (int)((accumNm / 6.0 * factor) + 0.5);
  // TODO:  improve this decision about whether to use the mtn metrics or
  //        the normal metrics.  mtn metrics give better recognition of
  //        small areas ("mouths")... the hard part is telling when they
  //        are reliable enough to use.
  float c1 = float(std::max(norm1, norm2)) / float(std::max(std::min(norm1, norm2), 1));
  float c2 = float(std::max(mtn1, mtn2)) / float(std::max(std::min(mtn1, mtn2), 1));
  float mr = float(std::max(mtn1, mtn2)) / float(std::max(std::max(norm1, norm2), 1));
  if (((mtn1 >= 500 || mtn2 >= 500) && (mtn1 * 2 < mtn2 * 1 || mtn2 * 2 < mtn1 * 1)) ||
    ((mtn1 >= 1000 || mtn2 >= 1000) && (mtn1 * 3 < mtn2 * 2 || mtn2 * 3 < mtn1 * 2)) ||
    ((mtn1 >= 2000 || mtn2 >= 2000) && (mtn1 * 5 < mtn2 * 4 || mtn2 * 5 < mtn1 * 4)) ||
    ((mtn1 >= 4000 || mtn2 >= 4000) && c2 > c1))
  {
    if (mtn1 > mtn2) ret = match2;
    else ret = match1;
  }
  else if (mr > 0.005 && std::max(mtn1, mtn2) > 150 && (mtn1 * 2 < mtn2 * 1 || mtn2 * 2 < mtn1 * 1))
  {
    if (mtn1 > mtn2) ret = match2;
    else ret = match1;
  }
  else
  {
    if (norm1 > norm2) ret = match2;
    else ret = match1;
  }
  if (debug)
  {
    sprintf(buf, "TFM:  frame %d  - comparing %c to %c\n", n, MTC(match1), MTC(match2));
    OutputDebugString(buf);
    sprintf(buf, "TFM:  frame %d  - nmatches:  %d vs %d (%3.1f)  mmatches:  %d vs %d (%3.1f)\n", n,
      norm1, norm2, c1, mtn1, mtn2, c2);
    OutputDebugString(buf);
  }
  return ret;
}

int TFM::compareFieldsSlow(PVideoFrame& prv, PVideoFrame& src, PVideoFrame& nxt, int match1,
  int match2, int& norm1, int& norm2, int& mtn1, int& mtn2, const VideoInfo& vi, int n)
{
  if (slow == 2) {
    if (vi.ComponentSize() == 1)
      return compareFieldsSlow2_core<uint8_t>(prv, src, nxt, match1, match2, norm1, norm2, mtn1, mtn2, vi, n);
    else
      return compareFieldsSlow2_core<uint16_t>(prv, src, nxt, match1, match2, norm1, norm2, mtn1, mtn2, vi, n);
  }
  if (vi.ComponentSize() == 1)
    return compareFieldsSlow_core<uint8_t>(prv, src, nxt, match1, match2, norm1, norm2, mtn1, mtn2, vi, n);
  else
    return compareFieldsSlow_core<uint16_t>(prv, src, nxt, match1, match2, norm1, norm2, mtn1, mtn2, vi, n);
}

template<typename pixel_t>
int TFM::compareFieldsSlow_core(PVideoFrame &prv, PVideoFrame &src, PVideoFrame &nxt, int match1,
  int match2, int &norm1, int &norm2, int &mtn1, int &mtn2, const VideoInfo &vi, int n)
{
  (void)n;

  const int bits_per_pixel = vi.BitsPerComponent();

  int ret;
  int y0a, y1a;  // exclusion regio

  int tpitch_current;

  const int stop = vi.IsYUY2() || vi.IsY() || !mChroma ? 1 : 3;
  const int incl = vi.IsYUY2() && !mChroma ? 2 : 1;  // pixel increments: 2 if YUY2 with no-chroma option otherwise 1

  uint64_t accumPc = 0, accumNc = 0;
  uint64_t accumPm = 0, accumNm = 0;
  uint64_t accumPml = 0, accumNml = 0; // plus compared to CompareFields
  norm1 = norm2 = mtn1 = mtn2 = 0;

  const int planes[3] = { PLANAR_Y, PLANAR_U, PLANAR_V };
  for (int b = 0; b < stop; ++b)
  {
    const int plane = planes[b];

    uint8_t* mapp = map->GetPtr(b);
    int map_pitch = map->GetPitch(b);

    const pixel_t* prvp = reinterpret_cast<const pixel_t*>(prv->GetReadPtr(plane));
    const int prv_pitch = prv->GetPitch(plane) / sizeof(pixel_t);

    const pixel_t* srcp = reinterpret_cast<const pixel_t*>(src->GetReadPtr(plane));
    const int src_pitch = src->GetPitch(plane) / sizeof(pixel_t);

    const int Width = src->GetRowSize(plane) / sizeof(pixel_t);
    const int Height = src->GetHeight(plane);

    const pixel_t* nxtp = reinterpret_cast<const pixel_t*>(nxt->GetReadPtr(plane));
    const int nxt_pitch = nxt->GetPitch(plane) / sizeof(pixel_t);

    const int startx = vi.IsYUY2() ? 16 : 8 >> vi.GetPlaneWidthSubsampling(plane);
    const int stopx = Width - startx;

    const pixel_t* prvpf = nullptr, * curf = nullptr, * nxtpf = nullptr;
    int prvf_pitch = 0, curf_pitch = 0, nxtf_pitch = 0;

    curf_pitch = src_pitch << 1;

    memset(mapp, 0, Height * map_pitch);

    // exclusion area limits from parameters
    if (b == 0)
    {
      y0a = y0;
      y1a = y1;
      tpitch_current = tpitchy; // plus compared to simple compareFields
    }
    else
    {
      const int ysubsampling = vi.GetPlaneHeightSubsampling(plane);
      y0a = y0 >> ysubsampling;
      y1a = y1 >> ysubsampling;
      tpitch_current = tpitchuv; // plus compared to simple compareFields
    }
    const bool noBandExclusion = (y0a == y1a);
    if (y0a >= 2) y0a = y0a - 2; // v18: real limit, since y goes only till Height-2
    if (y1a <= Height - 2) y1a = y1a + 2; // v18: real limit, since y goes only from 2

    if (match1 < 3)
    {
      curf = srcp + ((3 - field)*src_pitch);
      mapp = mapp + ((field == 1 ? 1 : 2)*map_pitch);
    }
    if (match1 == 0)
    {
      prvf_pitch = prv_pitch << 1;
      prvpf = prvp + ((field == 1 ? 1 : 2)*prv_pitch);
    }
    else if (match1 == 1)
    {
      prvf_pitch = src_pitch << 1;
      prvpf = srcp + ((field == 1 ? 1 : 2)*src_pitch);
    }
    else if (match1 == 2)
    {
      prvf_pitch = nxt_pitch << 1;
      prvpf = nxtp + ((field == 1 ? 1 : 2)*nxt_pitch);
    }
    else if (match1 == 3)
    {
      curf = srcp + ((2 + field)*src_pitch);
      prvf_pitch = prv_pitch << 1;
      prvpf = prvp + ((field == 1 ? 2 : 1)*prv_pitch);
      mapp = mapp + ((field == 1 ? 2 : 1)*map_pitch);
    }
    else if (match1 == 4)
    {
      curf = srcp + ((2 + field)*src_pitch);
      prvf_pitch = nxt_pitch << 1;
      prvpf = nxtp + ((field == 1 ? 2 : 1)*nxt_pitch);
      mapp = mapp + ((field == 1 ? 2 : 1)*map_pitch);
    }
    if (match2 == 0)
    {
      nxtf_pitch = prv_pitch << 1;
      nxtpf = prvp + ((field == 1 ? 1 : 2)*prv_pitch);
    }
    else if (match2 == 1)
    {
      nxtf_pitch = src_pitch << 1;
      nxtpf = srcp + ((field == 1 ? 1 : 2)*src_pitch);
    }
    else if (match2 == 2)
    {
      nxtf_pitch = nxt_pitch << 1;
      nxtpf = nxtp + ((field == 1 ? 1 : 2)*nxt_pitch);
    }
    else if (match2 == 3)
    {
      nxtf_pitch = prv_pitch << 1;
      nxtpf = prvp + ((field == 1 ? 2 : 1)*prv_pitch);
    }
    else if (match2 == 4)
    {
      nxtf_pitch = nxt_pitch << 1;
      nxtpf = nxtp + ((field == 1 ? 2 : 1)*nxt_pitch);
    }

    const pixel_t* prvnf = prvpf + prvf_pitch;
    const pixel_t* curpf = curf - curf_pitch;
    const pixel_t* curnf = curf + curf_pitch;
    const pixel_t* nxtnf = nxtpf + nxtf_pitch;

    map_pitch <<= 1;
    uint8_t* mapn = mapp + map_pitch;

    // back to byte pointers
    if (vi.IsPlanar())
    {
      if ((match1 >= 3 && field == 1) || (match1 < 3 && field != 1))
        buildDiffMapPlane_Planar<pixel_t>(
          reinterpret_cast<const uint8_t*>(prvpf),
          reinterpret_cast<const uint8_t*>(nxtpf),
          mapp,
          prvf_pitch * sizeof(pixel_t),
          nxtf_pitch * sizeof(pixel_t),
          map_pitch, Height, Width, tpitch_current, bits_per_pixel);
      else
        buildDiffMapPlane_Planar<pixel_t>(
          reinterpret_cast<const uint8_t*>(prvnf),
          reinterpret_cast<const uint8_t*>(nxtnf),
          mapn,
          prvf_pitch * sizeof(pixel_t),
          nxtf_pitch * sizeof(pixel_t),
          map_pitch, Height, Width, tpitch_current, bits_per_pixel);
    }
    else
    { // YUY2
      if constexpr (sizeof(pixel_t) == 1) {
        if ((match1 >= 3 && field == 1) || (match1 < 3 && field != 1))
          buildDiffMapPlaneYUY2(prvpf, nxtpf, mapp, prvf_pitch, nxtf_pitch, map_pitch, Height, Width, tpitch_current);
        else
          buildDiffMapPlaneYUY2(prvnf, nxtnf, mapn, prvf_pitch, nxtf_pitch, map_pitch, Height, Width, tpitch_current);
      }
    }
#ifdef USE_C_NO_ASM
    const int Const23 = 23 << (bits_per_pixel - 8);
    const int Const42 = 42 << (bits_per_pixel - 8);

    // TFM 1144
    // almost the same as in compareFields and buildDiffMapPlane2
    for (int y = 2; y < Height - 2; y += 2) {
      if ((y < y0a) || noBandExclusion || (y > y1a)) // exclusion area check
      {
        for (int x = startx; x < stopx; x += incl)
        {
          // diff from prev asm block (at buildDiffMapPlane2): <<3 instead of <<2
          int eax = (mapp[x] << 3) + mapn[x];
          if ((eax & 0xFF) == 0)
            continue;

          int a_curr = curpf[x] + (curf[x] << 2) + curnf[x];
          int a_prev = 3 * (prvpf[x] + prvnf[x]);
          int diff_p_c = abs(a_prev - a_curr);
          if (diff_p_c > Const23) {
            if((eax & 9) != 0) // diff from previous similar asm block: condition
              accumPc += diff_p_c;
            if (diff_p_c > Const42) {
              if ((eax & 18) != 0) // diff: &18 instead of &10
                accumPm += diff_p_c;
              if ((eax & 36) != 0) // diff: new condition and accumulator
                accumPml += diff_p_c;
            }
          }
          int a_next = 3 * (nxtpf[x] + nxtnf[x]);
          int diff_n_c = abs(a_next - a_curr);
          if (diff_n_c > Const23) {
            if ((eax & 9) != 0) // diff from previous similar asm block: condition
              accumNc += diff_n_c;
            if (diff_n_c > Const42) {
              if ((eax & 18) != 0) // diff: &18 instead of &10
                accumNm += diff_n_c;
              if ((eax & 36) != 0) // diff: &18 instead of &10
                accumNml += diff_n_c;
            }
          }
        }
      } // if

      mapp += map_pitch;
      prvpf += prvf_pitch;
      curpf += curf_pitch;
      prvnf += prvf_pitch;
      curf += curf_pitch;
      nxtpf += nxtf_pitch;
      curnf += curf_pitch;
      nxtnf += nxtf_pitch;
      mapn += map_pitch;
    }

#else
    // TFM 1144
    __asm
    {
      push ebx // pf170421

      mov y, 2
      yloop:
      mov ecx, y0a
        mov edx, y1a
        cmp ecx, edx
        je xloop_pre
        mov eax, y
        cmp eax, ecx
        jl xloop_pre
        cmp eax, edx
        jle end_yloop
        xloop_pre :
      mov esi, incl
        mov ebx, startx
        mov edi, mapp
        mov edx, mapn
        mov ecx, stopx
        xloop :
      movzx eax, BYTE PTR[edi + ebx]
        shl eax, 3
        add al, BYTE PTR[edx + ebx]
        jnz b1
        add ebx, esi
        cmp ebx, ecx
        jl xloop
        jmp end_yloop
        b1 :
      mov edx, curf
        mov edi, curpf
        movzx ecx, BYTE PTR[edx + ebx]
        movzx esi, BYTE PTR[edi + ebx]
        shl ecx, 2
        mov edx, curnf
        add ecx, esi
        mov edi, prvpf
        movzx esi, BYTE PTR[edx + ebx]
        movzx edx, BYTE PTR[edi + ebx]
        add ecx, esi
        mov edi, prvnf
        movzx esi, BYTE PTR[edi + ebx]
        add edx, esi
        mov edi, edx
        add edx, edx
        sub edi, ecx
        add edx, edi
        jge b3
        neg edx
        b3 :
      cmp edx, 23
        jle p3
        test eax, 9
        jz p1
        add accumPc, edx
        p1 :
      cmp edx, 42
        jle p3
        test eax, 18
        jz p2
        add accumPm, edx
        p2 :
      test eax, 36
        jz p3
        add accumPml, edx
        p3 :
      mov edi, nxtpf
        mov esi, nxtnf
        movzx edx, BYTE PTR[edi + ebx]
        movzx edi, BYTE PTR[esi + ebx]
        add edx, edi
        mov esi, edx
        add edx, edx
        sub esi, ecx
        add edx, esi
        jge b2
        neg edx
        b2 :
      cmp edx, 23
        jle p6
        test eax, 9
        jz p4
        add accumNc, edx
        p4 :
      cmp edx, 42
        jle p6
        test eax, 18
        jz p5
        add accumNm, edx
        p5 :
      test eax, 36
        jz p6
        add accumNml, edx
        p6 :
      mov esi, incl
        mov ecx, stopx
        mov edi, mapp
        add ebx, esi
        mov edx, mapn
        cmp ebx, ecx
        jl xloop
        end_yloop :
      mov esi, Height
        mov eax, prvf_pitch
        mov ebx, curf_pitch
        mov ecx, nxtf_pitch
        mov edi, map_pitch
        sub esi, 2
        add y, 2
        add mapp, edi
        add prvpf, eax
        add curpf, ebx
        add prvnf, eax
        add curf, ebx
        add nxtpf, ecx
        add curnf, ebx
        add nxtnf, ecx
        add mapn, edi
        cmp y, esi
        jl yloop

        pop ebx // pf170421

    }
#endif
  }

  const unsigned int Const500 = 500 << (bits_per_pixel - 8);
  if (accumPm < Const500 && accumNm < Const500 && (accumPml >= Const500 || accumNml >= Const500) &&
    std::max(accumPml, accumNml) > 3 * std::min(accumPml, accumNml))
  {
    accumPm = accumPml;
    accumNm = accumNml;
  }

  // High bit depth: I chose to scale back to 8 bit range.
  // Or else we should treat them as int64 and act upon them outside
  const double factor = 1.0 / (1 << (bits_per_pixel - 8));

  norm1 = (int)((accumPc / 6.0 * factor) + 0.5);
  norm2 = (int)((accumNc / 6.0 * factor) + 0.5);
  mtn1 = (int)((accumPm / 6.0 * factor) + 0.5);
  mtn2 = (int)((accumNm / 6.0 * factor) + 0.5);
  // we are in the 8bit normalized region again, no change from here
  float c1 = float(std::max(norm1, norm2)) / float(std::max(std::min(norm1, norm2), 1));
  float c2 = float(std::max(mtn1, mtn2)) / float(std::max(std::min(mtn1, mtn2), 1));
  float mr = float(std::max(mtn1, mtn2)) / float(std::max(std::max(norm1, norm2), 1));
  if (((mtn1 >= 375 || mtn2 >= 375) && (mtn1 * 3 < mtn2 * 1 || mtn2 * 3 < mtn1 * 1)) ||
    ((mtn1 >= 500 || mtn2 >= 500) && (mtn1 * 2 < mtn2 * 1 || mtn2 * 2 < mtn1 * 1)) ||
    ((mtn1 >= 1000 || mtn2 >= 1000) && (mtn1 * 3 < mtn2 * 2 || mtn2 * 3 < mtn1 * 2)) ||
    ((mtn1 >= 2000 || mtn2 >= 2000) && (mtn1 * 5 < mtn2 * 4 || mtn2 * 5 < mtn1 * 4)) ||
    ((mtn1 >= 4000 || mtn2 >= 4000) && c2 > c1))
  {
    if (mtn1 > mtn2) ret = match2;
    else ret = match1;
  }
  else if (mr > 0.005 && std::max(mtn1, mtn2) > 150 && (mtn1 * 2 < mtn2 * 1 || mtn2 * 2 < mtn1 * 1))
  {
    if (mtn1 > mtn2) ret = match2;
    else ret = match1;
  }
  else
  {
    if (norm1 > norm2) ret = match2;
    else ret = match1;
  }
  if (debug)
  {
    sprintf(buf, "TFM:  frame %d  - comparing %c to %c  (SLOW 1)\n", n, MTC(match1), MTC(match2));
    OutputDebugString(buf);
    sprintf(buf, "TFM:  frame %d  - nmatches:  %d vs %d (%3.1f)  mmatches:  %d vs %d (%3.1f)\n", n,
      norm1, norm2, c1, mtn1, mtn2, c2);
    OutputDebugString(buf);
  }
  return ret;
}

template<typename pixel_t>
int TFM::compareFieldsSlow2_core(PVideoFrame &prv, PVideoFrame &src, PVideoFrame &nxt, int match1,
  int match2, int &norm1, int &norm2, int &mtn1, int &mtn2, const VideoInfo& vi, int n)
{
  (void)n;

  const int bits_per_pixel = vi.BitsPerComponent();

  int ret;
  int y0a, y1a;  // exclusion regio

  int tpitch_current;

  const int stop = vi.IsYUY2() || vi.IsY() || !mChroma ? 1 : 3;
  int incl = vi.IsYUY2() && !mChroma ? 2 : 1;  // pixel increments: 2 if YUY2 with no-chroma option otherwise 1

  uint64_t accumPc = 0, accumNc = 0;
  uint64_t accumPm = 0, accumNm = 0;
  uint64_t accumPml = 0, accumNml = 0; // plus compared to CompareFields
  norm1 = norm2 = mtn1 = mtn2 = 0;

  const int planes[3] = { PLANAR_Y, PLANAR_U, PLANAR_V };
  for (int b = 0; b < stop; ++b)
  {
    const int plane = planes[b];
    uint8_t* mapp = map->GetPtr(b);
    int map_pitch = map->GetPitch(b);

    const pixel_t* prvp = reinterpret_cast<const pixel_t*>(prv->GetReadPtr(plane));
    const int prv_pitch = prv->GetPitch(plane) / sizeof(pixel_t);

    const pixel_t* srcp = reinterpret_cast<const pixel_t*>(src->GetReadPtr(plane));
    const int src_pitch = src->GetPitch(plane) / sizeof(pixel_t);

    const int Width = src->GetRowSize(plane) / sizeof(pixel_t);
    const int Height = src->GetHeight(plane);

    const pixel_t* nxtp = reinterpret_cast<const pixel_t*>(nxt->GetReadPtr(plane));
    const int nxt_pitch = nxt->GetPitch(plane) / sizeof(pixel_t);

    const int startx = vi.IsYUY2() ? 16 : 8 >> vi.GetPlaneWidthSubsampling(plane);
    const int stopx = Width - startx;

    const pixel_t* prvpf = nullptr, * curf = nullptr, * nxtpf = nullptr;
    int prvf_pitch = 0, curf_pitch = 0, nxtf_pitch = 0;

    curf_pitch = src_pitch << 1;

    memset(mapp, 0, Height * map_pitch);

    // exclusion area limits from parameters
    if (b == 0)
    {
      y0a = y0;
      y1a = y1;
      tpitch_current = tpitchy;
    }
    else
    {
      const int ysubsampling = vi.GetPlaneHeightSubsampling(plane);
      y0a = y0 >> ysubsampling;
      y1a = y1 >> ysubsampling;
      tpitch_current = tpitchuv;
    }
    const bool noBandExclusion = (y0a == y1a);
    if (y0a >= 2) y0a = y0a - 2; // v18: real limit, since y goes only till Height-2
    if (y1a <= Height - 2) y1a = y1a + 2; // v18: real limit, since y goes only from 2

    if (match1 < 3)
    {
      curf = srcp + ((3 - field)*src_pitch);
      mapp = mapp + ((field == 1 ? 1 : 2)*map_pitch);
    }
    if (match1 == 0)
    {
      prvf_pitch = prv_pitch << 1;
      prvpf = prvp + ((field == 1 ? 1 : 2)*prv_pitch);
    }
    else if (match1 == 1)
    {
      prvf_pitch = src_pitch << 1;
      prvpf = srcp + ((field == 1 ? 1 : 2)*src_pitch);
    }
    else if (match1 == 2)
    {
      prvf_pitch = nxt_pitch << 1;
      prvpf = nxtp + ((field == 1 ? 1 : 2)*nxt_pitch);
    }
    else if (match1 == 3)
    {
      curf = srcp + ((2 + field)*src_pitch);
      prvf_pitch = prv_pitch << 1;
      prvpf = prvp + ((field == 1 ? 2 : 1)*prv_pitch);
      mapp = mapp + ((field == 1 ? 2 : 1)*map_pitch);
    }
    else if (match1 == 4)
    {
      curf = srcp + ((2 + field)*src_pitch);
      prvf_pitch = nxt_pitch << 1;
      prvpf = nxtp + ((field == 1 ? 2 : 1)*nxt_pitch);
      mapp = mapp + ((field == 1 ? 2 : 1)*map_pitch);
    }
    if (match2 == 0)
    {
      nxtf_pitch = prv_pitch << 1;
      nxtpf = prvp + ((field == 1 ? 1 : 2)*prv_pitch);
    }
    else if (match2 == 1)
    {
      nxtf_pitch = src_pitch << 1;
      nxtpf = srcp + ((field == 1 ? 1 : 2)*src_pitch);
    }
    else if (match2 == 2)
    {
      nxtf_pitch = nxt_pitch << 1;
      nxtpf = nxtp + ((field == 1 ? 1 : 2)*nxt_pitch);
    }
    else if (match2 == 3)
    {
      nxtf_pitch = prv_pitch << 1;
      nxtpf = prvp + ((field == 1 ? 2 : 1)*prv_pitch);
    }
    else if (match2 == 4)
    {
      nxtf_pitch = nxt_pitch << 1;
      nxtpf = nxtp + ((field == 1 ? 2 : 1)*nxt_pitch);
    }

    const pixel_t* prvppf = prvpf - prvf_pitch;
    const pixel_t* prvnf = prvpf + prvf_pitch;
    const pixel_t* prvnnf = prvnf + prvf_pitch;

    const pixel_t* curpf = curf - curf_pitch;
    const pixel_t* curnf = curf + curf_pitch;

    const pixel_t* nxtppf = nxtpf - nxtf_pitch;
    const pixel_t* nxtnf = nxtpf + nxtf_pitch;
    const pixel_t* nxtnnf = nxtnf + nxtf_pitch;

    map_pitch <<= 1;
    uint8_t* mapn = mapp + map_pitch;

    // back to byte pointers
    if (vi.IsPlanar())
    {
      if ((match1 >= 3 && field == 1) || (match1 < 3 && field != 1))
        buildDiffMapPlane_Planar<pixel_t>(
          reinterpret_cast<const uint8_t*>(prvpf),
          reinterpret_cast<const uint8_t*>(nxtpf),
          mapp,
          prvf_pitch * sizeof(pixel_t),
          nxtf_pitch * sizeof(pixel_t),
          map_pitch, Height, Width, tpitch_current, bits_per_pixel);
      else
        buildDiffMapPlane_Planar<pixel_t>(
          reinterpret_cast<const uint8_t*>(prvnf),
          reinterpret_cast<const uint8_t*>(nxtnf),
          mapn,
          prvf_pitch * sizeof(pixel_t),
          nxtf_pitch * sizeof(pixel_t),
          map_pitch, Height, Width, tpitch_current, bits_per_pixel);
    }
    else
    { // YUY2
      if constexpr (sizeof(pixel_t) == 1) {
        if ((match1 >= 3 && field == 1) || (match1 < 3 && field != 1))
          buildDiffMapPlaneYUY2(prvpf, nxtpf, mapp, prvf_pitch, nxtf_pitch, map_pitch, Height, Width, tpitch_current);
        else
          buildDiffMapPlaneYUY2(prvnf, nxtnf, mapn, prvf_pitch, nxtf_pitch, map_pitch, Height, Width, tpitch_current);
      }
    }

    const int Const23 = 23 << (bits_per_pixel - 8);
    const int Const42 = 42 << (bits_per_pixel - 8);

    if (field == 0) {
    // TFM 1436
    // almost the same as in TFM 1144
      for (int y = 2; y < Height - 2; y += 2) {
        if ((y < y0a) || noBandExclusion || (y > y1a))
        {
          for (int x = startx; x < stopx; x += incl)
          {
            int eax = (mapp[x] << 3) + mapn[x]; // diff from prev asm block (at buildDiffMapPlane2): <<3 instead of <<2
            if ((eax & 0xFF) == 0)
              continue;

            int a_curr = curpf[x] + (curf[x] << 2) + curnf[x];
            int a_prev = 3 * (prvpf[x] + prvnf[x]);
            int diff_p_c = abs(a_prev - a_curr);
            if (diff_p_c > Const23) {
              if ((eax & 9) != 0) // diff from previous similar asm block: condition
                accumPc += diff_p_c;
              if (diff_p_c > Const42) {
                if ((eax & 18) != 0) // diff: &18 instead of &10
                  accumPm += diff_p_c;
                if ((eax & 36) != 0) // diff: new condition and accumulator
                  accumPml += diff_p_c;
              }
            }
            int a_next = 3 * (nxtpf[x] + nxtnf[x]);
            int diff_n_c = abs(a_next - a_curr);
            if (diff_n_c > Const23) {
              if ((eax & 9) != 0) // diff from previous similar asm block: condition
                accumNc += diff_n_c;
              if (diff_n_c > Const42) {
                if ((eax & 18) != 0) // diff: &18 instead of &10
                  accumNm += diff_n_c;
                if ((eax & 36) != 0) // diff: &18 instead of &10
                  accumNml += diff_n_c;
              }
            }

            // additional difference from TFM 1144
            if ((eax & 56) != 0) {

              int a_prev = prvppf[x] + (prvpf[x] << 2) + prvnf[x];
              int a_curr = 3 * (curpf[x] + curf[x]);
              int diff_p_c = abs(a_prev - a_curr);
              if (diff_p_c > Const23) {
                if ((eax & 8) != 0) // diff from previous similar asm block: condition
                  accumPc += diff_p_c;
                if (diff_p_c > Const42) {
                  if ((eax & 16) != 0) // diff: &16 instead of &18
                    accumPm += diff_p_c;
                  if ((eax & 32) != 0) // diff: new condition and accumulator
                    accumPml += diff_p_c;
                }
              }
              int a_next = nxtppf[x] + (nxtpf[x] << 2) + nxtnf[x]; // really! not 3*
              int diff_n_c = abs(a_next - a_curr);
              if (diff_n_c > Const23) {
                if ((eax & 8) != 0) // diff: &8 instead of &9
                  accumNc += diff_n_c;
                if (diff_n_c > Const42) {
                  if ((eax & 16) != 0) // diff: &16 instead of &18
                    accumNm += diff_n_c;
                  if ((eax & 32) != 0) // diff: &32 instead of &36
                    accumNml += diff_n_c;
                }
              }
            }
          }
        } // if

        mapp += map_pitch;
        prvpf += prvf_pitch;
        curpf += curf_pitch;
        prvnf += prvf_pitch;
        curf += curf_pitch;
        nxtpf += nxtf_pitch;
        curnf += curf_pitch;
        nxtnf += nxtf_pitch;
        mapn += map_pitch;

        prvppf += prvf_pitch;
        nxtppf += nxtf_pitch;
      }
    }
    else {
      // TFM 1633
      // almost the same as in TFM 1436 (field==0= case)
      // differences are after eax&56 block, see later

      for (int y = 2; y < Height - 2; y += 2) {
        if ((y < y0a) || noBandExclusion || (y > y1a))
        {
          for (int x = startx; x < stopx; x += incl)
          {
            int eax = (mapp[x] << 3) + mapn[x]; // diff from prev asm block (at buildDiffMapPlane2): <<3 instead of <<2
            if ((eax & 0xFF) == 0)
              continue;

            int a_curr = curpf[x] + (curf[x] << 2) + curnf[x];
            int a_prev = 3 * (prvpf[x] + prvnf[x]);
            int diff_p_c = abs(a_prev - a_curr);
            if (diff_p_c > Const23) {
              if ((eax & 9) != 0) // diff from previous similar asm block: condition
                accumPc += diff_p_c;
              if (diff_p_c > Const42) {
                if ((eax & 18) != 0) // diff: &18 instead of &10
                  accumPm += diff_p_c;
                if ((eax & 36) != 0) // diff: new condition and accumulator
                  accumPml += diff_p_c;
              }
            }
            int a_next = 3 * (nxtpf[x] + nxtnf[x]); // L2008
            int diff_n_c = abs(a_next - a_curr);
            if (diff_n_c > Const23) {
              if ((eax & 9) != 0) // diff from previous similar asm block: condition
                accumNc += diff_n_c;
              if (diff_n_c > Const42) {
                if ((eax & 18) != 0) // diff: &18 instead of &10
                  accumNm += diff_n_c;
                if ((eax & 36) != 0) // diff: &18 instead of &10
                  accumNml += diff_n_c;
              }
            }

            // difference from TFM 1436
            // prvpf  -> prvnf
            // prvppf -> prvpf
            // prvnf  -> prvnnf
            // curpf  -> curf
            // curf   -> curnf
            // nxtpf  -> nxtnf
            // nxtppf -> nxtpf
            // nxtnf  -> nxtnnf
            // mask 8/16/32 -> 1/2/4
            if ((eax & 7) != 0) { // 1.0.12: diff: &7 instead of &56 L2036

              int a_prev = prvpf[x] + (prvnf[x] << 2) + prvnnf[x];
              int a_curr = 3 * (curf[x] + curnf[x]);
              int diff_p_c = abs(a_prev - a_curr);
              if (diff_p_c > Const23) {
                if ((eax & 1) != 0) // diff: &1 instead of &8
                  accumPc += diff_p_c;
                if (diff_p_c > Const42) {
                  if ((eax & 2) != 0) // diff: &2 instead of &16
                    accumPm += diff_p_c;
                  if ((eax & 4) != 0) // diff: &4 instead of &32
                    accumPml += diff_p_c;
                }
              }
              //int a_next = *(nxtppf + ebx) + (*(nxtpf + ebx) << 2) + *(nxtnf + ebx); // really! not 3*
              int a_next = nxtpf[x] + (nxtnf[x] << 2) + nxtnnf[x]; // really! not 3* L2075
              int diff_n_c = abs(a_next - a_curr);
              if (diff_n_c > Const23) { // L2088
                if ((eax & 1) != 0) // diff: &1 instead of &8
                  accumNc += diff_n_c;
                if (diff_n_c > Const42) { // L2094
                  if ((eax & 2) != 0) // diff: &2 instead of &16 // 1.0.12 really 2
                    accumNm += diff_n_c;
                  if ((eax & 4) != 0) // diff: &4 instead of &32
                    accumNml += diff_n_c;
                }
              }
            }
          }
        } // if

        mapp += map_pitch;
        prvpf += prvf_pitch;
        curpf += curf_pitch;
        prvnf += prvf_pitch;
        curf += curf_pitch;
        prvnnf += prvf_pitch; // 1.0.12
        nxtpf += nxtf_pitch;
        curnf += curf_pitch;
        nxtnf += nxtf_pitch;
        nxtnnf += nxtf_pitch;
        mapn += map_pitch;

        // not used prvppf += prvf_pitch;
        // not used nxtppf += nxtf_pitch;

      }

    }

#if 0
    if (field == 0)
    {
      // TFM 1436
      __asm
      {
        push ebx // pf170421

        mov y, 2
        yloop0:
        mov ecx, y0a
          mov edx, y1a
          cmp ecx, edx
          je xloop_pre0
          mov eax, y
          cmp eax, ecx
          jl xloop_pre0
          cmp eax, edx
          jle end_yloop0
          xloop_pre0 :
        mov esi, incl
          mov ebx, startx
          mov edi, mapp
          mov edx, mapn
          mov ecx, stopx
          xloop0 :
        movzx eax, BYTE PTR[edi + ebx]
          shl eax, 3
          add al, BYTE PTR[edx + ebx]
          jnz b10
          add ebx, esi
          cmp ebx, ecx
          jl xloop0
          jmp end_yloop0
          b10 :
        mov edx, curf
          mov edi, curpf
          movzx ecx, BYTE PTR[edx + ebx]
          movzx esi, BYTE PTR[edi + ebx]
          shl ecx, 2
          mov edx, curnf
          add ecx, esi
          mov edi, prvpf
          movzx esi, BYTE PTR[edx + ebx]
          movzx edx, BYTE PTR[edi + ebx]
          add ecx, esi
          mov edi, prvnf
          movzx esi, BYTE PTR[edi + ebx]
          add edx, esi
          mov edi, edx
          add edx, edx
          sub edi, ecx
          add edx, edi
          jge b30
          neg edx
          b30 :
        cmp edx, 23
          jle p30
          test eax, 9
          jz p10
          add accumPc, edx
          p10 :
        cmp edx, 42
          jle p30
          test eax, 18
          jz p20
          add accumPm, edx
          p20 :
        test eax, 36
          jz p30
          add accumPml, edx
          p30 :
        mov edi, nxtpf
          mov esi, nxtnf
          movzx edx, BYTE PTR[edi + ebx]
          movzx edi, BYTE PTR[esi + ebx]
          add edx, edi
          mov esi, edx
          add edx, edx
          sub esi, ecx
          add edx, esi
          jge b20
          neg edx
          b20 :
        cmp edx, 23
          jle p60
          test eax, 9
          jz p40
          add accumNc, edx
          p40 :
        cmp edx, 42
          jle p60
          test eax, 18
          jz p50
          add accumNm, edx
          p50 :
        test eax, 36
          jz p60
          add accumNml, edx
          p60 :
        test eax, 56
          jz p120
          mov ecx, prvpf
          mov edi, prvppf
          movzx edx, BYTE PTR[ecx + ebx]
          movzx esi, BYTE PTR[edi + ebx]
          shl edx, 2
          mov ecx, prvnf
          add edx, esi
          mov edi, curpf
          movzx esi, BYTE PTR[ecx + ebx]
          movzx ecx, BYTE PTR[edi + ebx]
          add edx, esi
          mov edi, curf
          movzx esi, BYTE PTR[edi + ebx]
          add ecx, esi
          mov edi, ecx
          add ecx, ecx
          add ecx, edi
          sub edx, ecx
          jge b40
          neg edx
          b40 :
        cmp edx, 23
          jle p90
          test eax, 8
          jz p70
          add accumPc, edx
          p70 :
        cmp edx, 42
          jle p90
          test eax, 16
          jz p80
          add accumPm, edx
          p80 :
        test eax, 32
          jz p90
          add accumPml, edx
          p90 :
        mov edi, nxtpf
          mov esi, nxtppf
          movzx edx, BYTE PTR[edi + ebx]
          movzx edi, BYTE PTR[esi + ebx]
          shl edx, 2
          mov esi, nxtnf
          add edx, edi
          movzx edi, BYTE PTR[esi + ebx]
          add edx, edi
          sub edx, ecx
          jge b50
          neg edx
          b50 :
        cmp edx, 23
          jle p120
          test eax, 8
          jz p100
          add accumNc, edx
          p100 :
        cmp edx, 42
          jle p120
          test eax, 16
          jz p110
          add accumNm, edx
          p110 :
        test eax, 32
          jz p120
          add accumNml, edx
          p120 :
        mov esi, incl
          mov ecx, stopx
          mov edi, mapp
          add ebx, esi
          mov edx, mapn
          cmp ebx, ecx
          jl xloop0
          end_yloop0 :
        mov esi, Height
          mov eax, prvf_pitch
          mov ebx, curf_pitch
          mov ecx, nxtf_pitch
          mov edi, map_pitch
          sub esi, 2
          add y, 2
          add mapp, edi
          add prvpf, eax
          add curpf, ebx
          add prvnf, eax
          add curf, ebx
          add nxtpf, ecx
          add prvppf, eax
          add curnf, ebx
          add nxtnf, ecx
          add mapn, edi
          add nxtppf, ecx
          cmp y, esi
          jl yloop0

          pop ebx // pf170421
      }
    }
    else
    {
      // TFM 1633
      __asm
      {
        push ebx // pf170421

        mov y, 2
        yloop1:
        mov ecx, y0a
          mov edx, y1a
          cmp ecx, edx
          je xloop_pre1
          mov eax, y
          cmp eax, ecx
          jl xloop_pre1
          cmp eax, edx
          jle end_yloop1
          xloop_pre1 :
        mov esi, incl
          mov ebx, startx
          mov edi, mapp
          mov edx, mapn
          mov ecx, stopx
          xloop1 :
        movzx eax, BYTE PTR[edi + ebx]
          shl eax, 3
          add al, BYTE PTR[edx + ebx]
          jnz b11
          add ebx, esi
          cmp ebx, ecx
          jl xloop1
          jmp end_yloop1
          b11 :
        mov edx, curf
          mov edi, curpf
          movzx ecx, BYTE PTR[edx + ebx]
          movzx esi, BYTE PTR[edi + ebx]
          shl ecx, 2
          mov edx, curnf
          add ecx, esi
          mov edi, prvpf
          movzx esi, BYTE PTR[edx + ebx]
          movzx edx, BYTE PTR[edi + ebx]
          add ecx, esi
          mov edi, prvnf
          movzx esi, BYTE PTR[edi + ebx]
          add edx, esi
          mov edi, edx
          add edx, edx
          sub edi, ecx
          add edx, edi
          jge b31
          neg edx
          b31 :
        cmp edx, 23
          jle p31
          test eax, 9
          jz p11
          add accumPc, edx
          p11 :
        cmp edx, 42
          jle p31
          test eax, 18
          jz p21
          add accumPm, edx
          p21 :
        test eax, 36
          jz p31
          add accumPml, edx
          p31 :
        mov edi, nxtpf
          mov esi, nxtnf
          movzx edx, BYTE PTR[edi + ebx]
          movzx edi, BYTE PTR[esi + ebx]
          add edx, edi
          mov esi, edx
          add edx, edx
          sub esi, ecx
          add edx, esi
          jge b21
          neg edx
          b21 :
        cmp edx, 23
          jle p61
          test eax, 9
          jz p41
          add accumNc, edx
          p41 :
        cmp edx, 42
          jle p61
          test eax, 18
          jz p51
          add accumNm, edx
          p51 :
        test eax, 36
          jz p61
          add accumNml, edx
          p61 :
        test eax, 7
          jz p121
          mov ecx, prvnf
          mov edi, prvpf
          movzx edx, BYTE PTR[ecx + ebx]
          movzx esi, BYTE PTR[edi + ebx]
          shl edx, 2
          mov ecx, prvnnf
          add edx, esi
          mov edi, curf
          movzx esi, BYTE PTR[ecx + ebx]
          movzx ecx, BYTE PTR[edi + ebx]
          add edx, esi
          mov edi, curnf
          movzx esi, BYTE PTR[edi + ebx]
          add ecx, esi
          mov edi, ecx
          add ecx, ecx
          add ecx, edi
          sub edx, ecx
          jge b41
          neg edx
          b41 :
        cmp edx, 23
          jle p91
          test eax, 1
          jz p71
          add accumPc, edx
          p71 :
        cmp edx, 42
          jle p91
          test eax, 2
          jz p81
          add accumPm, edx
          p81 :
        test eax, 4
          jz p91
          add accumPml, edx
          p91 :
        mov edi, nxtnf
          mov esi, nxtpf
          movzx edx, BYTE PTR[edi + ebx]
          movzx edi, BYTE PTR[esi + ebx]
          shl edx, 2
          mov esi, nxtnnf
          add edx, edi
          movzx edi, BYTE PTR[esi + ebx]
          add edx, edi
          sub edx, ecx
          jge b51
          neg edx
          b51 :
        cmp edx, 23
          jle p121
          test eax, 1
          jz p101
          add accumNc, edx
          p101 :
        cmp edx, 42
          jle p121
          test eax, 2
          jz p111
          add accumNm, edx
          p111 :
        test eax, 4
          jz p121
          add accumNml, edx
          p121 :
        mov esi, incl
          mov ecx, stopx
          mov edi, mapp
          add ebx, esi
          mov edx, mapn
          cmp ebx, ecx
          jl xloop1
          end_yloop1 :
        mov esi, Height
          mov eax, prvf_pitch
          mov ebx, curf_pitch
          mov ecx, nxtf_pitch
          mov edi, map_pitch
          sub esi, 2
          add y, 2
          add mapp, edi
          add prvpf, eax
          add curpf, ebx
          add prvnf, eax
          add curf, ebx
          add prvnnf, eax
          add nxtpf, ecx
          add curnf, ebx
          add nxtnf, ecx
          add mapn, edi
          add nxtnnf, ecx
          cmp y, esi
          jl yloop1

          pop ebx // pf170421

      }
    }
#endif
  }

  const unsigned int Const500 = 500 << (bits_per_pixel - 8);
  if (accumPm < Const500 && accumNm < Const500 && (accumPml >= Const500 || accumNml >= Const500) &&
    std::max(accumPml, accumNml) > 3 * std::min(accumPml, accumNml))
  {
    accumPm = accumPml;
    accumNm = accumNml;
  }

  // High bit depth: I chose to scale back to 8 bit range.
  // Or else we should treat them as int64 and act upon them outside
  const double factor = 1.0 / (1 << (bits_per_pixel - 8));

  norm1 = (int)((accumPc / 6.0 * factor) + 0.5);
  norm2 = (int)((accumNc / 6.0 * factor) + 0.5);
  mtn1 = (int)((accumPm / 6.0 * factor) + 0.5);
  mtn2 = (int)((accumNm / 6.0 * factor) + 0.5);
  // we are in the 8bit normalized region again, no change from here
  float c1 = float(std::max(norm1, norm2)) / float(std::max(std::min(norm1, norm2), 1));
  float c2 = float(std::max(mtn1, mtn2)) / float(std::max(std::min(mtn1, mtn2), 1));
  float mr = float(std::max(mtn1, mtn2)) / float(std::max(std::max(norm1, norm2), 1));
  if (((mtn1 >= 250 || mtn2 >= 250) && (mtn1 * 4 < mtn2 * 1 || mtn2 * 4 < mtn1 * 1)) ||
    ((mtn1 >= 375 || mtn2 >= 375) && (mtn1 * 3 < mtn2 * 1 || mtn2 * 3 < mtn1 * 1)) ||
    ((mtn1 >= 500 || mtn2 >= 500) && (mtn1 * 2 < mtn2 * 1 || mtn2 * 2 < mtn1 * 1)) ||
    ((mtn1 >= 1000 || mtn2 >= 1000) && (mtn1 * 3 < mtn2 * 2 || mtn2 * 3 < mtn1 * 2)) ||
    ((mtn1 >= 2000 || mtn2 >= 2000) && (mtn1 * 5 < mtn2 * 4 || mtn2 * 5 < mtn1 * 4)) ||
    ((mtn1 >= 4000 || mtn2 >= 4000) && c2 > c1))
  {
    if (mtn1 > mtn2) ret = match2;
    else ret = match1;
  }
  else if (mr > 0.005 && std::max(mtn1, mtn2) > 150 && (mtn1 * 2 < mtn2 * 1 || mtn2 * 2 < mtn1 * 1))
  {
    if (mtn1 > mtn2) ret = match2;
    else ret = match1;
  }
  else
  {
    if (norm1 > norm2) ret = match2;
    else ret = match1;
  }
  if (debug)
  {
    sprintf(buf, "TFM:  frame %d  - comparing %c to %c  (SLOW 2)\n", n, MTC(match1), MTC(match2));
    OutputDebugString(buf);
    sprintf(buf, "TFM:  frame %d  - nmatches:  %d vs %d (%3.1f)  mmatches:  %d vs %d (%3.1f)\n", n,
      norm1, norm2, c1, mtn1, mtn2, c2);
    OutputDebugString(buf);
  }
  return ret;
}

template<typename pixel_t>
static void checkSceneChangePlanar_1_c(const pixel_t* srcp, const pixel_t* nxtp,
  int height, int width, int src_pitch, int nxt_pitch, uint64_t& diff)
{
  for (int y = 0; y < height; ++y)
  {
    uint32_t rowdiff = 0;
    for (int x = 0; x < width; x += 4)
    {
      rowdiff += abs(srcp[x + 0] - nxtp[x + 0]);
      rowdiff += abs(srcp[x + 1] - nxtp[x + 1]);
      rowdiff += abs(srcp[x + 2] - nxtp[x + 2]);
      rowdiff += abs(srcp[x + 3] - nxtp[x + 3]);
    }
    diff += rowdiff;
    srcp += src_pitch;
    nxtp += nxt_pitch;
  }
}

void checkSceneChangeYUY2_1_c(const uint8_t* srcp, const uint8_t* nxtp,
int height, int width, int src_pitch, int nxt_pitch, uint64_t& diff)
{
  for (int y = 0; y < height; ++y)
  {
    uint32_t rowdiff = 0;
    for (int x = 0; x < width; x += 8)
    {
      rowdiff += abs(srcp[x + 0] - nxtp[x + 0]);
      rowdiff += abs(srcp[x + 2] - nxtp[x + 2]);
      rowdiff += abs(srcp[x + 4] - nxtp[x + 4]);
      rowdiff += abs(srcp[x + 6] - nxtp[x + 6]);
    }
    diff += rowdiff;
    srcp += src_pitch;
    nxtp += nxt_pitch;
  }
}

template<typename pixel_t>
static void checkSceneChangePlanar_2_c(const pixel_t* prvp, const pixel_t* srcp,
  const pixel_t* nxtp, int height, int width, int prv_pitch, int src_pitch,
  int nxt_pitch, uint64_t& diffp, uint64_t& diffn)
{
  for (int y = 0; y < height; ++y)
  {
    uint32_t rowdiffp = 0;
    uint32_t rowdiffn = 0;
    for (int x = 0; x < width; x += 4)
    {
      rowdiffp += abs(srcp[x + 0] - prvp[x + 0]);
      rowdiffp += abs(srcp[x + 1] - prvp[x + 1]);
      rowdiffp += abs(srcp[x + 2] - prvp[x + 2]);
      rowdiffp += abs(srcp[x + 3] - prvp[x + 3]);
      rowdiffn += abs(srcp[x + 0] - nxtp[x + 0]);
      rowdiffn += abs(srcp[x + 1] - nxtp[x + 1]);
      rowdiffn += abs(srcp[x + 2] - nxtp[x + 2]);
      rowdiffn += abs(srcp[x + 3] - nxtp[x + 3]);
    }
    diffp += rowdiffp;
    diffn += rowdiffn;
    prvp += prv_pitch;
    srcp += src_pitch;
    nxtp += nxt_pitch;
  }
}

static void checkSceneChangeYUY2_2_c(const uint8_t* prvp, const uint8_t* srcp,
  const uint8_t* nxtp, int height, int width, int prv_pitch, int src_pitch,
  int nxt_pitch, uint64_t& diffp, uint64_t& diffn)
{
  for (int y = 0; y < height; ++y)
  {
    uint32_t rowdiffp = 0;
    uint32_t rowdiffn = 0;
    for (int x = 0; x < width; x += 8)
    {
      rowdiffp += abs(srcp[x + 0] - prvp[x + 0]);
      rowdiffp += abs(srcp[x + 2] - prvp[x + 2]);
      rowdiffp += abs(srcp[x + 4] - prvp[x + 4]);
      rowdiffp += abs(srcp[x + 6] - prvp[x + 6]);
      rowdiffn += abs(srcp[x + 0] - nxtp[x + 0]);
      rowdiffn += abs(srcp[x + 2] - nxtp[x + 2]);
      rowdiffn += abs(srcp[x + 4] - nxtp[x + 4]);
      rowdiffn += abs(srcp[x + 6] - nxtp[x + 6]);
    }
    diffp += rowdiffp;
    diffn += rowdiffn;
    prvp += prv_pitch;
    srcp += src_pitch;
    nxtp += nxt_pitch;
  }
}

bool TFM::checkSceneChange(PVideoFrame& prv, PVideoFrame& src, PVideoFrame& nxt, int n)
{
  const int bits_per_pixel = vi.BitsPerComponent();
  if (bits_per_pixel == 8)
    return checkSceneChange_core<uint8_t>(prv, src, nxt, n, bits_per_pixel);
  else
    return checkSceneChange_core<uint16_t>(prv, src, nxt, n, bits_per_pixel);
}

template<typename pixel_t>
bool TFM::checkSceneChange_core(PVideoFrame &prv, PVideoFrame &src, PVideoFrame &nxt,
  int n, int bits_per_pixel)
{
  if (sclast.frame == n + 1) return sclast.sc;
  uint64_t diffp = 0;
  uint64_t diffn = 0;
  const uint8_t *prvp = prv->GetReadPtr(PLANAR_Y);
  const uint8_t *srcp = src->GetReadPtr(PLANAR_Y);
  const uint8_t *nxtp = nxt->GetReadPtr(PLANAR_Y);
  const int height = src->GetHeight(PLANAR_Y) >> 1;
  const int rowsize = src->GetRowSize(PLANAR_Y);
  int width = rowsize / sizeof(pixel_t);
  // this mod16 must be the same as in computing "diffmaxsc"

  // safe mod16 rounding for SSE2 in mind
  if (vi.IsPlanar())
    width = ((width >> 4) << 4); // mod16
  else // YUY2
    width = ((width >> 5) << 5); // mod32

  // every 2nd line
  int prv_pitch = prv->GetPitch(PLANAR_Y) << 1;
  int src_pitch = src->GetPitch(PLANAR_Y) << 1;
  int nxt_pitch = nxt->GetPitch(PLANAR_Y) << 1;
  prvp += (1 - field)*(prv_pitch >> 1);
  srcp += (1 - field)*(src_pitch >> 1);
  nxtp += (1 - field)*(nxt_pitch >> 1);

  bool use_sse2 = (cpuFlags & CPUF_SSE2) ? true : false;

  if (sclast.frame == n)
  {
    diffp = ((uint64_t)sclast.diff) << (bits_per_pixel - 8);
    if (vi.IsPlanar())
      if (sizeof(pixel_t) == 1 && use_sse2)
        checkSceneChangePlanar_1_SSE2(srcp, nxtp, height, width, src_pitch, nxt_pitch, diffn);
      else
        checkSceneChangePlanar_1_c<pixel_t>(
          reinterpret_cast<const pixel_t*>(srcp),
          reinterpret_cast<const pixel_t*>(nxtp),
          height, width,
          src_pitch / sizeof(pixel_t),
          nxt_pitch / sizeof(pixel_t),
          diffn);
    else
      if (use_sse2)
        checkSceneChangeYUY2_1_SSE2(srcp, nxtp, height, width, src_pitch, nxt_pitch, diffn);
      else
        checkSceneChangeYUY2_1_c(srcp, nxtp, height, width, src_pitch, nxt_pitch, diffn);
  }
  else
  {
    if (vi.IsPlanar())
      if (sizeof(pixel_t) == 1 && use_sse2)
        checkSceneChangePlanar_2_SSE2(prvp, srcp, nxtp, height, width, prv_pitch, src_pitch, nxt_pitch, diffp, diffn);
      else
        checkSceneChangePlanar_2_c<pixel_t>(
          reinterpret_cast<const pixel_t*>(prvp),
          reinterpret_cast<const pixel_t*>(srcp),
          reinterpret_cast<const pixel_t*>(nxtp),
          height, width,
          prv_pitch / sizeof(pixel_t),
          src_pitch / sizeof(pixel_t),
          nxt_pitch / sizeof(pixel_t),
          diffp, diffn);
    else
      if (use_sse2)
        checkSceneChangeYUY2_2_SSE2(prvp, srcp, nxtp, height, width, prv_pitch, src_pitch, nxt_pitch, diffp, diffn);
      else
        checkSceneChangeYUY2_2_c(prvp, srcp, nxtp, height, width, prv_pitch, src_pitch, nxt_pitch, diffp, diffn);
  }

  // scale back to 8 bit world
  diffn >>= (bits_per_pixel - 8);
  diffp >>= (bits_per_pixel - 8);

  if (debug)
  {
    sprintf(buf, "TFM:  frame %d  - diffp = %u   diffn = %u  diffmaxsc = %u  %c\n", n, (unsigned int)diffp, (unsigned int)diffn, (unsigned int)diffmaxsc,
      (diffp > diffmaxsc || diffn > diffmaxsc) ? 'T' : 'F');
    OutputDebugString(buf);
  }
  sclast.frame = n + 1;
  sclast.diff = (unsigned long)diffn;
  sclast.sc = true;
  if (diffp > diffmaxsc || diffn > diffmaxsc) return true;
  sclast.sc = false;
  return false;
}

void TFM::createWeaveFrame(PVideoFrame &dst, PVideoFrame &prv, PVideoFrame &src,
  PVideoFrame &nxt, int match, int &cfrm, IScriptEnvironment * env, const VideoInfo &vi) const
{
  if (cfrm == match)
    return;

  const int np = vi.IsYUY2() || vi.IsY() ? 1 : 3;
  const int planes[3] = { PLANAR_Y, PLANAR_U, PLANAR_V };
  for (int b = 0; b < np; ++b)
  {
    const int plane = planes[b];
    if (match == 0)
    {
      BitBlt(dst->GetWritePtr(plane) + (1 - field)*dst->GetPitch(plane), dst->GetPitch(plane) << 1,
        src->GetReadPtr(plane) + (1 - field)*src->GetPitch(plane), src->GetPitch(plane) << 1,
        src->GetRowSize(plane), src->GetHeight(plane) >> 1);
      BitBlt(dst->GetWritePtr(plane) + field*dst->GetPitch(plane), dst->GetPitch(plane) << 1,
        prv->GetReadPtr(plane) + field*prv->GetPitch(plane), prv->GetPitch(plane) << 1,
        prv->GetRowSize(plane), prv->GetHeight(plane) >> 1);
    }
    else if (match == 1)
    {
      BitBlt(dst->GetWritePtr(plane), dst->GetPitch(plane), src->GetReadPtr(plane),
        src->GetPitch(plane), src->GetRowSize(plane), src->GetHeight(plane));
    }
    else if (match == 2)
    {
      BitBlt(dst->GetWritePtr(plane) + (1 - field)*dst->GetPitch(plane), dst->GetPitch(plane) << 1,
        src->GetReadPtr(plane) + (1 - field)*src->GetPitch(plane), src->GetPitch(plane) << 1,
        src->GetRowSize(plane), src->GetHeight(plane) >> 1);
      BitBlt(dst->GetWritePtr(plane) + field*dst->GetPitch(plane), dst->GetPitch(plane) << 1,
        nxt->GetReadPtr(plane) + field*nxt->GetPitch(plane), nxt->GetPitch(plane) << 1,
        nxt->GetRowSize(plane), nxt->GetHeight(plane) >> 1);
    }
    else if (match == 3)
    {
      BitBlt(dst->GetWritePtr(plane) + field*dst->GetPitch(plane), dst->GetPitch(plane) << 1,
        src->GetReadPtr(plane) + field*src->GetPitch(plane), src->GetPitch(plane) << 1,
        src->GetRowSize(plane), src->GetHeight(plane) >> 1);
      BitBlt(dst->GetWritePtr(plane) + (1 - field)*dst->GetPitch(plane), dst->GetPitch(plane) << 1,
        prv->GetReadPtr(plane) + (1 - field)*prv->GetPitch(plane), prv->GetPitch(plane) << 1,
        prv->GetRowSize(plane), prv->GetHeight(plane) >> 1);
    }
    else if (match == 4)
    {
      BitBlt(dst->GetWritePtr(plane) + field*dst->GetPitch(plane), dst->GetPitch(plane) << 1,
        src->GetReadPtr(plane) + field*src->GetPitch(plane), src->GetPitch(plane) << 1,
        src->GetRowSize(plane), src->GetHeight(plane) >> 1);
      BitBlt(dst->GetWritePtr(plane) + (1 - field)*dst->GetPitch(plane), dst->GetPitch(plane) << 1,
        nxt->GetReadPtr(plane) + (1 - field)*nxt->GetPitch(plane), nxt->GetPitch(plane) << 1,
        nxt->GetRowSize(plane), nxt->GetHeight(plane) >> 1);
    }
    //else env->ThrowError("TFM:  an unknown error occurred (no such match!)");
  }
  cfrm = match;
}

void TFM::putFrameProperties(PVideoFrame& dst, int match, int combed, bool d2vfilm, const int mics[5], IScriptEnvironment* env) const
{
  AVSMap* props = env->getFramePropsRW(dst);

  env->propSetInt(props, PROP_TFMMATCH, match, AVSPropAppendMode::PROPAPPENDMODE_REPLACE);
  env->propSetInt(props, PROP_Combed, combed > 1, AVSPropAppendMode::PROPAPPENDMODE_REPLACE);
  env->propSetInt(props, PROP_TFMD2VFilm, d2vfilm, AVSPropAppendMode::PROPAPPENDMODE_REPLACE);
  env->propSetInt(props, PROP_TFMField, field, AVSPropAppendMode::PROPAPPENDMODE_REPLACE);
  for (int i = 0; i < 5; i++)
    env->propSetInt(props, PROP_TFMMics, mics[i], i ? AVSPropAppendMode::PROPAPPENDMODE_APPEND : AVSPropAppendMode::PROPAPPENDMODE_REPLACE);
  env->propSetInt(props, PROP_TFMPP, PP, AVSPropAppendMode::PROPAPPENDMODE_REPLACE);
}

void TFM::putHint(const VideoInfo& vi, PVideoFrame& dst, int match, int combed, bool d2vfilm)
{
  if (vi.ComponentSize() == 1)
    putHint_core<uint8_t>(dst, match, combed, d2vfilm);
  else
    putHint_core<uint16_t>(dst, match, combed, d2vfilm);
}

template<typename pixel_t>
void TFM::putHint_core(PVideoFrame &dst, int match, int combed, bool d2vfilm)
{
  pixel_t *p = reinterpret_cast<pixel_t *>(dst->GetWritePtr(PLANAR_Y));
  pixel_t *srcp = p;
  unsigned int i, hint = 0;
  unsigned int hint2 = 0, magic_number = 0;
  if (match == 0) hint |= ISP;
  else if (match == 1 && combed < 2) hint |= ISC;
  else if (match == 2) hint |= ISN;
  else if (match == 3) hint |= ISB;
  else if (match == 4) hint |= ISU;
  else if (match == 1 && combed > 1 && field == 0) hint |= ISDB;
  else if (match == 1 && combed > 1 && field == 1) hint |= ISDT;
  if (field == 1) hint |= TOP_FIELD;
  if (combed > 1) hint |= COMBED;
  if (d2vfilm) hint |= D2VFILM;
  for (i = 0; i < 32; ++i)
  {
    magic_number |= ((*srcp++ & 1) << i);
  }
  if (magic_number == MAGIC_NUMBER_2)
  {
    for (i = 0; i < 32; ++i)
    {
      hint2 |= ((*srcp++ & 1) << i);
    }
    hint2 <<= 8;
    hint2 &= 0xFF00;
    hint |= hint2 | 0x80;
  }
  for (i = 0; i < 32; ++i)
  {
    *p &= ~1;
    *p++ |= ((MAGIC_NUMBER & (1 << i)) >> i);
  }
  for (i = 0; i < 32; ++i)
  {
    *p &= ~1;
    *p++ |= ((hint & (1 << i)) >> i);
  }
}


// check in TDeint, plus don't call with aligned width!
template<typename pixel_t>
void TFM::buildDiffMapPlane2(const uint8_t *prvp, const uint8_t *nxtp,
  uint8_t *dstp, int prv_pitch, int nxt_pitch, int dst_pitch, int Height,
  int Width, int bits_per_pixel) const
{
  const bool YUY2_LumaOnly = vi.IsYUY2() && !mChroma;
  do_buildABSDiffMask2<pixel_t>(prvp, nxtp, dstp, prv_pitch, nxt_pitch, dst_pitch, Width, Height, YUY2_LumaOnly, cpuFlags, bits_per_pixel);
}

// instantiate
template void TFM::buildDiffMapPlane2<uint8_t>(const uint8_t* prvp, const uint8_t* nxtp,
  uint8_t* dstp, int prv_pitch, int nxt_pitch, int dst_pitch, int Height,
  int Width, int bits_per_pixel) const;
template void TFM::buildDiffMapPlane2<uint16_t>(const uint8_t* prvp, const uint8_t* nxtp,
  uint8_t* dstp, int prv_pitch, int nxt_pitch, int dst_pitch, int Height,
  int Width, int bits_per_pixel) const;

template<typename pixel_t>
void TFM::buildABSDiffMask(const uint8_t *prvp, const uint8_t *nxtp,
  int prv_pitch, int nxt_pitch, int tpitch, int width, int height) const
{
  const bool YUY2_LumaOnly = vi.IsYUY2() && !mChroma;
  do_buildABSDiffMask<pixel_t>(prvp, nxtp, tbuffer.get(), prv_pitch, nxt_pitch, tpitch, width, height, YUY2_LumaOnly, cpuFlags);
}

// instantiate
template void TFM::buildABSDiffMask<uint8_t>(const uint8_t* prvp, const uint8_t* nxtp,
  int prv_pitch, int nxt_pitch, int tpitch, int width, int height) const;
template void TFM::buildABSDiffMask<uint16_t>(const uint8_t* prvp, const uint8_t* nxtp,
  int prv_pitch, int nxt_pitch, int tpitch, int width, int height) const;


AVSValue __cdecl Create_TFM(AVSValue args, void* user_data, IScriptEnvironment* env)
{
  bool chroma = args[16].IsBool() ? args[16].AsBool() : false;
  VideoInfo vi = args[0].AsClip()->GetVideoInfo();
  if (vi.IsY()) chroma = false;

  AVSValue v = new TFM(args[0].AsClip(), args[1].AsInt(-1), args[2].AsInt(-1), args[3].AsInt(1),
    args[4].AsInt(6), args[5].AsString(""), args[6].AsString(""), args[7].AsString(""), args[8].AsString(""),
    args[9].AsBool(false), args[10].AsBool(false), args[11].AsInt(1), args[12].AsBool(true),
    args[13].AsInt(15), args[14].AsInt(9), args[15].AsInt(80), chroma, args[17].AsInt(16),
    args[18].AsInt(16), args[19].AsInt(0), args[20].AsInt(0), args[23].AsString(""), args[24].AsInt(0),
    args[25].AsInt(4), args[26].AsFloat(12.0), args[27].AsInt(0), args[28].AsInt(1), args[29].AsString(""),
    args[30].AsBool(true), args[31].AsInt(0), args[32].AsBool(false), args[33].AsBool(true),
    args[34].AsBool(true), args[35].AsInt(4), env);
  if (!args[4].IsInt() || args[4].AsInt() >= 2)
  {
    if (!args[4].IsInt() || args[4].AsInt() > 4)
    {
      try { v = env->Invoke("InternalCache", v).AsClip(); }
      catch (IScriptEnvironment::NotFound) {}
    }
    v = new TFMPP(v.AsClip(), args[4].AsInt(6), args[21].AsInt(5), args[5].AsString(""),
      args[10].AsBool(false), (args[22].IsClip() ? args[22].AsClip() : NULL),
      args[30].AsBool(true), args[35].AsInt(4), env);
  }
  return v;
}

TFM::TFM(PClip _child, int _order, int _field, int _mode, int _PP, const char* _ovr,
  const char* _input, const char* _output, const char * _outputC, bool _debug, bool _display,
  int _slow, bool _mChroma, int _cNum, int _cthresh, int _MI, bool _chroma, int _blockx,
  int _blocky, int _y0, int _y1, const char* _d2v, int _ovrDefault, int _flags, double _scthresh,
  int _micout, int _micmatching, const char* _trimIn, bool _usehints, int _metric, bool _batch,
  bool _ubsco, bool _mmsco, int _opt, IScriptEnvironment* env) : GenericVideoFilter(_child),
  order(_order), field(_field), mode(_mode), PP(_PP), ovr(_ovr), input(_input), output(_output),
  outputC(_outputC), debug(_debug), display(_display), slow(_slow), mChroma(_mChroma), cNum(_cNum),
  cthresh(_cthresh), MI(_MI), chroma(_chroma), blockx(_blockx), blocky(_blocky), y0(_y0),
  y1(_y1), d2v(_d2v), ovrDefault(_ovrDefault), flags(_flags), scthresh(_scthresh), micout(_micout),
  micmatching(_micmatching), trimIn(_trimIn), usehints(_usehints), metric(_metric),
  batch(_batch), ubsco(_ubsco), mmsco(_mmsco), opt(_opt), cArray(nullptr, &AlignedDeleter), tbuffer(nullptr, &AlignedDeleter)
{

  map = cmask = NULL;
  int z, w, q = 0, b, i, count, last, fieldt, firstLine, qt;
  int countOvrS, countOvrM;
  char linein[1024];
  char *linep, *linet;
  FILE *f = nullptr;

  has_at_least_v8 = true;
  try { env->CheckVersion(8); }
  catch (const AvisynthError&) { has_at_least_v8 = false; }

  cpuFlags = env->GetCPUFlags();
  if (opt == 0) cpuFlags = 0;

   if (vi.IsY())
    chroma = false;

  if (vi.BitsPerComponent() > 16)
    env->ThrowError("TFM:  only 8-16 bit formats supported!");
  if (!vi.IsYUV())
    env->ThrowError("TFM:  YUV data only!");
  if (vi.height & 1 || vi.width & 1)
    env->ThrowError("TFM:  height and width must be divisible by 2!");
  if (vi.height < 6 || vi.width < 64)
    env->ThrowError("TFM:  frame dimensions too small!");
  if (mode < 0 || mode > 7)
    env->ThrowError("TFM:  mode must be set to 0, 1, 2, 3, 4, 5, 6, or 7!");
  if (field < -1 || field > 1)
    env->ThrowError("TFM:  field must be set to -1, 0, or 1!");
  if (PP < 0 || PP > 7)
    env->ThrowError("TFM:  PP must be at least 0 and less than 8!");
  if (order < -1 || order > 1)
    env->ThrowError("TFM:  order must be set to -1, 0, or 1!");
  if (blockx != 4 && blockx != 8 && blockx != 16 && blockx != 32 && blockx != 64 &&
    blockx != 128 && blockx != 256 && blockx != 512 && blockx != 1024 && blockx != 2048)
    env->ThrowError("TFM:  illegal blockx size!");
  if (blocky != 4 && blocky != 8 && blocky != 16 && blocky != 32 && blocky != 64 &&
    blocky != 128 && blocky != 256 && blocky != 512 && blocky != 1024 && blocky != 2048)
    env->ThrowError("TFM:  illegal blocky size!");
  if (y0 != y1 && (y0 < 0 || y1 < 0 || y0 > y1 || y1 > vi.height || y0 > vi.height))
    env->ThrowError("TFM:  bad y0 and y1 exclusion band values!");
  if (ovrDefault < 0 || ovrDefault > 2)
    env->ThrowError("TFM:  ovrDefault must be set to 0, 1, or 2!");
  if (flags < 0 || flags > 5)
    env->ThrowError("TFM:  flags must be set to 0, 1, 2, 3, 4, or 5!");
  if (slow < 0 || slow > 2)
    env->ThrowError("TFM:  slow must be set to 0, 1, or 2!");
  if (micout < 0 || micout > 2)
    env->ThrowError("TFM:  micout must be set to 0, 1, or 2!");
  if (micmatching < 0 || micmatching > 4)
    env->ThrowError("TFM:  micmatching must be set to 0, 1, 2, 3, or 4!");
  if (opt < 0 || opt > 4)
    env->ThrowError("TFM:  opt must be set to 0, 1, 2, 3, or 4!");
  if (metric != 0 && metric != 1)
    env->ThrowError("TFM:  metric must be set to 0 or 1!");
  if (scthresh < 0.0 || scthresh > 100.0)
    env->ThrowError("TFM:  scthresh must be between 0.0 and 100.0 (inclusive)!");
  if (debug)
  {
    sprintf(buf, "TFM:  %s by tritical\n", TFM_VERSION);
    OutputDebugString(buf);
  }

  child->SetCacheHints(CACHE_GENERIC, 3);  // fixed to diameter (07/30/2005)

  lastMatch.frame = lastMatch.field = lastMatch.combed = lastMatch.match = -20;
  nfrms = vi.num_frames - 1;
  f = nullptr;
  mode_origSaved = mode;
  PP_origSaved = PP;
  MI_origSaved = MI;
  d2vpercent = -20.00f;
  vidCount = 0;

  xhalf = blockx >> 1;
  yhalf = blocky >> 1;

  xshift = blockx == 4 ? 2 : blockx == 8 ? 3 : blockx == 16 ? 4 : blockx == 32 ? 5 :
    blockx == 64 ? 6 : blockx == 128 ? 7 : blockx == 256 ? 8 : blockx == 512 ? 9 :
    blockx == 1024 ? 10 : 11;
  yshift = blocky == 4 ? 2 : blocky == 8 ? 3 : blocky == 16 ? 4 : blocky == 32 ? 5 :
    blocky == 64 ? 6 : blocky == 128 ? 7 : blocky == 256 ? 8 : blocky == 512 ? 9 :
    blocky == 1024 ? 10 : 11;


  // no high bit depth scaling here
  // Warning: this mod16 must match with the calculation in "checkSceneChange"
  diffmaxsc = int((double(((vi.width >> 4) << 4)*vi.height * (235-16))*scthresh*0.5) / 100.0);

  sclast.frame = -20;
  sclast.sc = true;

  if (mode == 1 || mode == 2 || mode == 3 || mode == 5 || mode == 6 || mode == 7 ||
    PP > 0 || micout > 0 || micmatching > 0)
  {
    const int numElements = (((vi.width + xhalf) >> xshift) + 1) * (((vi.height + yhalf) >> yshift) + 1) * 4;
    int* buffer = static_cast<int*>(_aligned_malloc(numElements * sizeof(int), FRAME_ALIGN));
    cArray.reset(buffer);

    if (!cArray) env->ThrowError("TFM:  malloc failure (cArray)!");
    cmask = new PlanarFrame(vi, true, cpuFlags);
  }

  VideoInfo vi_map = vi; // prepare map format: always 8 bits
  vi_map.pixel_type = (vi_map.pixel_type & ~VideoInfo::CS_Sample_Bits_Mask) | VideoInfo::CS_Sample_Bits_8;
  map = new PlanarFrame(vi_map, true, cpuFlags);

  if (vi.IsYUY2())
  {
    xhalf *= 2;
    ++xshift;
  }
  if (d2v.size())
  {
    parseD2V(env);
    trimArray.resize(0);
  }
  order_origSaved = order;
  field_origSaved = fieldO = field;
  if (fieldO == -1)
  {
    if (order == -1) {
       if(has_at_least_v8) {
         char error[512] = "TFM: Couldn't fetch the first frame from the input clip to determine the clip's field order. Reason: ";
         size_t len = strlen(error);

         PVideoFrame first_frame = child->GetFrame(0, env);
         const AVSMap *props = env->getFramePropsRO(first_frame);

         int err;
         int64_t field_based = env->propGetInt(props, "_FieldBased", 0, &err);
         if (err) {
            fieldO = child->GetParity(0) ? 1 : 0; // fallback to traditional Avisynth
            /* VS case: strict frame prop dependency
            env->ThrowError("TFM: Couldn't find the '_FieldBased' frame property. The 'order' parameter must be used.");
            */
         }
         else {
        /// Pretend it's top field first when it says progressive?
         fieldO = (field_based == TopFieldFirst || field_based == Progressive);
          }
        } else {
          fieldO = child->GetParity(0) ? 1 : 0; // no frameprop support, old method
        }

    }
    else fieldO = order;
  }
  tpitchy = tpitchuv = -20;

  const int ALIGN_BUF = FRAME_ALIGN;
  if (vi.IsPlanar())
  {
    // tbuffer is 8 or 16 bits wide
    const int pixelsize = vi.ComponentSize();
    tpitchy = AlignNumber(vi.width * pixelsize, ALIGN_BUF);
    const int widthUV = !vi.IsY() && !vi.IsRGB() ? vi.width >> vi.GetPlaneWidthSubsampling(PLANAR_U) : 0;
    tpitchuv = AlignNumber(widthUV * pixelsize, ALIGN_BUF);
  }
  else { // YUY2
    tpitchy = AlignNumber((vi.width << 1), ALIGN_BUF);
  }
  // 16 would be is enough for sse2 but maybe we'll do AVX2?
  // unique_ptr!

  const int numElements = (vi.height >> 1) * tpitchy;
  uint8_t* buffer = static_cast<uint8_t*>(_aligned_malloc(numElements * sizeof(uint8_t), ALIGN_BUF));
  tbuffer.reset(buffer);

  if (tbuffer == NULL)
    env->ThrowError("TFM:  malloc failure (tbuffer)!");
  mode7_field = field;
  if (input.size())
  {
    bool d2vmarked, micmarked;
    if ((f = tivtc_fopen(input.c_str(), "r")) != NULL)
    {
      ovrArray.resize(vi.num_frames, 255);
      if (d2vfilmarray.size() == 0)
      {
        d2vfilmarray.resize(vi.num_frames + 1, 0);
      }
      fieldt = fieldO;
      firstLine = 0;
      if (debug)
      {
        sprintf(buf, "TFM:  successfully opened input file.  Field defaulting to - %s.\n",
          fieldt == 0 ? "bottom" : "top");
        OutputDebugString(buf);
      }
      while (fgets(linein, 1024, f) != nullptr)
      {
        if (linein[0] == 0 || linein[0] == '\n' || linein[0] == '\r' || linein[0] == ';' || linein[0] == '#')
          continue;
        ++firstLine;
        linep = linein;
        while (*linep != 'f' && *linep != 'F' && *linep != 0 && *linep != ' ' && *linep != 'c') linep++;
        if (*linep == 'f' || *linep == 'F')
        {
          if (firstLine == 1)
          {
            bool changed = false;
            if (_strnicmp(linein, "field = top", 11) == 0) { fieldt = 1; changed = true; }
            else if (_strnicmp(linein, "field = bottom", 14) == 0) { fieldt = 0; changed = true; }
            if (debug && changed)
            {
              sprintf(buf, "TFM:  detected field for input file - %s.\n",
                fieldt == 0 ? "bottom" : "top");
              OutputDebugString(buf);
            }
          }
        }
        else if (*linep == 'c')
        {
          if (_strnicmp(linein, "crc32 = ", 8) == 0)
          {
            linet = linein;
            while (*linet != ' ') linet++;
            linet++;
            while (*linet != ' ') linet++;
            linet++;
            unsigned int m, tempCrc;
            sscanf(linet, "%x", &m);
            calcCRC(child, 15, tempCrc, env);
            if (tempCrc != m && !batch)
            {
              fclose(f);
              f = nullptr;
              env->ThrowError("TFM:  crc32 in input file does not match that of the current clip (%#x vs %#x)!",
                m, tempCrc);
            }
          }
        }
        else if (*linep == ' ')
        {
          linet = linein;
          while (*linet != 0)
          {
            if (*linet != ' ' && *linet != 10) break;
            linet++;
          }
          if (*linet == 0) { --firstLine; continue; }
          sscanf(linein, "%d", &z);
          linep = linein;
          while (*linep != 'p' && *linep != 'c' && *linep != 'n' && *linep != 'u' &&
            *linep != 'b' && *linep != 'l' && *linep != 'h' && *linep != 0) linep++;
          if (*linep != 0)
          {
            if (z<0 || z>nfrms)
            {
              fclose(f);
              f = nullptr;
              env->ThrowError("TFM:  input file error (out of range or non-ascending frame #)!");
            }
            linep = linein;
            while (*linep != ' ' && *linep != 0) linep++;
            if (*linep != 0)
            {
              qt = -1;
              d2vmarked = micmarked = false;
              linep++;
              q = *linep;
              if (q == 112) q = 0;
              else if (q == 99) q = 1;
              else if (q == 110) q = 2;
              else if (q == 98) q = 3;
              else if (q == 117) q = 4;
              else if (q == 108) q = 5;
              else if (q == 104) q = 6;
              else
              {
                fclose(f);
                f = nullptr;
                env->ThrowError("TFM:  input file error (invalid match specifier)!");
              }
              linep++;
              linep++;
              if (*linep != 0)
              {
                qt = *linep;
                if (qt == 45) qt = 0;
                else if (qt == 43) qt = COMBED;
                else if (qt == '1') { d2vmarked = true; qt = -1; }
                else if (qt == '[') { micmarked = true; qt = -1; }
                else
                {
                  fclose(f);
                  f = nullptr;
                  env->ThrowError("TFM:  input file error (invalid specifier)!");
                }
              }
              if (fieldt != fieldO)
              {
                if (q == 0) q = 3;
                else if (q == 2) q = 4;
                else if (q == 3) q = 0;
                else if (q == 4) q = 2;
              }
              if (!d2vmarked && !micmarked && qt != -1)
              {
                linep++;
                linep++;
                if (*linep == '1') d2vmarked = true;
                else if (*linep == '[') micmarked = true;
              }
              if (d2vmarked)
              {
                d2vfilmarray[z] &= ~0x03;
                d2vfilmarray[z] |= fieldt == 1 ? 0x3 : 0x1;
                if (!micmarked)
                {
                  linep++;
                  linep++;
                  if (*linep == '[') micmarked = true;
                }
              }
              if (micmarked)
              {
                // add mic input handling in the future
              }
              ovrArray[z] |= 0x07;
              ovrArray[z] &= (q | 0xF8);
              if (qt != -1)
              {
                ovrArray[z] &= 0xDF;
                ovrArray[z] |= 0x10;
                ovrArray[z] &= (qt | 0xEF);
              }
            }
          }
        }
      }
      fclose(f);
      f = nullptr;
    }
    else env->ThrowError("TFM:  input file error (could not open file)!");
  }
  if (ovr.size())
  {
    if ((f = tivtc_fopen(ovr.c_str(), "r")) != nullptr)
    {
      countOvrS = countOvrM = 0;
      while (fgets(linein, 1024, f) != nullptr)
      {
        if (linein[0] == 0 || linein[0] == '\n' || linein[0] == '\r' || linein[0] == ';' || linein[0] == '#')
          continue;
        linep = linein;
        while (*linep != 'c' && *linep != 'p' && *linep != 'n' && *linep != 'b' &&
          *linep != 'u' && *linep != 'l' && *linep != 'h' && *linep != '+' && *linep != '-' && *linep != 0) linep++;
        if (*linep == 0) ++countOvrS;
        else ++countOvrM;
      }
      fclose(f);
      f = nullptr;
      if (ovrDefault != 0 && ovrArray.size())

      {
        if (ovrDefault == 1) q = 0;
        else if (ovrDefault == 2) q = COMBED;
        for (int h = 0; h < vi.num_frames; ++h)
        {
          ovrArray[h] &= 0xDF;
          ovrArray[h] |= 0x10;
          ovrArray[h] &= (q | 0xEF);
          if (q == 0 && ((ovrArray[h] & 7) == 6 ||
            (ovrArray[h] & 7) == 5))
          {
            ovrArray[h] |= 0x07;
            ovrArray[h] &= (1 | 0xF8);
          }
        }
      }
      if (countOvrS == 0 && countOvrM == 0) { goto emptyovr; }
      if (countOvrS > 0)
      {
        ++countOvrS;
        countOvrS *= 4;
        setArray.resize(countOvrS, 0xffffffff);
      }
      if (countOvrM > 0 && ovrArray.size() == 0)
      {
        ovrArray.resize(vi.num_frames, 255);
        if (ovrDefault != 0)
        {
          if (ovrDefault == 1) q = 0;
          else if (ovrDefault == 2) q = COMBED;
          for (int h = 0; h < vi.num_frames; ++h)
          {
            ovrArray[h] &= 0xDF;
            ovrArray[h] |= 0x10;
            ovrArray[h] &= (q | 0xEF);
          }
        }
      }
      last = -1;
      fieldt = fieldO;
      firstLine = 0;
      i = 0;
      if ((f = tivtc_fopen(ovr.c_str(), "r")) != nullptr)
      {
        if (debug)
        {
          sprintf(buf, "TFM:  successfully opened ovr file.  Field defaulting to - %s.\n",
            fieldt == 0 ? "bottom" : "top");
          OutputDebugString(buf);
        }
        while (fgets(linein, 1024, f) != nullptr)
        {
          if (linein[0] == 0 || linein[0] == '\n' || linein[0] == '\r' || linein[0] == ';' || linein[0] == '#')
            continue;
          ++firstLine;
          linep = linein;
          while (*linep != 'f' && *linep != 'F' && *linep != 0 && *linep != ' ' && *linep != ',') linep++;
          if (*linep == 'f' || *linep == 'F')
          {
            if (firstLine == 1)
            {
              bool changed = false;
              if (_strnicmp(linein, "field = top", 11) == 0) { fieldt = 1; changed = true; }
              else if (_strnicmp(linein, "field = bottom", 14) == 0) { fieldt = 0; changed = true; }
              if (debug && changed)
              {
                sprintf(buf, "TFM:  detected field for ovr file - %s.\n",
                  fieldt == 0 ? "bottom" : "top");
                OutputDebugString(buf);
              }
            }
          }
          else if (*linep == ' ')
          {
            linet = linein;
            while (*linet != 0)
            {
              if (*linet != ' ' && *linet != 10) break;
              linet++;
            }
            if (*linet == 0) { --firstLine; continue; }
            linep++;
            if (*linep == 'p' || *linep == 'c' || *linep == 'n' || *linep == 'b' || *linep == 'u' || *linep == 'l' || *linep == 'h')
            {
              sscanf(linein, "%d", &z);
              if (z<0 || z>nfrms || z <= last)
              {
                fclose(f);
                f = nullptr;
                env->ThrowError("TFM:  ovr file error (out of range or non-ascending frame #)!");
              }
              linep = linein;
              while (*linep != ' ' && *linep != 0) linep++;
              if (*linep != 0)
              {
                linep++;
                q = *linep;
                if (q == 112) q = 0;
                else if (q == 99) q = 1;
                else if (q == 110) q = 2;
                else if (q == 98) q = 3;
                else if (q == 117) q = 4;
                else if (q == 108) q = 5;
                else if (q == 104) q = 6;
                else
                {
                  fclose(f);
                  f = nullptr;
                  env->ThrowError("TFM:  ovr file error (invalid match specifier)!");
                }
                if (fieldt != fieldO)
                {
                  if (q == 0) q = 3;
                  else if (q == 2) q = 4;
                  else if (q == 3) q = 0;
                  else if (q == 4) q = 2;
                }
                ovrArray[z] |= 0x07;
                ovrArray[z] &= (q | 0xF8);
                last = z;
              }
            }
            else if (*linep == '-' || *linep == '+')
            {
              sscanf(linein, "%d", &z);
              if (z<0 || z>nfrms)
              {
                fclose(f);
                f = nullptr;
                env->ThrowError("TFM:  ovr file error (out of range or non-ascending frame #)!");
              }
              linep = linein;
              while (*linep != ' ' && *linep != 0) linep++;
              if (*linep != 0)
              {
                linep++;
                q = *linep;
                if (q == 45) q = 0;
                else if (q == 43) q = COMBED;
                else
                {
                  fclose(f);
                  f = nullptr;
                  env->ThrowError("TFM:  ovr file error (invalid symbol)!");
                }
                ovrArray[z] &= 0xDF;
                ovrArray[z] |= 0x10;
                ovrArray[z] &= (q | 0xEF);
                if (q == 0 && ((ovrArray[z] & 7) == 6 ||
                  (ovrArray[z] & 7) == 5))
                {
                  ovrArray[z] |= 0x07;
                  ovrArray[z] &= (1 | 0xF8);
                }
              }
            }
            else
            {
              sscanf(linein, "%d", &z);
              if (z<0 || z>nfrms)
              {
                fclose(f);
                f = nullptr;
                env->ThrowError("TFM:  ovr input error (out of range frame #)!");
              }
              linep = linein;
              while (*linep != ' ' && *linep != 0) linep++;
              if (*linep != 0)
              {
                linep++;
                if (*linep == 'f' || *linep == 'm' || *linep == 'o' || *linep == 'P' || *linep == 'i')
                {
                  q = *linep;
                  linep++;
                  linep++;
                  if (*linep == 0) continue;
                  sscanf(linep, "%d", &b);
                  if (q == 102 && b != 0 && b != 1 && b != -1)
                  {
                    fclose(f);
                    f = nullptr;
                    env->ThrowError("TFM:  ovr input error (bad field value)!");
                  }
                  else if (q == 111 && b != 0 && b != 1 && b != -1)
                  {
                    fclose(f);
                    f = nullptr;
                    env->ThrowError("TFM:  ovr input error (bad order value)!");
                  }
                  else if (q == 109 && (b < 0 || b > 7))
                  {
                    fclose(f);
                    f = nullptr;
                    env->ThrowError("TFM:  ovr input error (bad mode value)!");
                  }
                  else if (q == 80 && (b < 0 || b > 7))
                  {
                    fclose(f);
                    f = nullptr;
                    env->ThrowError("TFM:  ovr input error (bad PP value)!");
                  }
                  setArray[i] = q; ++i;
                  setArray[i] = z; ++i;
                  setArray[i] = z; ++i;
                  setArray[i] = b; ++i;
                }
              }
            }
          }
          else if (*linep == ',')
          {
            while (*linep != ' ' && *linep != 0) linep++;
            if (*linep == 0) continue;
            linep++;
            if (*linep == 'p' || *linep == 'c' || *linep == 'n' || *linep == 'u' || *linep == 'b' || *linep == 'l' || *linep == 'h')
            {
              sscanf(linein, "%d,%d", &z, &w);
              if (w == 0) w = nfrms;
              if (z<0 || z>nfrms || w<0 || w>nfrms || w < z || z <= last)
              {
                fclose(f);
                f = nullptr;
                env->ThrowError("TFM:  input file error (out of range or non-ascending frame #)!");
              }
              linep = linein;
              while (*linep != ' ' && *linep != 0) linep++;
              if (*linep != 0)
              {
                linep++;
                if (*(linep + 1) == 'p' || *(linep + 1) == 'c' || *(linep + 1) == 'n' || *(linep + 1) == 'b' || *(linep + 1) == 'u' || *(linep + 1) == 'l' || *(linep + 1) == 'h')
                {
                  count = 0;
                  while ((*linep == 'p' || *linep == 'c' || *linep == 'n' || *linep == 'b' || *linep == 'u' || *linep == 'l' || *linep == 'h') && (z + count <= w))
                  {
                    q = *linep;
                    if (q == 112) q = 0;
                    else if (q == 99) q = 1;
                    else if (q == 110) q = 2;
                    else if (q == 98) q = 3;
                    else if (q == 117) q = 4;
                    else if (q == 108) q = 5;
                    else if (q == 104) q = 6;
                    else
                    {
                      fclose(f);
                      f = nullptr;
                      env->ThrowError("TFM:  input file error (invalid match specifier)!");
                    }
                    if (fieldt != fieldO)
                    {
                      if (q == 0) q = 3;
                      else if (q == 2) q = 4;
                      else if (q == 3) q = 0;
                      else if (q == 4) q = 2;
                    }
                    ovrArray[z + count] |= 0x07;
                    ovrArray[z + count] &= (q | 0xF8);
                    ++count;
                    linep++;
                  }
                  while (z + count <= w)
                  {
                    ovrArray[z + count] |= 0x07;
                    ovrArray[z + count] &= (ovrArray[z] | 0xF8);
                    ++z;
                  }
                  last = w;
                }
                else
                {
                  q = *linep;
                  if (q == 112) q = 0;
                  else if (q == 99) q = 1;
                  else if (q == 110) q = 2;
                  else if (q == 98) q = 3;
                  else if (q == 117) q = 4;
                  else if (q == 108) q = 5;
                  else if (q == 104) q = 6;
                  else
                  {
                    fclose(f);
                    f = nullptr;
                    env->ThrowError("TFM:  input file error (invalid match specifier)!");
                  }
                  if (fieldt != fieldO)
                  {
                    if (q == 0) q = 3;
                    else if (q == 2) q = 4;
                    else if (q == 3) q = 0;
                    else if (q == 4) q = 2;
                  }
                  while (z <= w)
                  {
                    ovrArray[z] |= 0x07;
                    ovrArray[z] &= (q | 0xF8);
                    ++z;
                  }
                  last = w;
                }
              }
            }
            else if (*linep == '-' || *linep == '+')
            {
              sscanf(linein, "%d,%d", &z, &w);
              if (w == 0) w = nfrms;
              if (z<0 || z>nfrms || w<0 || w>nfrms || w < z)
              {
                fclose(f);
                f = nullptr;
                env->ThrowError("TFM:  input file error (out of range or non-ascending frame #)!");
              }
              linep = linein;
              while (*linep != ' ' && *linep != 0) linep++;
              if (*linep != 0)
              {
                linep++;
                if (*(linep + 1) == '-' || *(linep + 1) == '+')
                {
                  count = 0;
                  while ((*linep == '-' || *linep == '+') && (z + count <= w))
                  {
                    q = *linep;
                    if (q == 45) q = 0;
                    else if (q == 43) q = COMBED;
                    else
                    {
                      fclose(f);
                      f = nullptr;
                      env->ThrowError("TFM:  input file error (invalid symbol)!");
                    }
                    ovrArray[z + count] &= 0xDF;
                    ovrArray[z + count] |= 0x10;
                    ovrArray[z + count] &= (q | 0xEF);
                    if (q == 0 && ((ovrArray[z + count] & 7) == 6 ||
                      (ovrArray[z + count] & 7) == 5))
                    {
                      ovrArray[z + count] |= 0x07;
                      ovrArray[z + count] &= (1 | 0xF8);
                    }
                    ++count;
                    linep++;
                  }
                  while (z + count <= w)
                  {
                    ovrArray[z + count] &= 0xDF;
                    ovrArray[z + count] |= 0x10;
                    ovrArray[z + count] &= (ovrArray[z] | 0xEF);
                    if ((ovrArray[z] & 0x10) == 0 && ((ovrArray[z + count] & 7) == 6 ||
                      (ovrArray[z + count] & 7) == 5))
                    {
                      ovrArray[z + count] |= 0x07;
                      ovrArray[z + count] &= (1 | 0xF8);
                    }
                    ++z;
                  }
                }
                else
                {
                  q = *linep;
                  if (q == 45) q = 0;
                  else if (q == 43) q = COMBED;
                  else
                  {
                    fclose(f);
                    f = nullptr;
                    env->ThrowError("TFM:  input file error (invalid symbol)!");
                  }
                  while (z <= w)
                  {
                    ovrArray[z] &= 0xDF;
                    ovrArray[z] |= 0x10;
                    ovrArray[z] &= (q | 0xEF);
                    if (q == 0 && ((ovrArray[z] & 7) == 6 ||
                      (ovrArray[z] & 7) == 5))
                    {
                      ovrArray[z] |= 0x07;
                      ovrArray[z] &= (1 | 0xF8);
                    }
                    ++z;
                  }
                }
              }
            }
            else
            {
              sscanf(linein, "%d,%d", &z, &w);
              if (w == 0) w = nfrms;
              if (z<0 || z>nfrms || w<0 || w>nfrms || w < z)
              {
                fclose(f);
                f = nullptr;
                env->ThrowError("TFM: ovr input error (invalid frame range)!");
              }
              linep = linein;
              while (*linep != ' ' && *linep != 0) linep++;
              if (*linep != 0)
              {
                linep++;
                if (*linep == 'f' || *linep == 'm' || *linep == 'o' || *linep == 'P' || *linep == 'i')
                {
                  q = *linep;
                  linep++;
                  linep++;
                  if (*linep == 0) continue;
                  sscanf(linep, "%d", &b);
                  if (q == 102 && b != 0 && b != 1 && b != -1)
                  {
                    fclose(f);
                    f = nullptr;
                    env->ThrowError("TFM:  ovr input error (bad field value)!");
                  }
                  else if (q == 111 && b != 0 && b != 1 && b != -1)
                  {
                    fclose(f);
                    f = nullptr;
                    env->ThrowError("TFM:  ovr input error (bad order value)!");
                  }
                  else if (q == 109 && (b < 0 || b > 7))
                  {
                    fclose(f);
                    f = nullptr;
                    env->ThrowError("TFM:  ovr input error (bad mode value)!");
                  }
                  else if (q == 80 && (b < 0 || b > 7))
                  {
                    fclose(f);
                    f = nullptr;
                    env->ThrowError("TFM:  ovr input error (bad PP value)!");
                  }
                  setArray[i] = q; ++i;
                  setArray[i] = z; ++i;
                  setArray[i] = w; ++i;
                  setArray[i] = b; ++i;
                }
              }
            }
          }
        }
        fclose(f);
        f = nullptr;
      }
      else env->ThrowError("TFM:  ovr file error (could not open file)!");
    }
    else env->ThrowError("TFM:  ovr input error (could not open ovr file)!");
  }
emptyovr:
  if (output.size())
  {
    if ((f = tivtc_fopen(output.c_str(), "w")) != NULL)
    {
#ifdef _WIN32
      _fullpath(outputFull, output.c_str(), MAX_PATH);
#else
      realpath(output.c_str(), outputFull);
#endif
      calcCRC(child, 15, outputCrc, env);
      fclose(f);
      f = nullptr;
      outArray.resize(vi.num_frames, 0);
      moutArray.resize(vi.num_frames, -1);

      if (micout > 0)
      {
        int sn = micout == 1 ? 3 : 5;
        moutArrayE.resize(vi.num_frames* sn, -20);
      }
    }
    else env->ThrowError("TFM:  output file error (cannot create file)!");
  }
  if (outputC.size())
  {
    if ((f = tivtc_fopen(outputC.c_str(), "w")) != NULL)
    {
#ifdef _WIN32
      _fullpath(outputCFull, outputC.c_str(), MAX_PATH);
#else
      realpath(outputC.c_str(), outputCFull);
#endif
      fclose(f);
      f = nullptr;
      if (outArray.size() == 0)
      {
        outArray.resize(vi.num_frames, 0);
      }
    }
    else env->ThrowError("TFM:  outputC file error (cannot create file)!");
  }
  /// Vapoursynth comment:
  /// attach the value of PP to the first frame? TDecimate uses this to do something in the constructor while processing the tfmIn file.
  ///
  AVSValue tfmPassValue(PP);
  const char *varname = "TFMPPValue";
  env->SetVar(varname, tfmPassValue);
  if (f != NULL) fclose(f);
}

TFM::~TFM()
{
  if (map) delete map;
  if (cmask) delete cmask;

  if (outArray.size())
  {
    FILE *f = nullptr;
    if (output.size())
    {
      if ((f = tivtc_fopen(outputFull, "w")) != NULL)
      {
        char tempBuf[40], tb2[40];
        int match, sn = micout == 1 ? 3 : 5;
        if (moutArrayE.size())
        {
          for (int i = 0; i < sn*vi.num_frames; ++i)
          {
            if (moutArrayE[i] == -20) moutArrayE[i] = -1;
          }
        }
        fprintf(f, "#TFM %s by tritical\n", TFM_VERSION);
        fprintf(f, "field = %s\n", fieldO == 1 ? "top" : "bottom");
        fprintf(f, "crc32 = %x\n", outputCrc);
        for (int h = 0; h <= nfrms; ++h)
        {
          if (outArray[h] & FILE_ENTRY)
          {
            match = (outArray[h] & 0x07);
            sprintf(tempBuf, "%d %c", h, MTC(match));
            if (outArray[h] & 0x20)
            {
              if (outArray[h] & 0x10) strcat(tempBuf, " +");
              else strcat(tempBuf, " -");
            }
            if (outArray[h] & FILE_D2V) strcat(tempBuf, " 1");
            if (moutArray.size() && moutArray[h] != -1)
            {
              sprintf(tb2, " [%d]", moutArray[h]);
              strcat(tempBuf, tb2);
            }
            if (moutArrayE.size())
            {
              int th = h*sn;
              if (sn == 3) sprintf(tb2, " (%d %d %d)", moutArrayE[th + 0],
                moutArrayE[th + 1], moutArrayE[th + 2]);
              else sprintf(tb2, " (%d %d %d %d %d)", moutArrayE[th + 0],
                moutArrayE[th + 1], moutArrayE[th + 2], moutArrayE[th + 3],
                moutArrayE[th + 4]);
              strcat(tempBuf, tb2);
            }
            strcat(tempBuf, "\n");
            fprintf(f, "%s", tempBuf);
          }
        }
        generateOvrHelpOutput(f);
        fclose(f);
        f = nullptr;
      }
    }
    if (outputC.size())
    {
      if ((f = tivtc_fopen(outputCFull, "w")) != nullptr)
      {
        int count = 0, match;
        fprintf(f, "#TFM %s by tritical\n", TFM_VERSION);
        for (int h = 0; h <= nfrms; ++h)
        {
          if (outArray[h] & FILE_ENTRY) match = (outArray[h] & 0x07);
          else match = 0;
          if (match == 1 || match == 5 || match == 6) ++count;
          else
          {
            if (count > cNum) fprintf(f, "%d,%d\n", h - count, h - 1);
            count = 0;
          }
        }
        if (count > cNum) fprintf(f, "%d,%d\n", nfrms - count + 1, nfrms);
        fclose(f);
        f = nullptr;
      }
    }
    if (f != nullptr) fclose(f);
  }
}

void TFM::generateOvrHelpOutput(FILE *f) const
{
  int ccount = 0, mcount = 0, acount = 0;
  int ordert = order == -1 ? child->GetParity(0) : order;
  int ao = fieldO^ordert ? 0 : 2;
  for (int i = 0; i < vi.num_frames; ++i)
  {
    if (!(outArray[i] & FILE_ENTRY)) return;
    const int temp = outArray[i] & 0x07;
    if (temp == 3 || temp == 4 || temp == ao) ++acount;
    if (moutArray[i] != -1) ++mcount;
    if ((outArray[i] & 0x30) == 0x30) ++ccount;
  }
  fprintf(f, "#\n#\n# OVR HELP INFORMATION:\n#\n");
  fprintf(f, "# [COMBED FRAMES]\n#\n");
  fprintf(f, "#   [Individual Frames]\n");
  fprintf(f, "#   FORMAT:  frame_number (mic_value)\n#\n");
  if (PP == 0) fprintf(f, "#   none detected (PP=0)\n");
  else if (ccount)
  {
    for (int i = 0; i < vi.num_frames; ++i)
    {
      if ((outArray[i] & 0x30) == 0x30)
      {
        if (moutArray[i] < 0) fprintf(f, "#   %d\n", i);
        else fprintf(f, "#   %d (%d)\n", i, moutArray[i]);
      }
    }
  }
  else fprintf(f, "#   none detected\n");
  fprintf(f, "#\n#   [Grouped Ranges Allowing Small Breaks]\n");
  fprintf(f, "#   FORMAT:  frame_start, frame_end (percentage combed)\n#\n");
  if (PP == 0) fprintf(f, "#   none detected (PP=0)\n");
  else if (ccount)
  {
    int icount = 0, pcount = 0, rcount = 0, i = 0;
    for (; i < vi.num_frames; ++i)
    {
      if ((outArray[i] & 0x30) == 0x30)
      {
        ++icount;
        ++rcount;
        pcount = 0;
      }
      else
      {
        ++pcount;
        if (rcount > 0) ++rcount;
        if (pcount > 12)
        {
          if (icount > 1)
            fprintf(f, "#   %d,%d (%3.1f%c)\n", i - rcount + 1, i - pcount,
              icount*100.0 / double(rcount - pcount), '%');
          rcount = icount = 0;
        }
      }
    }
    if (icount > 1)
      fprintf(f, "#   %d,%d (%3.1f%c)\n", i - rcount, i - pcount,
        icount*100.0 / double(rcount - pcount), '%');
  }
  else fprintf(f, "#   none detected\n");
  fprintf(f, "#\n#\n# [POSSIBLE MISSED COMBED FRAMES]\n#\n");
  fprintf(f, "#   FORMAT:  frame_number (mic_value)\n#\n");
  if (PP == 0) fprintf(f, "#   none detected (PP=0)\n");
  else if (mcount)
  {
    int maxcp = int(MI*0.85), count = 0;
    int mt = std::max(int(MI*0.1875), 5);
    for (int i = 0; i < vi.num_frames; ++i)
    {
      if ((outArray[i] & 0x30) == 0x30)
        continue;
      const int prev = i > 0 ? moutArray[i - 1] : 0;
      const int curr = moutArray[i];
      const int next = i < vi.num_frames - 1 ? moutArray[i + 1] : 0;
      if (curr <= MI && ((curr >= mt && curr > next * 2 && curr > prev * 2 &&
        curr - next > mt && curr - prev > mt) || (curr > maxcp) ||
        (prev > MI && next > MI && curr > MI*0.5) ||
        ((prev > MI || next > MI) && curr > MI*0.75)))
      {
        fprintf(f, "#   %d (%d)\n", i, moutArray[i]);
        ++count;
      }
    }
    if (!count) fprintf(f, "#   none detected\n");
  }
  else fprintf(f, "#   none detected\n");
  fprintf(f, "#\n#\n# [u, b, AND AGAINST ORDER (%c) MATCHES]\n#\n", MTC(ao));
  fprintf(f, "#   FORMAT:  frame_number match  or  range_start,range_end match\n#\n");
  if (acount)
  {
    int lastf = -1, count = 0, i = 0;
    for (; i < vi.num_frames; ++i)
    {
      const int temp = outArray[i] & 0x07;
      if (temp == 3 || temp == 4 || temp == ao)
      {
        if (lastf == -1) lastf = temp;
        else if (temp != lastf)
        {
          if (count == 1) fprintf(f, "#   %d %c\n", i - 1, MTC(lastf));
          else fprintf(f, "#   %d,%d %c\n", i - count, i - 1, MTC(lastf));
          count = 0;
          lastf = temp;
        }
        ++count;
      }
      else if (count)
      {
        if (count == 1) fprintf(f, "#   %d %c\n", i - 1, MTC(lastf));
        else fprintf(f, "#   %d,%d %c\n", i - count, i - 1, MTC(lastf));
        count = 0;
        lastf = -1;
      }
    }
    if (count == 1) fprintf(f, "#   %d %c\n", i - 1, MTC(lastf));
    else if (count > 1) fprintf(f, "#   %d,%d %c\n", i - count, i - 1, MTC(lastf));
  }
  else fprintf(f, "#   none detected\n");
}
