/* Copyright (c) 2020, Samsung Electronics Co., Ltd.
   All Rights Reserved. */
/*
   Redistribution and use in source and binary forms, with or without
   modification, are permitted provided that the following conditions are met:

   - Redistributions of source code must retain the above copyright notice,
   this list of conditions and the following disclaimer.

   - Redistributions in binary form must reproduce the above copyright notice,
   this list of conditions and the following disclaimer in the documentation
   and/or other materials provided with the distribution.

   - Neither the name of the copyright owner, nor the names of its contributors
   may be used to endorse or promote products derived from this software
   without specific prior written permission.

   THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
   AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
   IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
   ARE DISCLAIMED.IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE
   LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
   CONSEQUENTIAL DAMAGES(INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
   SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
   INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
   CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
   ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
   POSSIBILITY OF SUCH DAMAGE.
*/

#ifndef _XEVD_ITDQ_H_
#define _XEVD_ITDQ_H_
#include "xevd_def.h"
typedef void INV_TRANS(s16 *, s16 *, int, int, int, int);
extern const XEVD_ITXB xevd_tbl_itxb[MAX_TR_LOG2];
void xevd_itdq(XEVD_CTX * ctx, s16 *coef, int log2_w, int log2_h, int scale, int bit_depth);
void xevd_sub_block_itdq(XEVD_CTX * ctx, s16 coef[N_C][MAX_CU_DIM], int log2_cuw, int log2_cuh, u8 qp_y, u8 qp_u, u8 qp_v
                       , int flag[N_C], int nnz_sub[N_C][MAX_SUB_TB_NUM], int bit_depth, int chroma_format_idc);
void xevd_dquant(s16 *coef, int log2_w, int log2_h, int scale, s32 offset, u8 shift);
void xevd_itx_pb2b(void* src, void* dst, int shift, int line, int step);
void xevd_itx_pb4b(void* src, void* dst, int shift, int line, int step);
void xevd_itx_pb8b(void* src, void* dst, int shift, int line, int step);
void xevd_itx_pb16b(void* src, void* dst, int shift, int line, int step);
void xevd_itx_pb32b(void* src, void* dst, int shift, int line, int step);
void xevd_itx_pb64b(void* src, void* dst, int shift, int line, int step);
#endif /* _XEVD_ITDQ_H_ */
