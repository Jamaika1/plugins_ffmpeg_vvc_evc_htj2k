/*
Copyright (c) 2003-2004, Mark Borgerding
Lots of modifications by Jean-Marc Valin
Copyright (c) 2005-2007, Xiph.Org Foundation
Copyright (c) 2008,      Xiph.Org Foundation, CSIRO

All rights reserved.

Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:

    * Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.
    * Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

#ifndef SKIP_CONFIG_H
#  ifdef HAVE_CONFIG_H
#    include "config.h"
#  endif
#endif

#include "_kiss_fft_guts.h"
#include "arch.h"
#include "os_support.h"
#include "mathops.h"
#include "stack_alloc.h"

/* The guts header contains all the multiplication and addition macros that are defined for
   complex numbers.  It also delares the kf_ internal functions.
*/

static void kf_bfly2(
                     celt_kiss_fft_cpx * Fout,
                     const size_t fstride,
                     const celt_kiss_fft_state *st,
                     int m,
                     int N,
                     int mm
                    )
{
   celt_kiss_fft_cpx * Fout2;
   const kiss_twiddle_cpx * tw1;
   int i,j;
   celt_kiss_fft_cpx * Fout_beg = Fout;
   for (i=0;i<N;i++)
   {
      Fout = Fout_beg + i*mm;
      Fout2 = Fout + m;
      tw1 = st->twiddles;
      for(j=0;j<m;j++)
      {
         celt_kiss_fft_cpx t;
         Fout->r = SHR(Fout->r, 1);Fout->i = SHR(Fout->i, 1);
         Fout2->r = SHR(Fout2->r, 1);Fout2->i = SHR(Fout2->i, 1);
         C_MUL (t,  *Fout2 , *tw1);
         tw1 += fstride;
         C_SUB( *Fout2 ,  *Fout , t );
         C_ADDTO( *Fout ,  t );
         ++Fout2;
         ++Fout;
      }
   }
}

static void ki_bfly2(
                     celt_kiss_fft_cpx * Fout,
                     const size_t fstride,
                     const celt_kiss_fft_state *st,
                     int m,
                     int N,
                     int mm
                    )
{
   celt_kiss_fft_cpx * Fout2;
   const kiss_twiddle_cpx * tw1;
   celt_kiss_fft_cpx t;
   int i,j;
   celt_kiss_fft_cpx * Fout_beg = Fout;
   for (i=0;i<N;i++)
   {
      Fout = Fout_beg + i*mm;
      Fout2 = Fout + m;
      tw1 = st->twiddles;
      for(j=0;j<m;j++)
      {
         C_MULC (t,  *Fout2 , *tw1);
         tw1 += fstride;
         C_SUB( *Fout2 ,  *Fout , t );
         C_ADDTO( *Fout ,  t );
         ++Fout2;
         ++Fout;
      }
   }
}

static void kf_bfly4(
                     celt_kiss_fft_cpx * Fout,
                     const size_t fstride,
                     const celt_kiss_fft_state *st,
                     int m,
                     int N,
                     int mm
                    )
{
   const kiss_twiddle_cpx *tw1,*tw2,*tw3;
   celt_kiss_fft_cpx scratch[6];
   const size_t m2=2*m;
   const size_t m3=3*m;
   int i, j;

   celt_kiss_fft_cpx * Fout_beg = Fout;
   for (i=0;i<N;i++)
   {
      Fout = Fout_beg + i*mm;
      tw3 = tw2 = tw1 = st->twiddles;
      for (j=0;j<m;j++)
      {
         C_MUL4(scratch[0],Fout[m] , *tw1 );
         C_MUL4(scratch[1],Fout[m2] , *tw2 );
         C_MUL4(scratch[2],Fout[m3] , *tw3 );

         Fout->r = PSHR(Fout->r, 2);
         Fout->i = PSHR(Fout->i, 2);
         C_SUB( scratch[5] , *Fout, scratch[1] );
         C_ADDTO(*Fout, scratch[1]);
         C_ADD( scratch[3] , scratch[0] , scratch[2] );
         C_SUB( scratch[4] , scratch[0] , scratch[2] );
         Fout[m2].r = PSHR(Fout[m2].r, 2);
         Fout[m2].i = PSHR(Fout[m2].i, 2);
         C_SUB( Fout[m2], *Fout, scratch[3] );
         tw1 += fstride;
         tw2 += fstride*2;
         tw3 += fstride*3;
         C_ADDTO( *Fout , scratch[3] );

         Fout[m].r = scratch[5].r + scratch[4].i;
         Fout[m].i = scratch[5].i - scratch[4].r;
         Fout[m3].r = scratch[5].r - scratch[4].i;
         Fout[m3].i = scratch[5].i + scratch[4].r;
         ++Fout;
      }
   }
}

static void ki_bfly4(
                     celt_kiss_fft_cpx * Fout,
                     const size_t fstride,
                     const celt_kiss_fft_state *st,
                     int m,
                     int N,
                     int mm
                    )
{
   const kiss_twiddle_cpx *tw1,*tw2,*tw3;
   celt_kiss_fft_cpx scratch[6];
   const size_t m2=2*m;
   const size_t m3=3*m;
   int i, j;

   celt_kiss_fft_cpx * Fout_beg = Fout;
   for (i=0;i<N;i++)
   {
      Fout = Fout_beg + i*mm;
      tw3 = tw2 = tw1 = st->twiddles;
      for (j=0;j<m;j++)
      {
         C_MULC(scratch[0],Fout[m] , *tw1 );
         C_MULC(scratch[1],Fout[m2] , *tw2 );
         C_MULC(scratch[2],Fout[m3] , *tw3 );

         C_SUB( scratch[5] , *Fout, scratch[1] );
         C_ADDTO(*Fout, scratch[1]);
         C_ADD( scratch[3] , scratch[0] , scratch[2] );
         C_SUB( scratch[4] , scratch[0] , scratch[2] );
         C_SUB( Fout[m2], *Fout, scratch[3] );
         tw1 += fstride;
         tw2 += fstride*2;
         tw3 += fstride*3;
         C_ADDTO( *Fout , scratch[3] );

         Fout[m].r = scratch[5].r - scratch[4].i;
         Fout[m].i = scratch[5].i + scratch[4].r;
         Fout[m3].r = scratch[5].r + scratch[4].i;
         Fout[m3].i = scratch[5].i - scratch[4].r;
         ++Fout;
      }
   }
}

#ifndef RADIX_TWO_ONLY

static void kf_bfly3(
                     celt_kiss_fft_cpx * Fout,
                     const size_t fstride,
                     const celt_kiss_fft_state *st,
                     int m,
                     int N,
                     int mm
                    )
{
   int i;
   size_t k;
   const size_t m2 = 2*m;
   const kiss_twiddle_cpx *tw1,*tw2;
   celt_kiss_fft_cpx scratch[5];
   kiss_twiddle_cpx epi3;

   celt_kiss_fft_cpx * Fout_beg = Fout;
   epi3 = st->twiddles[fstride*m];
   for (i=0;i<N;i++)
   {
      Fout = Fout_beg + i*mm;
      tw1=tw2=st->twiddles;
      k=m;
      do {
         C_FIXDIV(*Fout,3); C_FIXDIV(Fout[m],3); C_FIXDIV(Fout[m2],3);

         C_MUL(scratch[1],Fout[m] , *tw1);
         C_MUL(scratch[2],Fout[m2] , *tw2);

         C_ADD(scratch[3],scratch[1],scratch[2]);
         C_SUB(scratch[0],scratch[1],scratch[2]);
         tw1 += fstride;
         tw2 += fstride*2;

         Fout[m].r = Fout->r - HALF_OF(scratch[3].r);
         Fout[m].i = Fout->i - HALF_OF(scratch[3].i);

         C_MULBYSCALAR( scratch[0] , epi3.i );

         C_ADDTO(*Fout,scratch[3]);

         Fout[m2].r = Fout[m].r + scratch[0].i;
         Fout[m2].i = Fout[m].i - scratch[0].r;

         Fout[m].r -= scratch[0].i;
         Fout[m].i += scratch[0].r;

         ++Fout;
      } while(--k);
   }
}

static void ki_bfly3(
                     celt_kiss_fft_cpx * Fout,
                     const size_t fstride,
                     const celt_kiss_fft_state *st,
                     size_t m,
                     int N,
                     int mm
                    )
{
   size_t i, k;
   const size_t m2 = 2*m;
   const kiss_twiddle_cpx *tw1,*tw2;
   celt_kiss_fft_cpx scratch[5];
   kiss_twiddle_cpx epi3;

   celt_kiss_fft_cpx * Fout_beg = Fout;
   epi3 = st->twiddles[fstride*m];
   for (i=0;i<N;i++)
   {
      Fout = Fout_beg + i*mm;
      tw1=tw2=st->twiddles;
      k=m;
      do{

         C_MULC(scratch[1],Fout[m] , *tw1);
         C_MULC(scratch[2],Fout[m2] , *tw2);

         C_ADD(scratch[3],scratch[1],scratch[2]);
         C_SUB(scratch[0],scratch[1],scratch[2]);
         tw1 += fstride;
         tw2 += fstride*2;

         Fout[m].r = Fout->r - HALF_OF(scratch[3].r);
         Fout[m].i = Fout->i - HALF_OF(scratch[3].i);

         C_MULBYSCALAR( scratch[0] , -epi3.i );

         C_ADDTO(*Fout,scratch[3]);

         Fout[m2].r = Fout[m].r + scratch[0].i;
         Fout[m2].i = Fout[m].i - scratch[0].r;

         Fout[m].r -= scratch[0].i;
         Fout[m].i += scratch[0].r;

         ++Fout;
      }while(--k);
   }
}


static void kf_bfly5(
                     celt_kiss_fft_cpx * Fout,
                     const size_t fstride,
                     const celt_kiss_fft_state *st,
                     int m,
                     int N,
                     int mm
                    )
{
   celt_kiss_fft_cpx *Fout0,*Fout1,*Fout2,*Fout3,*Fout4;
   int i, u;
   celt_kiss_fft_cpx scratch[13];
   const kiss_twiddle_cpx * twiddles = st->twiddles;
   const kiss_twiddle_cpx *tw;
   kiss_twiddle_cpx ya,yb;
   celt_kiss_fft_cpx * Fout_beg = Fout;

   ya = twiddles[fstride*m];
   yb = twiddles[fstride*2*m];
   tw=st->twiddles;

   for (i=0;i<N;i++)
   {
      Fout = Fout_beg + i*mm;
      Fout0=Fout;
      Fout1=Fout0+m;
      Fout2=Fout0+2*m;
      Fout3=Fout0+3*m;
      Fout4=Fout0+4*m;

      for ( u=0; u<m; ++u ) {
         C_FIXDIV( *Fout0,5); C_FIXDIV( *Fout1,5); C_FIXDIV( *Fout2,5); C_FIXDIV( *Fout3,5); C_FIXDIV( *Fout4,5);
         scratch[0] = *Fout0;

         C_MUL(scratch[1] ,*Fout1, tw[u*fstride]);
         C_MUL(scratch[2] ,*Fout2, tw[2*u*fstride]);
         C_MUL(scratch[3] ,*Fout3, tw[3*u*fstride]);
         C_MUL(scratch[4] ,*Fout4, tw[4*u*fstride]);

         C_ADD( scratch[7],scratch[1],scratch[4]);
         C_SUB( scratch[10],scratch[1],scratch[4]);
         C_ADD( scratch[8],scratch[2],scratch[3]);
         C_SUB( scratch[9],scratch[2],scratch[3]);

         Fout0->r += scratch[7].r + scratch[8].r;
         Fout0->i += scratch[7].i + scratch[8].i;

         scratch[5].r = scratch[0].r + S_MUL(scratch[7].r,ya.r) + S_MUL(scratch[8].r,yb.r);
         scratch[5].i = scratch[0].i + S_MUL(scratch[7].i,ya.r) + S_MUL(scratch[8].i,yb.r);

         scratch[6].r =  S_MUL(scratch[10].i,ya.i) + S_MUL(scratch[9].i,yb.i);
         scratch[6].i = -S_MUL(scratch[10].r,ya.i) - S_MUL(scratch[9].r,yb.i);

         C_SUB(*Fout1,scratch[5],scratch[6]);
         C_ADD(*Fout4,scratch[5],scratch[6]);

         scratch[11].r = scratch[0].r + S_MUL(scratch[7].r,yb.r) + S_MUL(scratch[8].r,ya.r);
         scratch[11].i = scratch[0].i + S_MUL(scratch[7].i,yb.r) + S_MUL(scratch[8].i,ya.r);
         scratch[12].r = - S_MUL(scratch[10].i,yb.i) + S_MUL(scratch[9].i,ya.i);
         scratch[12].i = S_MUL(scratch[10].r,yb.i) - S_MUL(scratch[9].r,ya.i);

         C_ADD(*Fout2,scratch[11],scratch[12]);
         C_SUB(*Fout3,scratch[11],scratch[12]);

         ++Fout0;++Fout1;++Fout2;++Fout3;++Fout4;
      }
   }
}

static void ki_bfly5(
                     celt_kiss_fft_cpx * Fout,
                     const size_t fstride,
                     const celt_kiss_fft_state *st,
                     int m,
                     int N,
                     int mm
                    )
{
   celt_kiss_fft_cpx *Fout0,*Fout1,*Fout2,*Fout3,*Fout4;
   int i, u;
   celt_kiss_fft_cpx scratch[13];
   const kiss_twiddle_cpx * twiddles = st->twiddles;
   const kiss_twiddle_cpx *tw;
   kiss_twiddle_cpx ya,yb;
   celt_kiss_fft_cpx * Fout_beg = Fout;

   ya = twiddles[fstride*m];
   yb = twiddles[fstride*2*m];
   tw=st->twiddles;

   for (i=0;i<N;i++)
   {
      Fout = Fout_beg + i*mm;
      Fout0=Fout;
      Fout1=Fout0+m;
      Fout2=Fout0+2*m;
      Fout3=Fout0+3*m;
      Fout4=Fout0+4*m;

      for ( u=0; u<m; ++u ) {
         scratch[0] = *Fout0;

         C_MULC(scratch[1] ,*Fout1, tw[u*fstride]);
         C_MULC(scratch[2] ,*Fout2, tw[2*u*fstride]);
         C_MULC(scratch[3] ,*Fout3, tw[3*u*fstride]);
         C_MULC(scratch[4] ,*Fout4, tw[4*u*fstride]);

         C_ADD( scratch[7],scratch[1],scratch[4]);
         C_SUB( scratch[10],scratch[1],scratch[4]);
         C_ADD( scratch[8],scratch[2],scratch[3]);
         C_SUB( scratch[9],scratch[2],scratch[3]);

         Fout0->r += scratch[7].r + scratch[8].r;
         Fout0->i += scratch[7].i + scratch[8].i;

         scratch[5].r = scratch[0].r + S_MUL(scratch[7].r,ya.r) + S_MUL(scratch[8].r,yb.r);
         scratch[5].i = scratch[0].i + S_MUL(scratch[7].i,ya.r) + S_MUL(scratch[8].i,yb.r);

         scratch[6].r = -S_MUL(scratch[10].i,ya.i) - S_MUL(scratch[9].i,yb.i);
         scratch[6].i =  S_MUL(scratch[10].r,ya.i) + S_MUL(scratch[9].r,yb.i);

         C_SUB(*Fout1,scratch[5],scratch[6]);
         C_ADD(*Fout4,scratch[5],scratch[6]);

         scratch[11].r = scratch[0].r + S_MUL(scratch[7].r,yb.r) + S_MUL(scratch[8].r,ya.r);
         scratch[11].i = scratch[0].i + S_MUL(scratch[7].i,yb.r) + S_MUL(scratch[8].i,ya.r);
         scratch[12].r =  S_MUL(scratch[10].i,yb.i) - S_MUL(scratch[9].i,ya.i);
         scratch[12].i = -S_MUL(scratch[10].r,yb.i) + S_MUL(scratch[9].r,ya.i);

         C_ADD(*Fout2,scratch[11],scratch[12]);
         C_SUB(*Fout3,scratch[11],scratch[12]);

         ++Fout0;++Fout1;++Fout2;++Fout3;++Fout4;
      }
   }
}

#endif

static void kf_work(
        celt_kiss_fft_cpx * Fout,
        const celt_kiss_fft_cpx * f,
        size_t fstride,
        int in_stride,
        const celt_int16 * factors,
        const celt_kiss_fft_state *st,
        int N,
        int m2
        )
{
    const int p=*factors++; /* the radix  */
    const int m=*factors++; /* stage's fft length/p */
    /*printf ("fft %d %d %d %d %d %d %d\n", p*m, m, p, s2, fstride*in_stride, N, m2);*/
    if (m!=1)
        kf_work( Fout , f, fstride*p, in_stride, factors,st, N*p, m);

    /* Compensate for longer twiddles table (when sharing) */
    if (st->shift>0)
       fstride <<= st->shift;
    switch (p) {
        case 2: kf_bfly2(Fout,fstride,st,m, N, m2); break;
        case 4: kf_bfly4(Fout,fstride,st,m, N, m2); break;
#ifndef RADIX_TWO_ONLY
        case 3: kf_bfly3(Fout,fstride,st,m, N, m2); break;
        case 5: kf_bfly5(Fout,fstride,st,m, N, m2); break;
#endif
    }
}


static void ki_work(
             celt_kiss_fft_cpx * Fout,
             const celt_kiss_fft_cpx * f,
             size_t fstride,
             int in_stride,
             const celt_int16 * factors,
             const celt_kiss_fft_state *st,
             int N,
             int m2
            )
{
   const int p=*factors++; /* the radix  */
   const int m=*factors++; /* stage's fft length/p */
   /*printf ("fft %d %d %d %d %d %d %d\n", p*m, m, p, s2, fstride*in_stride, N, m2);*/
   if (m!=1)
      ki_work( Fout , f, fstride*p, in_stride, factors,st, N*p, m);

   /* Compensate for longer twiddles table (when sharing) */
   if (st->shift>0)
      fstride <<= st->shift;
   switch (p) {
      case 2: ki_bfly2(Fout,fstride,st,m, N, m2); break;
      case 4: ki_bfly4(Fout,fstride,st,m, N, m2); break;
#ifndef RADIX_TWO_ONLY
      case 3: ki_bfly3(Fout,fstride,st,m, N, m2); break;
      case 5: ki_bfly5(Fout,fstride,st,m, N, m2); break;
#endif
   }
}


#ifdef CUSTOM_MODES

static
void compute_bitrev_table(
         int Fout,
         celt_int16 *f,
         const size_t fstride,
         int in_stride,
         celt_int16 * factors,
         const celt_kiss_fft_state *st
            )
{
   const int p=*factors++; /* the radix  */
   const int m=*factors++; /* stage's fft length/p */

    /*printf ("fft %d %d %d %d %d %d\n", p*m, m, p, s2, fstride*in_stride, N);*/
   if (m==1)
   {
      int j;
      for (j=0;j<p;j++)
      {
         *f = Fout+j;
         f += fstride*in_stride;
      }
   } else {
      int j;
      for (j=0;j<p;j++)
      {
         compute_bitrev_table( Fout , f, fstride*p, in_stride, factors,st);
         f += fstride*in_stride;
         Fout += m;
      }
   }
}


/*  facbuf is populated by p1,m1,p2,m2, ...
    where
    p[i] * m[i] = m[i-1]
    m0 = n                  */
static
int kf_factor(int n,celt_int16 * facbuf)
{
    int p=4;

    /*factor out powers of 4, powers of 2, then any remaining primes */
    do {
        while (n % p) {
            switch (p) {
                case 4: p = 2; break;
                case 2: p = 3; break;
                default: p += 2; break;
            }
            if (p>32000 || (celt_int32)p*(celt_int32)p > n)
                p = n;          /* no more factors, skip to end */
        }
        n /= p;
#ifdef RADIX_TWO_ONLY
        if (p!=2 && p != 4)
#else
        if (p>5)
#endif
        {
           return 0;
        }
        *facbuf++ = p;
        *facbuf++ = n;
    } while (n > 1);
    return 1;
}

static void compute_twiddles(kiss_twiddle_cpx *twiddles, int nfft)
{
   int i;
#ifdef FIXED_POINT
   for (i=0;i<nfft;++i) {
      celt_word32 phase = -i;
      kf_cexp2(twiddles+i, DIV32(SHL32(phase,17),nfft));
   }
#else
   for (i=0;i<nfft;++i) {
      const double pi=3.14159265358979323846264338327;
      double phase = ( -2*pi /nfft ) * i;
      kf_cexp(twiddles+i, phase );
   }
#endif
}


/*
 *
 * User-callable function to allocate all necessary storage space for the fft.
 *
 * The return value is a contiguous block of memory, allocated with malloc.  As such,
 * It can be freed with free(), rather than a celt_kiss_fft-specific function.
 * */
celt_kiss_fft_state *celt_fft_alloc_twiddles(int nfft,void * mem,size_t * lenmem,  const celt_kiss_fft_state *base)
{
    celt_kiss_fft_state *st=NULL;
    size_t memneeded = sizeof(struct celt_kiss_fft_state); /* twiddle factors*/

    if ( lenmem==NULL ) {
        st = ( celt_kiss_fft_state*)KISS_FFT_MALLOC( memneeded );
    }else{
        if (mem != NULL && *lenmem >= memneeded)
            st = (celt_kiss_fft_state*)mem;
        *lenmem = memneeded;
    }
    if (st) {
        celt_int16 *bitrev;
        kiss_twiddle_cpx *twiddles;

        st->nfft=nfft;
#ifndef FIXED_POINT
        st->scale = 1./nfft;
#endif
        if (base != NULL)
        {
           st->twiddles = base->twiddles;
           st->shift = 0;
           while (nfft<<st->shift != base->nfft && st->shift < 32)
              st->shift++;
           /* FIXME: Report error and do proper cleanup */
           if (st->shift>=32)
              return NULL;
        } else {
           st->twiddles = twiddles = (kiss_twiddle_cpx*)KISS_FFT_MALLOC(sizeof(kiss_twiddle_cpx)*nfft);
           compute_twiddles(twiddles, nfft);
           st->shift = -1;
        }
        if (!kf_factor(nfft,st->factors))
        {
           celt_fft_free(st);
           return NULL;
        }

        /* bitrev */
        st->bitrev = bitrev = (celt_int16*)KISS_FFT_MALLOC(sizeof(celt_int16)*nfft);
        compute_bitrev_table(0, bitrev, 1,1, st->factors,st);
    }
    return st;
}

celt_kiss_fft_state *celt_fft_alloc(int nfft,void * mem,size_t * lenmem )
{
   return celt_fft_alloc_twiddles(nfft, mem, lenmem, NULL);
}

void celt_fft_free(const celt_kiss_fft_state *cfg)
{
   if (cfg)
   {
      celt_free((celt_int16*)cfg->bitrev);
      if (cfg->shift < 0)
         celt_free((kiss_twiddle_cpx*)cfg->twiddles);
      celt_free((celt_kiss_fft_state*)cfg);
   }
}

#endif /* CUSTOM_MODES */

static void celt_kiss_fft_stride(const celt_kiss_fft_state *st,const celt_kiss_fft_cpx *fin,celt_kiss_fft_cpx *fout,int in_stride)
{
    int i;
    celt_assert2 (fin != fout, "In-place FFT not supported");
    /* Bit-reverse the input */
    for (i=0;i<st->nfft;i++)
    {
       fout[st->bitrev[i]] = fin[i];
#ifndef FIXED_POINT
       fout[st->bitrev[i]].r *= st->scale;
       fout[st->bitrev[i]].i *= st->scale;
#endif
    }
    kf_work( fout, fin, 1,in_stride, st->factors,st, 1, 1);
}

void celt_fft(const celt_kiss_fft_state *cfg,const celt_kiss_fft_cpx *fin,celt_kiss_fft_cpx *fout)
{
    celt_kiss_fft_stride(cfg,fin,fout,1);
}

static void kiss_ifft_stride(const celt_kiss_fft_state *st,const celt_kiss_fft_cpx *fin,celt_kiss_fft_cpx *fout,int in_stride)
{
   int i;
   celt_assert2 (fin != fout, "In-place FFT not supported");
   /* Bit-reverse the input */
   for (i=0;i<st->nfft;i++)
      fout[st->bitrev[i]] = fin[i];
   ki_work( fout, fin, 1,in_stride, st->factors,st, 1, 1);
}

void celt_ifft(const celt_kiss_fft_state *cfg,const celt_kiss_fft_cpx *fin,celt_kiss_fft_cpx *fout)
{
   kiss_ifft_stride(cfg,fin,fout,1);
}

