// Avisynth v2.5.  Copyright 2002 Ben Rudiak-Gould et al.
// http://avisynth.nl

// This program is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation; either version 2 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software
// Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA, or visit
// http://www.gnu.org/copyleft/gpl.html .
//
// Linking Avisynth statically or dynamically with other modules is making a
// combined work based on Avisynth.  Thus, the terms and conditions of the GNU
// General Public License cover the whole combination.
//
// As a special exception, the copyright holders of Avisynth give you
// permission to link Avisynth with independent modules that communicate with
// Avisynth solely through the interfaces defined in avisynth.h, regardless of the license
// terms of these independent modules, and to copy and distribute the
// resulting combined work under terms of your choice, provided that
// every copy of the combined work is accompanied by a complete copy of
// the source code of Avisynth (the version of Avisynth used to produce the
// combined work), being distributed under the terms of the GNU General
// Public License plus this exception.  An independent module is a module
// which is not derived from or based on Avisynth, such as 3rd-party filters,
// import and export plugins, or graphical user interfaces.

#include "resample_functions.h"
#include <cmath>
#include <vector>
#include <algorithm>
#include "avs/minmax.h"
#include "avs/alignment.h"
#if HAVE_RESAMPLEMT
#include "plugins/ResampleMT/MatrixClass.cc"
#endif


/*******************************************
   ***************************************
   **  Helper classes for resample.cpp  **
   ***************************************
 *******************************************/

/***************************
 ***** Point filter *****
 **************************/

double PointFilter::f(double x) {
  AVS_UNUSED(x);
  return 1.0;
}


/***************************
 ***** Triangle filter *****
 **************************/

double TriangleFilter::f(double x) {
  x = fabs(x);
  return (x<1.0) ? 1.0-x : 0.0;
}





/*********************************
 *** Mitchell-Netravali filter ***
 *********************************/

MitchellNetravaliFilter::MitchellNetravaliFilter (double b, double c) {
  p0 = (   6. -  2.*b            ) / 6.;
  p2 = ( -18. + 12.*b +  6.*c    ) / 6.;
  p3 = (  12. -  9.*b -  6.*c    ) / 6.;
  q0 = (            8.*b + 24.*c ) / 6.;
  q1 = (         - 12.*b - 48.*c ) / 6.;
  q2 = (            6.*b + 30.*c ) / 6.;
  q3 = (      -     b -  6.*c    ) / 6.;
}

double MitchellNetravaliFilter::f (double x) {
  x = fabs(x);
  return (x<1) ? (p0+x*x*(p2+x*p3)) : (x<2) ? (q0+x*(q1+x*(q2+x*q3))) : 0.0;
}


/***********************
 *** Lanczos3 filter ***
 ***********************/
LanczosFilter::LanczosFilter(int _taps) {
   taps = (double)clamp(_taps, 1, 100);
}

double LanczosFilter::sinc(double value) {
  if (value > 0.000001) {
    value *= M_PI;
    return sin(value) / value;
  } else {
    return 1.0;
  }
}

double LanczosFilter::f(double value) {
   value = fabs(value);

  if (value < taps) {
    return (sinc(value) * sinc(value / taps));
  } else {
    return 0.0;
  }
}


/***********************
 *** Blackman filter ***
 ***********************/
BlackmanFilter::BlackmanFilter(int _taps) {
   taps = (double)clamp(_taps, 1, 100);
   rtaps = 1.0/taps;
}

double BlackmanFilter::f(double value) {
   value = fabs(value);

  if (value < taps) {
    if (value > 0.000001) {
      value *= M_PI;
      return (sin(value) / value) * (0.42 + 0.5*cos(value*rtaps) + 0.08*cos(2*value*rtaps));
    } else {
      return 1.0;
    }
  } else {
    return 0.0;
  }
}


/***********************
 *** Spline16 filter ***
 ***********************/

double Spline16Filter::f(double value) {
  value = fabs(value);

  if (value < 1.0) {
    return ( ( value - 9.0/5.0 ) * value - 1.0/5.0 ) * value + 1.0;
  } else if (value < 2.0) {
    return ( ( -1.0/3.0 * (value-1.0) + 4.0/5.0 ) * (value-1.0) - 7.0/15.0 ) * (value-1.0);
  }
  return 0.0;
}

/***********************
 *** Spline36 filter ***
 ***********************/

double Spline36Filter::f(double value) {
  value = fabs(value);

  if        (value < 1.0) {
    return ( ( 13.0/11.0  * (value    ) - 453.0/ 209.0 ) * (value    ) -   3.0/ 209.0 ) *(value    ) + 1.0;
  } else if (value < 2.0) {
    return ( ( -6.0/11.0  * (value-1.0) + 270.0/ 209.0 ) * (value-1.0) - 156.0/ 209.0 ) *(value-1.0);
  } else if (value < 3.0) {
    return  ( ( 1.0/11.0  * (value-2.0) -  45.0/ 209.0 ) * (value-2.0) +  26.0/ 209.0 ) *(value-2.0);
  }
  return 0.0;
}

/***********************
 *** Spline64 filter ***
 ***********************/

double Spline64Filter::f(double value) {
  value = fabs(value);

  if        (value < 1.0) {
    return (( 49.0/41.0 * (value    ) - 6387.0/2911.0) * (value    ) -    3.0/2911.0) * (value    ) + 1.0;
  } else if (value < 2.0) {
    return ((-24.0/41.0 * (value-1.0) + 4032.0/2911.0) * (value-1.0) - 2328.0/2911.0) * (value-1.0);
  } else if (value < 3.0) {
    return ((  6.0/41.0 * (value-2.0) - 1008.0/2911.0) * (value-2.0) +  582.0/2911.0) * (value-2.0);
  } else if (value < 4.0) {
    return ((- 1.0/41.0 * (value-3.0) +  168.0/2911.0) * (value-3.0) -   97.0/2911.0) * (value-3.0);
  }
  return 0.0;
}

/***********************
 *** Gaussian filter ***
 ***********************/

/* Solve taps from p*value*value < 9 as pow(2.0, -9.0) == 1.0/512.0 i.e 0.5 bit
                     value*value < 9/p       p = param*0.1;
                     value*value < 90/param
                     value*value < 90/{0.1, 22.5, 30.0, 100.0}
                     value*value < {900, 4.0, 3.0, 0.9}
                     value       < {30, 2.0, 1.73, 0.949}         */

GaussianFilter::GaussianFilter(double p, double _b, double _s) {
  param = (double)clamp(p, 0.01, 100.0);
  b = (double)clamp(_b, 1.5, 3.5);
  s = _s;
  if (_s == 0) // auto-support signal
  {
    // get support from b and param for 0.01 of resudual kernel value
    // equatiion is s = sqrt(-ln(0.01)/(param*ln(b))
    // where ln(0.01) is about -4.6 and -ln(0.01) is 4.6
    s = sqrt(4.6 / ((param * 0.1) * log(b)));
  }
  s = (double)clamp(s, 0.1, 150.0);
}

double GaussianFilter::f(double value) {
  double p = param * 0.1;
  return pow(b, -p * value * value); // <3.7.4: b was fixed at 2.0
}

/***********************
 *** Sinc filter ***
 ***********************/
SincFilter::SincFilter(int _taps) {
   taps = (double)clamp(_taps, 1, 150);
}

double SincFilter::f(double value) {
  value = fabs(value);

  if (value > 0.000001) {
    value *= M_PI;
    return sin(value)/value;
  } else {
    return 1.0;
  }
}


/**********************
*** SinPower filter ***
***********************/

SinPowerFilter::SinPowerFilter(double p) {
  param = (double)clamp(p, 1.0, 10.0);
}

double SinPowerFilter::f(double value) {
  value = fabs(value);
  value *= M_PI / param;

  if (value < (M_PI / 2)) return pow(cos(value), 1.8);
  else
  {
    if (value < M_PI) return -(cos(value) * cos(value)) / (0.9 * value);
    else return 0;
  }
}

/***********************
*** SincLin2 filter ***
***********************/

SincLin2Filter::SincLin2Filter(int _taps)
{
  taps = (double)clamp(_taps, 1, 30);
}

double SincLin2Filter::sinc(double value)
{
  if (value > 0.000001)
  {
    value *= M_PI;
    return sin(value) / value;
  }
  else return 1.0;
}

double SincLin2Filter::f(double value)
{
  value = fabs(value);

  if (value < (taps / 2.0)) return sinc(value);
  else return sinc(value) * ((2.0 - (2.0 * value / taps)));

}


/*********************************
 *** UserDefined2 filter ***
 *********************************/

UserDefined2Filter::UserDefined2Filter(double _b, double _c, double _s)
{
  a = 1.0; // 0 sample = 1
  b = (double)clamp(_b, -50.0, 250.0); // 1 and -1  sample
  c = (double)clamp(_c, -50.0, 250.0); // 2 and -2 sample
  b = (b - 16.0) / 219.0;
  c = (c - 16.0) / 219.0;
  s = (double)clamp(_s, 1.5, 15.0); // filter support for resampler
}

double UserDefined2Filter::sinc(double value)
{

  if (fabs(value) > 0.000001)
  {
    value *= M_PI;
    return sin(value) / value;
  }
  else return 1.0;
}

double UserDefined2Filter::f(double x)
{
  x = fabs(x);

  return c * sinc(x + 2) + b * sinc(x + 1) + a * sinc(x) + b * sinc(x - 1) + c * sinc(x - 2);
}


/******************************
 **** Resampling Patterns  ****
 *****************************/

ResamplingProgram* ResamplingFunction::GetResamplingProgram(int source_size, double crop_start, double crop_size, int target_size, int bits_per_pixel,
  double center_pos_src, double center_pos_dst,
  IScriptEnvironment* env)
{
  // edge condition ideas from fmtconv, thanks.
  double src_step = crop_size / double(target_size); // Distance between source pixels for adjacent dest pixels
  double zc_size = std::max(src_step, 1.0) / 1.0;    // Size of filter unit step (kernel_scale=1.0 in our case)
  double imp_step = 1.0 / zc_size;                   // Corresponding distance in the impulse
  double filter_support = support() * zc_size;       // Number of source pixels covered by the FIR

  int fir_filter_size = std::max(int(std::ceil(filter_support * 2)), 1);
  int max_kernel_size = 0;

  const int last_line = source_size - 1;

  ResamplingProgram* program = new ResamplingProgram(fir_filter_size, source_size, target_size, crop_start, crop_size, bits_per_pixel, env);

/*#if HAVE_RESAMPLEMT
  if (!program->StatusOk)
  {
	  delete program;
	  return NULL;
  }
#endif*/

  // Initial position calculation

  double pos = crop_start;

  /*
  pre 3.7.4 logic:

  Now in 2025, let's fact-check this comment.

    pos = crop_start + ((crop_size - target_size) / (target_size*2)); // TODO this look wrong, gotta check
    ==>
    pos = crop_start + 1/2 * (crop_size / target_size - 1)
    ==>
    pos = crop_start + src_step * 0.5 - 1 * 0.5

    fmtconv generic formula:

    pos = crop_start + src_step * center_pos_dst - 1 * center_pos_src; // 3.7.4- fmtconv

    Solved: center_pos_dst = 0.5, center_pos_src = 0.5 in old Avisynth

  */

  // Introduces an offset because samples are located at the center of the
  // pixels, not on their boundaries. Excepted for pointresize.
  if (filter_support > 0)
  {
    // Pre 3.7.4 Avisynth worked with fixed center_pos_dst = center_pos_src = 0.5
    // Now it's externally configurable. In our use case they are always the same.
    pos += src_step * center_pos_dst - 1 * center_pos_src;
  }
  else
  {
    // In case of PointResize(), which now returns real 0 for support().
    // Avisynth heritage.
    filter_support = 0.0001;
  }

  const int current_FPScale = (bits_per_pixel > 8 && bits_per_pixel <= 16) ? FPScale16 : FPScale;

  std::vector<double> coef_tmp;
  for (int i = 0; i < target_size; ++i) {
    coef_tmp.clear();

    int start_pos = (int)(pos + filter_support) - fir_filter_size + 1;
    program->pixel_offset[i] = clamp(start_pos, 0, last_line);

    // First pass: Accumulate all coefficients for weighting
    double total = 0.0;
    for (int k = 0; k < fir_filter_size; ++k) {
      const int p = start_pos + k;
      double val = f((pos - p) * imp_step);
      coef_tmp.push_back(val);
      total += val;
    }

    if (total == 0.0) {
      // Shouldn't happen for valid positions.
      total = 1.0;
    }

    const int coeff_arr_base_index = i * fir_filter_size;

    // Second pass: Generate real coefficients, handling edge conditions
    double accu = 0.0;
    double prev_value = 0.0;

    int kernel_size = 0;

    if (bits_per_pixel == 32) {
      // Float version
      for (int k = 0; k < fir_filter_size; ++k) {
        const int p = start_pos + k;
        double val = coef_tmp[k];
        accu += val;
        if (p >= 0 && p <= last_line) {
          program->pixel_coefficient_float[coeff_arr_base_index + kernel_size] = float(accu / total);
          ++kernel_size;
          accu = 0;
        }
      }
    }
    else {
      // Integer version - using upscaled integer arithmetic (FPScale/FPScale16)
      for (int k = 0; k < fir_filter_size; ++k) {
        const int p = start_pos + k;
        double val = coef_tmp[k];
        accu += val;
        if (p >= 0 && p <= last_line) {
          double new_value = prev_value + accu / total;
          // differential approach ensures the filter coefficients sum to exactly FPScale)
          // The subtraction method guarantees that no matter how many terms we add, the
          // final sum will be exactly equal to the fixed-point representation of 1.0.
          program->pixel_coefficient[coeff_arr_base_index + kernel_size] = (short)((int)(new_value * current_FPScale + 0.5) - int(prev_value * current_FPScale + 0.5));
          prev_value = new_value;
          ++kernel_size;
          accu = 0;
        }
      }
    }

    // We even haven't reached any valid line,
    // or gathered accu values from past last line.
    if (accu != 0)
    {
      if (kernel_size > 0) {
        // Assign the remaining accumulator to the last line, just like we put
        // the accumulator before the first valid line to the first line.
        if (bits_per_pixel == 32)
          program->pixel_coefficient_float[coeff_arr_base_index + kernel_size - 1] += float(accu / total);
        else {
          double new_value = prev_value + accu / total;
          program->pixel_coefficient[coeff_arr_base_index + kernel_size - 1] += (short)((int)(new_value * current_FPScale + 0.5) - int(prev_value * current_FPScale + 0.5));
        }
        // no change in kernel_size
      }
      else
      {
        // new entry, accu/total must be 1.0 here (we always normalize)
        if (bits_per_pixel == 32)
          program->pixel_coefficient_float[coeff_arr_base_index + kernel_size] = float(accu / total);
        else
          program->pixel_coefficient[coeff_arr_base_index + kernel_size] = (short)((int)(accu / total * current_FPScale + 0.5));
        ++kernel_size;
      }
    }

    if (kernel_size == 0) {
      // write a single 1.0 coeff entry
      if (bits_per_pixel == 32)
        program->pixel_coefficient_float[coeff_arr_base_index + kernel_size] = 1.0f;
      else
        program->pixel_coefficient[coeff_arr_base_index + kernel_size] = (short)((int)(1.0 * current_FPScale + 0.5));
      ++kernel_size;
    }

    program->kernel_sizes[i] = kernel_size;
    if (kernel_size > max_kernel_size) max_kernel_size = kernel_size;

    pos += src_step;
  }

  // the different kernel sizes and coeff table will be later postprocessed
  // to have aligned and equally sized coefficients.

  program->filter_size_real = max_kernel_size;
  // can be less than original filter size if source dimensions are small

  return program;
}

#if HAVE_RESAMPLEMT
/******************************
 **** Desampling Patterns  ****
 *****************************/

ResamplingProgram* ResamplingFunction::GetDesamplingProgram(int source_size, double crop_start, double crop_size, int target_size, int bits_per_pixel, double center_pos_src, double center_pos_dst, uint8_t accuracy, int SizeY, uint8_t ShiftC, int &SizeOut, IScriptEnvironment* env)
{
  // edge condition ideas from fmtconv, thanks.
   double src_step = crop_size / double(target_size); // Distance between source pixels for adjacent dest pixels
   double zc_size = std::max(src_step, 1.0) / 1.0;    // Size of filter unit step (kernel_scale=1.0 in our case)
   double imp_step = 1.0 / zc_size;                   // Corresponding distance in the impulse
   double filter_support = support() * zc_size;       // Number of source pixels covered by the FIR

   int fir_filter_size = std::max(int(std::ceil(filter_support * 2)), 1);
   int max_kernel_size = 0;

   const int last_line = source_size - 1;

  ResamplingProgram *program = new ResamplingProgram(fir_filter_size, source_size, target_size, crop_start, crop_size, 32, env);

  /*if (!program->StatusOk)
  {
	  delete program;
	  return NULL;
  }*/

  // Initial position calculation

   double pos = crop_start;

  /*
  pre 3.7.4 logic:

  Now in 2025, let's fact-check this comment.

    pos = crop_start + ((crop_size - target_size) / (target_size*2)); // TODO this look wrong, gotta check
    ==>
    pos = crop_start + 1/2 * (crop_size / target_size - 1)
    ==>
    pos = crop_start + src_step * 0.5 - 1 * 0.5

    fmtconv generic formula:

    pos = crop_start + src_step * center_pos_dst - 1 * center_pos_src; // 3.7.4- fmtconv

    Solved: center_pos_dst = 0.5, center_pos_src = 0.5 in old Avisynth

  */

   // Introduces an offset because samples are located at the center of the
   // pixels, not on their boundaries. Excepted for pointresize.
   if (filter_support > 0)
  {
    // Pre 3.7.4 Avisynth worked with fixed center_pos_dst = center_pos_src = 0.5
    // Now it's externally configurable. In our use case they are always the same.
    pos += src_step * center_pos_dst - 1 * center_pos_src;
    //env->ThrowError("Resize: Source or target image too small for this resize method. Width=%d,%d, Support=%d",source_size,target_size,int(ceil(filter_support)));
  }
  else
  {
     // In case of PointResize(), which now returns real 0 for support().
     // Avisynth heritage.
     filter_support = 0.0001;
  }

    std::vector<double> coef_tmp;

  for (int i = 0; i < target_size; ++i)
   {
     coef_tmp.clear();

     int start_pos = (int)(pos + filter_support) - fir_filter_size + 1;
     program->pixel_offset[i] = clamp(start_pos, 0, last_line);

     // First pass: Accumulate all coefficients for weighting
     double total = 0.0;
     for (int k = 0; k < fir_filter_size; ++k)
     {
       const int p = start_pos + k;
       double val = f((pos - p) * imp_step);
       coef_tmp.push_back(val);
       total += val;
     }

     if (total == 0.0)
 	 {
       // Shouldn't happen for valid positions.
       total = 1.0;
     }

    const int coeff_arr_base_index = i * fir_filter_size;

    // Second pass: Generate real coefficients, handling edge conditions
    double accu = 0.0;
    double prev_value = 0.0;

    int kernel_size = 0;

       // Float version
       for (int k = 0; k < fir_filter_size; ++k)
 	   {
         const int p = start_pos + k;
         double val = coef_tmp[k];
         accu += val;
         if (p >= 0 && p <= last_line)
 		 {
           program->pixel_coefficient_float[coeff_arr_base_index + kernel_size] = float(accu / total);
           ++kernel_size;
           accu = 0;
         }
       }

     // We even haven't reached any valid line,
     // or gathered accu values from past last line.
     if (accu != 0)
     {
       if (kernel_size > 0)
 	   {
         // Assign the remaining accumulator to the last line, just like we put
         // the accumulator before the first valid line to the first line.
           program->pixel_coefficient_float[coeff_arr_base_index + kernel_size - 1] += float(accu / total);
         // no change in kernel_size
       }
       else
       {
           program->pixel_coefficient_float[coeff_arr_base_index + kernel_size] = float(accu / total);
         ++kernel_size;
       }
     }

    if (kernel_size == 0)
	{
       // write a single 1.0 coeff entry
         program->pixel_coefficient_float[coeff_arr_base_index + kernel_size] = 1.0f;
       ++kernel_size;
    }

     program->kernel_sizes[i] = kernel_size;
     if (kernel_size > max_kernel_size) max_kernel_size = kernel_size;

     pos += src_step;
  }

  program->filter_size_real = max_kernel_size;

  resize_prepare_coeffs(program,env,8);

  int posmin,posmax,SizeS0,SizeS,SizeM;

  Matrix_Compute AX0(target_size,source_size,DATA_FLOAT);

  AX0.FillZero();

  for (int i=0; i<target_size; i++)
  {
	  const int coeff_arr_base_index = i*program->filter_size;

 	  for (int j=0; j<program->filter_size_real; j++)
 		  AX0.SetF(i,program->pixel_offset[i]+j,program->pixel_coefficient_float[coeff_arr_base_index+j]);
  }

  delete program;

  posmin=0;
  while ((posmin<source_size) && (AX0.GetF(0,posmin)==0.0)) posmin++;
  posmax=source_size-1;
  while ((posmax>=0) && (AX0.GetF(target_size-1,posmax)==0.0)) posmax--;
  SizeS0=posmax-posmin+1;

  Matrix_Compute AX1(target_size,SizeS0,DATA_FLOAT),BX1(SizeS0,SizeS0,DATA_FLOAT),CX1(SizeS0,target_size,DATA_FLOAT);

  AX1.FillZero();

  for (int i=0; i<target_size; i++)
  {
	  for (int j=0; j<SizeS0; j++)
		  AX1.SetF(i,j,AX0.GetF(i,j+posmin));
  }

  BX1.Product_tAA(AX1);
  if (BX1.InverseSafe()!=0)
  {
	  SizeOut=-1;
	  return nullptr;
  }
  CX1.Product_AtB(BX1,AX1);

  switch(accuracy)
  {
	case 0 :
		for (int i=0; i<SizeS0; i++)
		{
			for (int j=0; j<target_size; j++)
				if (((int16_t)floor(0.5+CX1.GetF(i,j)*16384))==0) CX1.SetF(i,j,0.0);
		}
		break;
	case 1 :
		for (int i=0; i<SizeS0; i++)
		{
			float maxf=0.0;

			for (int j=0; j<target_size; j++)
				if (fabs(CX1.GetF(i,j))>maxf) maxf=(float)fabs(CX1.GetF(i,j));

			for (int j=0; j<target_size; j++)
				if (fabs(CX1.GetF(i,j)/maxf)<0.001) CX1.SetF(i,j,0.0);
		}
		break;
	case 2 :
		for (int i=0; i<SizeS0; i++)
		{
			for (int j=0; j<target_size; j++)
				if (fabs(CX1.GetF(i,j))<1e-6) CX1.SetF(i,j,0.0);
		}
		break;
	default :
		for (int i=0; i<SizeS0; i++)
		{
			for (int j=0; j<target_size; j++)
				if (((int16_t)floor(0.5+CX1.GetF(i,j)*16384))==0) CX1.SetF(i,j,0.0);
		}
		break;
  }

  fir_filter_size=0;
  for (int i=0; i<SizeS0; i++)
  {
	  int j1,j2,j=0;

	  while ((j<target_size) && (CX1.GetF(i,j)==0.0)) j++;
	  j1=j;
	  if (j1<target_size)
	  {
		  j=target_size-1;
		  while ((j>0) && (CX1.GetF(i,j)==0.0)) j--;
		  j2=j;
	  }
	  if ((j1<target_size) && ((j2-j1+1)>fir_filter_size)) fir_filter_size=j2-j1+1;
  }

  if (SizeY==0)
  {
	  switch(ShiftC)
	  {
		case 1 : SizeS=((SizeS0+1)>>1)<<1; break;
		case 2 : SizeS=((SizeS0+3)>>2)<<2; break;
		default : SizeS=SizeS0; break;
	  }
  }
  else SizeS=SizeY >> ShiftC;

  SizeOut=SizeS;

  program = new ResamplingProgram(fir_filter_size, target_size, SizeS, 0, SizeS, bits_per_pixel, env);

  /*if (!program->StatusOk)
  {
	  delete program;
	  return NULL;
  }*/

  if (SizeS0<SizeS) SizeM=SizeS0;
  else SizeM=SizeS;

  const int current_FPScale = ((bits_per_pixel>8) && (bits_per_pixel<=16)) ? FPScale16 : FPScale;

  for (int i=0; i<SizeM; i++)
  {
	  int start_pos=0;

	  while ((start_pos<target_size) && (CX1.GetF(i,start_pos)==0.0)) start_pos++;
      //while (start_pos<target_size) start_pos++;

	  int end_pos = start_pos+(fir_filter_size-1);

	  if (end_pos>=target_size) start_pos=target_size-fir_filter_size;

	  program->pixel_offset[i] = start_pos;
	  program->kernel_sizes[i] = fir_filter_size;

	  if (bits_per_pixel==32)
	  {
		  for (int j=0; j<fir_filter_size; j++)
			  program->pixel_coefficient_float[i*fir_filter_size+j]=CX1.GetF(i,start_pos+j);
	  }
	  else
	  {
		  for (int j=0; j<fir_filter_size; j++)
			  program->pixel_coefficient[i*fir_filter_size+j] = (short)floor(CX1.GetF(i,start_pos+j)*current_FPScale+0.5);
    }
  }

  for (int i=SizeM; i<SizeS; i++)
  {
	  int Pos1=posmax+(i-SizeM);

	  if (Pos1>=target_size) Pos1=target_size-1;

	  int start_pos=Pos1;
	  int end_pos = start_pos+(fir_filter_size-1);

	  if (end_pos>=target_size) start_pos=target_size-fir_filter_size;

	  program->pixel_offset[i] = start_pos;
	  program->kernel_sizes[i] = fir_filter_size;

	  if (bits_per_pixel==32)
	  {
		  for (int j=0; j<fir_filter_size; j++)
		  {
			  if ((start_pos+j)==Pos1)
				  program->pixel_coefficient_float[i*fir_filter_size+j]=1.0;
			  else
				  program->pixel_coefficient_float[i*fir_filter_size+j]=0.0;
		  }
	  }
	  else
	  {
		  for (int j=0; j<fir_filter_size; j++)
		  {
			  if ((start_pos+j)==Pos1)
				  program->pixel_coefficient[i*fir_filter_size+j] = (short)floor(1.0*current_FPScale+0.5);
			  else
				  program->pixel_coefficient[i*fir_filter_size+j] = 0;
		  }
	  }
  }

  program->filter_size_real = fir_filter_size;

  return program;
}


int ResamplingFunction::GetDesamplingData(int source_size, double crop_start, double crop_size, int target_size, int bits_per_pixel,
	double center_pos_src, double center_pos_dst, uint8_t ShiftC, IScriptEnvironment* env)
{
  // edge condition ideas from fmtconv, thanks.
  double src_step = crop_size / double(target_size); // Distance between source pixels for adjacent dest pixels
  double zc_size = std::max(src_step, 1.0) / 1.0;    // Size of filter unit step (kernel_scale=1.0 in our case)
  double imp_step = 1.0 / zc_size;                   // Corresponding distance in the impulse
  double filter_support = support() * zc_size;       // Number of source pixels covered by the FIR

  int fir_filter_size = std::max(int(std::ceil(filter_support * 2)), 1);
  int max_kernel_size = 0;

  const int last_line = source_size - 1;

  ResamplingProgram* program = new ResamplingProgram(fir_filter_size, source_size, target_size, crop_start, crop_size, 32, env);

  /*if (!program->StatusOk)
  {
	  delete program;
	  return -1;
  }*/

  // Initial position calculation

  double pos = crop_start;

  /*
  pre 3.7.4 logic:

  Now in 2025, let's fact-check this comment.

    pos = crop_start + ((crop_size - target_size) / (target_size*2)); // TODO this look wrong, gotta check
    ==>
    pos = crop_start + 1/2 * (crop_size / target_size - 1)
    ==>
    pos = crop_start + src_step * 0.5 - 1 * 0.5

    fmtconv generic formula:

    pos = crop_start + src_step * center_pos_dst - 1 * center_pos_src; // 3.7.4- fmtconv

    Solved: center_pos_dst = 0.5, center_pos_src = 0.5 in old Avisynth

  */

  // Introduces an offset because samples are located at the center of the
  // pixels, not on their boundaries. Excepted for pointresize.
  if (filter_support > 0)
  {
    // Pre 3.7.4 Avisynth worked with fixed center_pos_dst = center_pos_src = 0.5
    // Now it's externally configurable. In our use case they are always the same.
    pos += src_step * center_pos_dst - 1 * center_pos_src;
  }
  else
  {
    // In case of PointResize(), which now returns real 0 for support().
    // Avisynth heritage.
    filter_support = 0.0001;
  }

  const int current_FPScale = (bits_per_pixel > 8 && bits_per_pixel <= 16) ? FPScale16 : FPScale;

  std::vector<double> coef_tmp;

  for (int i = 0; i < target_size; ++i)
  {
    coef_tmp.clear();

    int start_pos = (int)(pos + filter_support) - fir_filter_size + 1;
    program->pixel_offset[i] = clamp(start_pos, 0, last_line);

    // First pass: Accumulate all coefficients for weighting
    double total = 0.0;
    for (int k = 0; k < fir_filter_size; ++k)
	{
      const int p = start_pos + k;
      double val = f((pos - p) * imp_step);
      coef_tmp.push_back(val);
      total += val;
    }

    if (total == 0.0)
	{
      // Shouldn't happen for valid positions.
      total = 1.0;
    }

    const int coeff_arr_base_index = i * fir_filter_size;

    // Second pass: Generate real coefficients, handling edge conditions
    double accu = 0.0;
    double prev_value = 0.0;

    int kernel_size = 0;

      // Float version
      for (int k = 0; k < fir_filter_size; ++k)
	  {
        const int p = start_pos + k;
        double val = coef_tmp[k];
        accu += val;
        if (p >= 0 && p <= last_line)
		{
          program->pixel_coefficient_float[coeff_arr_base_index + kernel_size] = float(accu / total);
          ++kernel_size;
          accu = 0;
        }
      }

    // We even haven't reached any valid line,
    // or gathered accu values from past last line.
    if (accu != 0)
    {
      if (kernel_size > 0)
	  {
        // Assign the remaining accumulator to the last line, just like we put
        // the accumulator before the first valid line to the first line.
          program->pixel_coefficient_float[coeff_arr_base_index + kernel_size - 1] += float(accu / total);
        // no change in kernel_size
      }
      else
      {
          program->pixel_coefficient_float[coeff_arr_base_index + kernel_size] = float(accu / total);
        ++kernel_size;
      }
    }

    if (kernel_size == 0)
	{
      // write a single 1.0 coeff entry
        program->pixel_coefficient_float[coeff_arr_base_index + kernel_size] = 1.0f;
      ++kernel_size;
    }

    program->kernel_sizes[i] = kernel_size;
    if (kernel_size > max_kernel_size) max_kernel_size = kernel_size;

    pos += src_step;
  }

  program->filter_size_real = max_kernel_size;

  resize_prepare_coeffs(program,env,8);

  int posmin,posmax,SizeS;

  posmin=0;
  while ((posmin<fir_filter_size) && (program->pixel_coefficient_float[posmin]==0.0)) posmin++;
  posmin+=program->pixel_offset[0];
  posmax=fir_filter_size-1;
  while ((posmax>=0) && (program->pixel_coefficient_float[(target_size-1)*fir_filter_size+posmax]==0.0)) posmax--;
  posmax+=program->pixel_offset[target_size-1];
  SizeS=posmax-posmin+1;

  delete program;

  switch(ShiftC)
  {
	case 1 : SizeS=((SizeS+1)>>1)<<1; break;
	case 2 : SizeS=((SizeS+3)>>2)<<2; break;
	default : break;
  }

  return(SizeS);
}
#endif
