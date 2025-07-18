/* -----------------------------------------------------------------------------
The copyright in this software is being made available under the Clear BSD
License, included below. No patent rights, trademark rights and/or
other Intellectual Property Rights other than the copyrights concerning
the Software are granted under this license.

The Clear BSD License

Copyright (c) 2019-2024, Fraunhofer-Gesellschaft zur Förderung der angewandten Forschung e.V. & The VVenC Authors.
All rights reserved.

Redistribution and use in source and binary forms, with or without modification,
are permitted (subject to the limitations in the disclaimer below) provided that
the following conditions are met:

     * Redistributions of source code must retain the above copyright notice,
     this list of conditions and the following disclaimer.

     * Redistributions in binary form must reproduce the above copyright
     notice, this list of conditions and the following disclaimer in the
     documentation and/or other materials provided with the distribution.

     * Neither the name of the copyright holder nor the names of its
     contributors may be used to endorse or promote products derived from this
     software without specific prior written permission.

NO EXPRESS OR IMPLIED LICENSES TO ANY PARTY'S PATENT RIGHTS ARE GRANTED BY
THIS LICENSE. THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND
CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A
PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR
CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR
BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER
IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
POSSIBILITY OF SUCH DAMAGE.


------------------------------------------------------------------------------------------- */


/** \file     EncPicture.cpp
    \brief    encode picture
*/

#include "EncPicture.h"
#include "EncGOP.h"
#include "../CommonLib/UnitTools.h"
#include "../CommonLib/CommonDef.h"
#include "../CommonLib/dtrace_buffer.h"
#include "../CommonLib/dtrace_codingstruct.h"

//! \ingroup EncoderLib
//! \{

namespace vvenc {

#ifdef TRACE_ENABLE_ITT
static __itt_string_handle* itt_handle_start = __itt_string_handle_create( "PicEnc" );
static __itt_domain* itt_domain_picEncoder   = __itt_domain_create( "PictureEncoder" );
#endif

// ---------------------------------------------------------------------------------------------------------------------

void EncPicture::init( const VVEncCfg& encCfg,
                       std::vector<int>* const globalCtuQpVector,
                       const SPS& sps,
                       const PPS& pps,
                       RateCtrl& rateCtrl,
                       NoMallocThreadPool* threadPool )
{
  m_pcEncCfg = &encCfg;

  if( encCfg.m_alf || encCfg.m_ccalf )
    m_ALF       .init( encCfg, pps, m_CABACEstimator, m_CtxCache, threadPool );

  m_SliceEncoder.init( encCfg, sps, pps, globalCtuQpVector, m_LoopFilter, m_ALF, rateCtrl, threadPool, &m_ctuTasksDoneCounter );
  m_pcRateCtrl = &rateCtrl;
}


void EncPicture::compressPicture( Picture& pic, EncGOP& gopEncoder )
{
  PROFILER_SCOPE_TOP_LEVEL_EXT( 1, g_timeProfiler, P_TOP_LEVEL, pic.cs );
  ITT_TASKSTART( itt_domain_picEncoder, itt_handle_start );

  pic.encTime.startTimer();

  pic.createTempBuffers( pic.cs->pcv->maxCUSize );
  pic.cs->createCoeffs();
  pic.cs->createTempBuffers( true );
  pic.cs->initStructData( MAX_INT, false, nullptr );

  if( pic.useLMCS && m_pcEncCfg->m_reshapeSignalType == RESHAPE_SIGNAL_PQ && m_pcEncCfg->m_alf )
  {
    const double *weights = gopEncoder.getReshaper().getlumaLevelToWeightPLUT();
    auto& vec = m_ALF.getLumaLevelWeightTable();
    const size_t numEl = size_t( 1 ) << m_pcEncCfg->m_internalBitDepth[0];

    vec.resize( numEl );
    std::copy( weights, weights + numEl, vec.begin() );

    m_ALF.setAlfWSSD( 1 );
  }
  else
  {
    m_ALF.setAlfWSSD( 0 );
  }

  // compress picture
  xInitPicEncoder( pic );

  // compress current slice
  pic.cs->slice = pic.slices[0];
  std::fill( pic.ctuSlice.begin(), pic.ctuSlice.end(), pic.slices[0] );
  m_SliceEncoder.compressSlice( &pic );

  ITT_TASKEND( itt_domain_picEncoder, itt_handle_start );
}

void EncPicture::finalizePicture( Picture& pic )
{
  PROFILER_SCOPE_TOP_LEVEL_EXT( 1, g_timeProfiler, P_TOP_LEVEL, pic.cs );
  CodingStructure& cs = *(pic.cs);
  Slice* slice        = pic.slices[0];
  // ALF
  if( slice->sps->alfEnabled )
  {
#ifdef TRACE_ENABLE_ITT
    std::stringstream ss;
    ss << "ALF_post_" << slice->poc;
    __itt_string_handle* itt_handle_post = __itt_string_handle_create( ss.str().c_str() );
#endif
    pic.picApsMap.setApsIdStart( m_ALF.getApsIdStart() );

    cs.slice->ccAlfFilterParam      = m_ALF.getCcAlfFilterParam();
    cs.slice->ccAlfFilterControl[0] = m_ALF.getCcAlfControlIdc(COMP_Cb);
    cs.slice->ccAlfFilterControl[1] = m_ALF.getCcAlfControlIdc(COMP_Cr);

    DTRACE( g_trace_ctx, D_CRC, "ALF" );
    DTRACE_CRC( g_trace_ctx, D_CRC, cs, cs.getRecoBuf() );
    DTRACE_PIC_COMP( D_REC_CB_LUMA_ALF,   cs, cs.getRecoBuf(), COMP_Y  );
    DTRACE_PIC_COMP( D_REC_CB_CHROMA_ALF, cs, cs.getRecoBuf(), COMP_Cb );
    DTRACE_PIC_COMP( D_REC_CB_CHROMA_ALF, cs, cs.getRecoBuf(), COMP_Cr );
  }

  // write picture
  DTRACE_UPDATE( g_trace_ctx, std::make_pair( "bsfinal", 1 ) );
  xWriteSliceData( pic );
  DTRACE_UPDATE( g_trace_ctx, std::make_pair( "bsfinal", 0 ) );

  xCalcDistortion( pic, *slice->sps );

  // finalize
  if ( m_pcEncCfg->m_useAMaxBT )
  {
    pic.picBlkStat.storeBlkSize( pic );
  }

  // copy ALF APSs to global list
  CHECK( m_pcEncCfg->m_alf && m_pcEncCfg->m_alfTempPred && !pic.picApsGlobal, "Missing APS from top level" );
  if( pic.picApsGlobal )
  {
    CHECK( pic.picApsGlobal->poc != pic.poc, "Global APS POC must be consistent with picture poc" );
    const ParameterSetMap<APS>& src = pic.picApsMap;
    ParameterSetMap<APS>&       dst = pic.picApsGlobal->apsMap;
    for( int i = 0; i < ALF_CTB_MAX_NUM_APS; i++ )
    {
      const int apsMapIdx = ( i << NUM_APS_TYPE_LEN ) + ALF_APS;
      const APS* srcAPS = src.getPS( apsMapIdx );
      if( srcAPS )
      {
        APS* dstAPS = dst.allocatePS( apsMapIdx );
        *dstAPS = *srcAPS;
      }
    }
    dst.setApsIdStart( src.getApsIdStart() );
  }

  // cleanup
  pic.cs->releaseIntermediateData();
  pic.cs->destroyTempBuffers();
  pic.cs->destroyCoeffs();
  pic.destroyTempBuffers();

  pic.encTime.stopTimer();
}

uint64_t findDistortionPlane( const CPelBuf& pic0, const CPelBuf& pic1, uint32_t rshift )
{
  uint64_t uiTotalDiff;
  const  Pel*  pSrc0 = pic0.bufAt(0, 0);
  const  Pel*  pSrc1 = pic1.bufAt(0, 0);

  CHECK(pic0.width  != pic1.width , "Unspecified error");
  CHECK(pic0.height != pic1.height, "Unspecified error");

  if( rshift > 0 )
  {
    uiTotalDiff = 0;
    for (int y = 0; y < pic0.height; y++)
    {
      for (int x = 0; x < pic0.width; x++)
      {
        Intermediate_Int iTemp = pSrc0[x] - pSrc1[x];
        uiTotalDiff += uint64_t((iTemp * iTemp) >> rshift);
      }
      pSrc0 += pic0.stride;
      pSrc1 += pic1.stride;
    }
  }
  else
  {
    uiTotalDiff = 0;
    for (int y = 0; y < pic0.height; y++)
    {
      for (int x = 0; x < pic0.width; x++)
      {
        Intermediate_Int iTemp = pSrc0[x] - pSrc1[x];
        uiTotalDiff += uint64_t(iTemp * iTemp);
      }
      pSrc0 += pic0.stride;
      pSrc1 += pic1.stride;
    }
  }

  return uiTotalDiff;
}

void EncPicture::xCalcDistortion( Picture& pic, const SPS& sps )
{
  const CPelUnitBuf cPicD = pic.getRecoBuf();
  const CPelUnitBuf org   = pic.getOrigBuf();
  const ChromaFormat formatD = cPicD.chromaFormat;
  const ChromaFormat format  = sps.chromaFormatIdc;

  for (int comp = 0; comp < getNumberValidComponents(formatD); comp++)
  {
    const ComponentID compID = ComponentID(comp);
    const CPelBuf&    p = cPicD.get(compID);
    const CPelBuf&    o = org.get(compID);

    CHECK(!( p.width  == o.width), "Unspecified error");
    CHECK(!( p.height == o.height), "Unspecified error");

    const uint32_t   width  = p.width  - (m_pcEncCfg->m_aiPad[ 0 ] >> getComponentScaleX(compID, format));
    const uint32_t   height = p.height - (m_pcEncCfg->m_aiPad[ 1 ] >> getComponentScaleY(compID, format));

    // create new buffers with correct dimensions
    const CPelBuf recPB(p.bufAt(0, 0), p.stride, width, height);
    const CPelBuf orgPB(o.bufAt(0, 0), o.stride, width, height);
    const uint32_t    bitDepth = sps.bitDepths[toChannelType(compID)];
    //pic.ssd[comp] = findDistortionPlane(recPB, orgPB, 0);
    const uint64_t uiSSDtemp = findDistortionPlane(recPB, orgPB, 0);
    const uint32_t maxval = 255 << (bitDepth - 8);
    const uint32_t size   = width * height;
    const double fRefValue = (double)maxval * maxval * size;
    pic.psnr[comp] = uiSSDtemp ? 10.0 * log10(fRefValue / (double)uiSSDtemp) : MAX_DOUBLE;
    pic.mse [comp] = (double)uiSSDtemp / size;
  }
}

void EncPicture::xInitPicEncoder( Picture& pic )
{
  Slice* slice = pic.cs->slice;

  CHECK( slice != pic.slices[0], "Slice pointers don't match!" );

  m_SliceEncoder.initPic( &pic );

  xInitSliceColFromL0Flag( slice );
  xInitSliceCheckLDC     ( slice );

  if( slice->sps->alfEnabled )
  {
    for (int s = 0; s < (int)pic.slices.size(); s++)
    {
      pic.slices[s]->alfEnabled[COMP_Y] = false;
    }
  }
}


void EncPicture::xInitSliceColFromL0Flag( Slice* slice ) const
{
  if( m_pcEncCfg->m_rprRASLtoolSwitch )
  {
    return;
  }

  if ( slice->sliceType == VVENC_B_SLICE )
  {
    const int refIdx = 0; // Zero always assumed
    const Picture* refPicL0 = slice->getRefPic( REF_PIC_LIST_0, refIdx );
    const Picture* refPicL1 = slice->getRefPic( REF_PIC_LIST_1, refIdx );
    slice->colFromL0Flag = ( refPicL0->slices[ 0 ]->sliceQp > refPicL1->slices[ 0 ]->sliceQp );
  }
}


void EncPicture::xInitSliceCheckLDC( Slice* slice ) const
{
  if ( slice->sliceType == VVENC_B_SLICE )
  {
    bool bLowDelay = true;
    int  iCurrPOC  = slice->poc;
    int  iRefIdx   = 0;

    for ( iRefIdx = 0; iRefIdx < slice->numRefIdx[ REF_PIC_LIST_0 ] && bLowDelay; iRefIdx++ )
    {
      if ( slice->getRefPic( REF_PIC_LIST_0, iRefIdx )->getPOC() > iCurrPOC )
      {
        bLowDelay = false;
      }
    }
    for ( iRefIdx = 0; iRefIdx < slice->numRefIdx[ REF_PIC_LIST_1 ] && bLowDelay; iRefIdx++ )
    {
      if ( slice->getRefPic( REF_PIC_LIST_1, iRefIdx )->getPOC() > iCurrPOC )
      {
        bLowDelay = false;
      }
    }

    slice->checkLDC = bLowDelay;
  }
  else
  {
    slice->checkLDC = true;
  }
}

void EncPicture::xWriteSliceData( Picture& pic )
{
  const int numSlices = (int)pic.slices.size();

  pic.sliceDataStreams.clear();
  pic.sliceDataStreams.resize( numSlices );
  pic.sliceDataNumBins = 0;

  for ( int i = 0; i < numSlices; i++ )
  {
    // set current slice
    pic.cs->slice = pic.slices[i];
    m_SliceEncoder.encodeSliceData( &pic );
  }
}

} // namespace vvenc

//! \}

