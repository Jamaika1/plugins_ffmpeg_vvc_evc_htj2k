#ifdef ZIMG_X86

#include "../../cpuinfo.h"
#include "../../pixel.h"
#include "cpuinfo_x86.h"
#include "../../libgraphengine/filter.h"
#include "dither_x86.h"

namespace zimg::depth {

namespace {

dither_convert_func select_ordered_dither_func_avx2(PixelType pixel_in, PixelType pixel_out)
{
	if (pixel_in == PixelType::BYTE && pixel_out == PixelType::BYTE)
		return ordered_dither_b2b_avx2;
	else if (pixel_in == PixelType::BYTE && pixel_out == PixelType::WORD)
		return ordered_dither_b2w_avx2;
	else if (pixel_in == PixelType::WORD && pixel_out == PixelType::BYTE)
		return ordered_dither_w2b_avx2;
	else if (pixel_in == PixelType::WORD && pixel_out == PixelType::WORD)
		return ordered_dither_w2w_avx2;
	else if (pixel_in == PixelType::HALF && pixel_out == PixelType::BYTE)
		return ordered_dither_h2b_avx2;
	else if (pixel_in == PixelType::HALF && pixel_out == PixelType::WORD)
		return ordered_dither_h2w_avx2;
	else if (pixel_in == PixelType::FLOAT && pixel_out == PixelType::BYTE)
		return ordered_dither_f2b_avx2;
	else if (pixel_in == PixelType::FLOAT && pixel_out == PixelType::WORD)
		return ordered_dither_f2w_avx2;
	else
		return nullptr;
}

#if defined(__AVX512F__)
dither_convert_func select_ordered_dither_func_avx512(PixelType pixel_in, PixelType pixel_out)
{
	if (pixel_in == PixelType::BYTE && pixel_out == PixelType::BYTE)
		return ordered_dither_b2b_avx512;
	else if (pixel_in == PixelType::BYTE && pixel_out == PixelType::WORD)
		return ordered_dither_b2w_avx512;
	else if (pixel_in == PixelType::WORD && pixel_out == PixelType::BYTE)
		return ordered_dither_w2b_avx512;
	else if (pixel_in == PixelType::WORD && pixel_out == PixelType::WORD)
		return ordered_dither_w2w_avx512;
	else if (pixel_in == PixelType::HALF && pixel_out == PixelType::BYTE)
		return ordered_dither_h2b_avx512;
	else if (pixel_in == PixelType::HALF && pixel_out == PixelType::WORD)
		return ordered_dither_h2w_avx512;
	else if (pixel_in == PixelType::FLOAT && pixel_out == PixelType::BYTE)
		return ordered_dither_f2b_avx512;
	else if (pixel_in == PixelType::FLOAT && pixel_out == PixelType::WORD)
		return ordered_dither_f2w_avx512;
	else
		return nullptr;
}
#endif

} // namespace


dither_convert_func select_ordered_dither_func_x86(const PixelFormat &pixel_in, const PixelFormat &pixel_out, CPUClass cpu)
{
	X86Capabilities caps = query_x86_capabilities();
	dither_convert_func func = nullptr;

	if (cpu_is_autodetect(cpu)) {
#if defined(__AVX512F__)
		if (!func && cpu == CPUClass::AUTO_64B && caps.avx512f && caps.avx512bw && caps.avx512vl)
			func = select_ordered_dither_func_avx512(pixel_in.type, pixel_out.type);
#endif
		if (!func && caps.avx2 && caps.fma)
			func = select_ordered_dither_func_avx2(pixel_in.type, pixel_out.type);
	} else {
#if defined(__AVX512F__)
		if (!func && cpu >= CPUClass::X86_AVX512)
			func = select_ordered_dither_func_avx512(pixel_in.type, pixel_out.type);
#endif
		if (!func && cpu >= CPUClass::X86_AVX2)
			func = select_ordered_dither_func_avx2(pixel_in.type, pixel_out.type);
	}

	return func;
}

std::unique_ptr<graphengine::Filter> create_error_diffusion_x86(unsigned width, unsigned height, const PixelFormat &pixel_in, const PixelFormat &pixel_out, CPUClass cpu)
{
	X86Capabilities caps = query_x86_capabilities();
	std::unique_ptr<graphengine::Filter> ret;

	if (cpu_is_autodetect(cpu)) {
		if (!ret && caps.avx2 && caps.f16c && caps.fma)
			ret = create_error_diffusion_avx2(width, height, pixel_in, pixel_out);
	} else {
		if (!ret && cpu >= CPUClass::X86_AVX2)
			ret = create_error_diffusion_avx2(width, height, pixel_in, pixel_out);
	}

	return ret;
}

} // namespace zimg::depth

#endif // ZIMG_X86
