#ifdef ZIMG_X86

#include "../../cpuinfo.h"
#include "cpuinfo_x86.h"
#include "../../pixel.h"
#include "depth_convert_x86.h"

namespace zimg::depth {

namespace {


left_shift_func select_left_shift_func_avx2(PixelType pixel_in, PixelType pixel_out)
{
	if (pixel_in == PixelType::BYTE && pixel_out == PixelType::BYTE)
		return left_shift_b2b_avx2;
	else if (pixel_in == PixelType::BYTE && pixel_out == PixelType::WORD)
		return left_shift_b2w_avx2;
	else if (pixel_in == PixelType::WORD && pixel_out == PixelType::BYTE)
		return left_shift_w2b_avx2;
	else if (pixel_in == PixelType::WORD && pixel_out == PixelType::WORD)
		return left_shift_w2w_avx2;
	else
		return nullptr;
}

#if defined(__AVX512F__)
left_shift_func select_left_shift_func_avx512(PixelType pixel_in, PixelType pixel_out)
{
	if (pixel_in == PixelType::BYTE && pixel_out == PixelType::BYTE)
		return left_shift_b2b_avx512;
	else if (pixel_in == PixelType::BYTE && pixel_out == PixelType::WORD)
		return left_shift_b2w_avx512;
	else if (pixel_in == PixelType::WORD && pixel_out == PixelType::BYTE)
		return left_shift_w2b_avx512;
	else if (pixel_in == PixelType::WORD && pixel_out == PixelType::WORD)
		return left_shift_w2w_avx512;
	else
		return nullptr;
}
#endif

depth_convert_func select_depth_convert_func_avx2(PixelType pixel_in, PixelType pixel_out)
{
	if (pixel_in == PixelType::BYTE && pixel_out == PixelType::HALF)
		return depth_convert_b2h_avx2;
	else if (pixel_in == PixelType::BYTE && pixel_out == PixelType::FLOAT)
		return depth_convert_b2f_avx2;
	else if (pixel_in == PixelType::WORD && pixel_out == PixelType::HALF)
		return depth_convert_w2h_avx2;
	else if (pixel_in == PixelType::WORD && pixel_out == PixelType::FLOAT)
		return depth_convert_w2f_avx2;
	else if (pixel_in == PixelType::HALF && pixel_out == PixelType::FLOAT)
		return half_to_float_avx2;
	else if (pixel_in == PixelType::FLOAT && pixel_out == PixelType::HALF)
		return float_to_half_avx2;
	else
		return nullptr;
}

#if defined(__AVX512F__)
depth_convert_func select_depth_convert_func_avx512(PixelType pixel_in, PixelType pixel_out)
{
	if (pixel_in == PixelType::BYTE && pixel_out == PixelType::HALF)
		return depth_convert_b2h_avx512;
	else if (pixel_in == PixelType::BYTE && pixel_out == PixelType::FLOAT)
		return depth_convert_b2f_avx512;
	else if (pixel_in == PixelType::WORD && pixel_out == PixelType::HALF)
		return depth_convert_w2h_avx512;
	else if (pixel_in == PixelType::WORD && pixel_out == PixelType::FLOAT)
		return depth_convert_w2f_avx512;
	else
		return nullptr;
}
#endif

} // namespace


left_shift_func select_left_shift_func_x86(PixelType pixel_in, PixelType pixel_out, CPUClass cpu)
{
	X86Capabilities caps = query_x86_capabilities();
	left_shift_func func = nullptr;

	if (cpu_is_autodetect(cpu)) {
#if defined(__AVX512F__)
		if (!func && cpu == CPUClass::AUTO_64B && caps.avx512f && caps.avx512bw && caps.avx512vl)
			func = select_left_shift_func_avx512(pixel_in, pixel_out);
#endif
		if (!func && caps.avx2)
			func = select_left_shift_func_avx2(pixel_in, pixel_out);
	} else {
#if defined(__AVX512F__)
		if (!func && cpu >= CPUClass::X86_AVX512)
			func = select_left_shift_func_avx512(pixel_in, pixel_out);
#endif
		if (!func && cpu >= CPUClass::X86_AVX2)
			func = select_left_shift_func_avx2(pixel_in, pixel_out);
	}

	return func;
}

depth_convert_func select_depth_convert_func_x86(const PixelFormat &pixel_in, const PixelFormat &pixel_out, CPUClass cpu)
{
	X86Capabilities caps = query_x86_capabilities();
	depth_convert_func func = nullptr;

	if (cpu_is_autodetect(cpu)) {
#if defined(__AVX512F__)
		if (!func && cpu == CPUClass::AUTO_64B && caps.avx512f && caps.avx512bw && caps.avx512vl)
			func = select_depth_convert_func_avx512(pixel_in.type, pixel_out.type);
#endif
		if (!func && caps.avx2 && caps.fma)
			func = select_depth_convert_func_avx2(pixel_in.type, pixel_out.type);
	} else {
#if defined(__AVX512F__)
		if (!func && cpu >= CPUClass::X86_AVX512)
			func = select_depth_convert_func_avx512(pixel_in.type, pixel_out.type);
#endif
		if (!func && cpu >= CPUClass::X86_AVX2)
			func = select_depth_convert_func_avx2(pixel_in.type, pixel_out.type);
	}

	return func;
}

} // namespace zimg::depth

#endif // ZIMG_X86
