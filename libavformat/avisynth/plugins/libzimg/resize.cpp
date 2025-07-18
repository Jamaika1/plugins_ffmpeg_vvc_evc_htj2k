#include <algorithm>
#include "cpuinfo.h"
#include "except.h"
#include "pixel.h"
#include "resize.h"
#include "resize_impl.h"

namespace zimg::resize {

namespace {

bool resize_h_first(double xscale, double yscale) noexcept
{
	double h_first_cost = std::max(xscale, 1.0) * 2.0 + xscale * std::max(yscale, 1.0);
	double v_first_cost = std::max(yscale, 1.0) + yscale * std::max(xscale, 1.0) * 2.0;

	return h_first_cost < v_first_cost;
}

} // namespace


ResizeConversion::ResizeConversion(unsigned src_width, unsigned src_height, PixelType type) :
	src_width{ src_width },
	src_height{ src_height },
	type{ type },
	depth{ pixel_depth(type) },
	filter{},
	dst_width{ src_width },
	dst_height{ src_height },
	shift_w{},
	shift_h{},
	subwidth{ static_cast<double>(src_width) },
	subheight{ static_cast<double>(src_height) },
	cpu{ CPUClass::NONE }
{}

auto ResizeConversion::create() const -> filter_pair try
{
	if (src_width > pixel_max_width(type) || dst_width > pixel_max_width(type))
		error::throw_<error::OutOfMemory>();

	bool skip_h = (src_width == dst_width && shift_w == 0 && subwidth == src_width);
	bool skip_v = (src_height == dst_height && shift_h == 0 && subheight == src_height);

	if (skip_h && skip_v)
		return{};

	auto builder = ResizeImplBuilder{ src_width, src_height, type }
		.set_depth(depth)
		.set_filter(filter)
		.set_cpu(cpu);
	filter_pair ret{};

	if (skip_h) {
		ret.first = builder.set_horizontal(false)
		                   .set_dst_dim(dst_height)
		                   .set_shift(shift_h)
		                   .set_subwidth(subheight)
		                   .create();
	} else if (skip_v) {
		ret.first = builder.set_horizontal(true)
		                   .set_dst_dim(dst_width)
		                   .set_shift(shift_w)
		                   .set_subwidth(subwidth)
		                   .create();
	} else {
		bool h_first = resize_h_first(static_cast<double>(dst_width) / subwidth, static_cast<double>(dst_height) / subheight);

		if (h_first) {
			ret.first = builder.set_horizontal(true)
			                   .set_dst_dim(dst_width)
			                   .set_shift(shift_w)
			                   .set_subwidth(subwidth)
			                   .create();

			builder.src_width = dst_width;
			ret.second = builder.set_horizontal(false)
			                    .set_dst_dim(dst_height)
			                    .set_shift(shift_h)
			                    .set_subwidth(subheight)
			                    .create();
		} else {
			ret.first = builder.set_horizontal(false)
			                   .set_dst_dim(dst_height)
			                   .set_shift(shift_h)
			                   .set_subwidth(subheight)
			                   .create();

			builder.src_height = dst_height;
			ret.second = builder.set_horizontal(true)
			                    .set_dst_dim(dst_width)
			                    .set_shift(shift_w)
			                    .set_subwidth(subwidth)
			                    .create();
		}
	}

	return ret;
} catch (const std::bad_alloc &) {
	error::throw_<error::OutOfMemory>();
}

} // namespace zimg::resize
