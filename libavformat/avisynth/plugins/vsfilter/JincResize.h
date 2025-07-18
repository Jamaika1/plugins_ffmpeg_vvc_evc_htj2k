#ifndef __JINCRESIZE_H__
#define __JINCRESIZE_H__

#include <execution>
#include <string>
#include <vector>

#include "../../avisynth.h"
//#include "../../avs/minmax.h"

struct EWAPixelCoeffMeta
{
    int start_x;
    int start_y;
    int coeff_meta;
};

struct EWAPixelCoeff
{
    float* factor;
    EWAPixelCoeffMeta* meta;
    int* factor_map;
    int filter_size;
    int coeff_stride;
};

class Lut
{
    int lut_size = 1024;

public:
    Lut();
    void InitLut(int lut_size, double radius, double blur);
    float GetFactor(int index);

    double* lut;
};

class JincResize : public GenericVideoFilter
{
    std::string cplace;
    Lut* init_lut;
    std::vector<EWAPixelCoeff*> out1;
    int planecount;
    float peak;

    template<typename T, int thr, int subsampled>
    void resize_plane_c(PVideoFrame& src, PVideoFrame& dst, IScriptEnvironment* env);
	template<typename T, int thr, int subsampled>
	void resize_eqplanes_c(PVideoFrame& src, PVideoFrame& dst, IScriptEnvironment* env);
    template <typename T, int thr, int subsampled>
    void resize_plane_sse41(PVideoFrame& src, PVideoFrame& dst, IScriptEnvironment* env);
	template <typename T, int thr, int subsampled>
	void resize_eqplanes_sse41(PVideoFrame& src, PVideoFrame& dst, IScriptEnvironment* env);
#if defined(__AVX2__)
    template <typename T, int thr, int subsampled>
    void resize_plane_avx2(PVideoFrame& src, PVideoFrame& dst, IScriptEnvironment* env);
#endif
#if defined(__AVX512F__)
    template <typename T, int thr, int subsampled>
    void resize_plane_avx512(PVideoFrame& src, PVideoFrame& dst, IScriptEnvironment* env);
#endif

    void(JincResize::*process_frame)(PVideoFrame&, PVideoFrame&, IScriptEnvironment*);

public:
    JincResize(PClip _child, int target_width, int target_height, double crop_left, double crop_top, double crop_width, double crop_height, int quant_x, int quant_y, int tap, double blur,
        std::string cplace, int threads, int opt, int initial_capacity, double initial_factor, IScriptEnvironment* env);
    PVideoFrame __stdcall GetFrame(int n, IScriptEnvironment* env);
    int __stdcall SetCacheHints(int cachehints, int frame_range)
    {
        return cachehints == CACHE_GET_MTMODE ? MT_MULTI_INSTANCE : 0;
    }
    ~JincResize();
};

/*class Arguments
{
    AVSValue _args[12];
    const char* _arg_names[12];
    int _idx;

public:
    Arguments() : _args{}, _arg_names{}, _idx{} {}

    void add(AVSValue arg, const char* arg_name = nullptr)
    {
        _args[_idx] = arg;
        _arg_names[_idx] = arg_name;
        ++_idx;
    }

    AVSValue args() const { return{ _args, _idx }; }

    const char* const* arg_names() const { return _arg_names; }
};*/

#endif
