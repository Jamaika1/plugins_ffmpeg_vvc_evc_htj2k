#ifdef ZIMG_X86

#include "../../cpuinfo.h"
#include "cpuinfo_x86.h"
#include "../../libgraphengine/filter.h"
#include "resize_impl_x86.h"

namespace zimg::resize {

std::unique_ptr<graphengine::Filter> create_resize_impl_h_x86(const FilterContext &context, unsigned height, PixelType type, unsigned depth, CPUClass cpu)
{
	X86Capabilities caps = query_x86_capabilities();
	std::unique_ptr<graphengine::Filter> ret;

	if (cpu_is_autodetect(cpu)) {
#if defined(__AVX512F__)
		if (!ret && cpu == CPUClass::AUTO_64B) {
			if (!ret && cpu_has_avx512_f_dq_bw_vl(caps) && caps.avx512vnni)
				ret = create_resize_impl_h_avx512_vnni(context, height, type, depth);
			if (!ret && cpu_has_avx512_f_dq_bw_vl(caps))
				ret = create_resize_impl_h_avx512(context, height, type, depth);
		}
#endif
		if (!ret && caps.avx2)
			ret = create_resize_impl_h_avx2(context, height, type, depth);
	} else {
#if defined(__AVX512F__)
		if (!ret && cpu >= CPUClass::X86_AVX512_CLX)
			ret = create_resize_impl_h_avx512_vnni(context, height, type, depth);
		if (!ret && cpu >= CPUClass::X86_AVX512)
			ret = create_resize_impl_h_avx512(context, height, type, depth);
#endif
		if (!ret && cpu >= CPUClass::X86_AVX2)
			ret = create_resize_impl_h_avx2(context, height, type, depth);
	}

	return ret;
}

std::unique_ptr<graphengine::Filter> create_resize_impl_v_x86(const FilterContext &context, unsigned width, PixelType type, unsigned depth, CPUClass cpu)
{
	X86Capabilities caps = query_x86_capabilities();
	std::unique_ptr<graphengine::Filter> ret;

	if (cpu_is_autodetect(cpu)) {
#if defined(__AVX512F__)
		if (!ret && cpu == CPUClass::AUTO_64B) {
			if (!ret && cpu_has_avx512_f_dq_bw_vl(caps) && caps.avx512vnni)
				ret = create_resize_impl_v_avx512_vnni(context, width, type, depth);
			if (!ret && cpu_has_avx512_f_dq_bw_vl(caps))
				ret = create_resize_impl_v_avx512(context, width, type, depth);
		}
#endif
		if (!ret && caps.avx2)
			ret = create_resize_impl_v_avx2(context, width, type, depth);
	} else {
#if defined(__AVX512F__)
		if (!ret && cpu >= CPUClass::X86_AVX512_CLX)
			ret = create_resize_impl_v_avx512_vnni(context, width, type, depth);
		if (!ret && cpu >= CPUClass::X86_AVX512)
			ret = create_resize_impl_v_avx512(context, width, type, depth);
#endif
		if (!ret && cpu >= CPUClass::X86_AVX2)
			ret = create_resize_impl_v_avx2(context, width, type, depth);
	}

	return ret;
}

} // namespace zimg::resize

#endif // ZIMG_X86
