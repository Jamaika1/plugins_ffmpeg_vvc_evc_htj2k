#include <windows.h>
#include "../../avisynth.h"
#include <vector>
#include <immintrin.h>
#include <algorithm>

class GetAudio : public GenericVideoFilter {
public:
	GetAudio(PClip _child, IScriptEnvironment* env) : GenericVideoFilter(_child) {
		buffer = malloc((int)vi.BytesFromAudioSamples(vi.AudioSamplesFromFrames(1) + 1));
		if (!buffer) env->ThrowError("GetAudio: Could not allocate buffer");
	}
	PVideoFrame __stdcall GetFrame(int n, IScriptEnvironment* env) {
		int64_t start = vi.AudioSamplesFromFrames(n);
		int64_t count = vi.AudioSamplesFromFrames(n + 1) - start;
		child->GetAudio(buffer, start, count, env);
		return child->GetFrame(n, env);
	}

private:
	void* buffer;
};

AVSValue __cdecl CreateGetAudio(AVSValue args, void* user_data, IScriptEnvironment* env) {
	return new GetAudio(
		args[0].AsClip(),
		env
	);
}

class Waveform : public GenericVideoFilter {
public:
	Waveform(PClip _child, float _window, float _height, float _zoom, bool _under, bool _smooth, bool _anti_alias, IScriptEnvironment* env);
	~Waveform();
	PVideoFrame __stdcall GetFrame(int n, IScriptEnvironment* env);

private:
	PClip resampled_child = NULL;
	VideoInfo resampled_vi;
	int window;
	bool under;
	bool smooth;
	bool anti_alias;
	int waveform_y;
	int total_waveform_height;
	int channel_height;
	int left_offset;
	int right_missing;
	int last_pane = 0x7fffffff;
	int last_frame = 0x7fffffff;
	__m128i darken_mask;
	__m128i alpha_mask;
	__m128i quarter_i;
	__m128i eighth_i;
	int eighth_int32;
	__m128 half = _mm_set1_ps(0.5f);
	__m128 quarter = _mm_set1_ps(0.25f);
	__m128 eighth = _mm_set1_ps(0.125f);
	std::vector<int> planes;
	std::vector<int> columns;
	std::vector<uint8_t*> panes;
	int buffer_samples;
	void* buffer;
	int pixels_per_pane;
	int supersamples_per_pane;
	int subpixel_max_height;
	int min_samples_per_supersample;
	int nominal_samples_per_frame;
	int bytes_per_sample;
	int bres;
	int extras;
	int64_t start_sample;
	int64_t end_sample;
	int64_t starts[4];
	struct min_max {
		int min;
		int max;
	};
	struct f_min_max {
		float min;
		float max;
	};
	min_max(*mm_a)[4];
	min_max(*mm_b)[4];
	min_max* sst;
	min_max* sst_p;
	min_max* sst_p_draw;
	int64_t* avg_totals;
	double multiply;
	double multiply1;
	double multiply2;
	bool stretched;
	double add;
	double add1, add2;
	float zoom;
	int getsamples_p;
	union int_float {
		int i;
		float f;
	};
	int_float white_lookup[256 * 17];
	int_float grey_lookup[256 * 17] ;
	uint8_t** channel_p;
	void GetSamples();
	void Pane(int frame, uint8_t* pane, IScriptEnvironment* env);
	void __stdcall GetAudio2(void* buffer, __int64 start, __int64 count, IScriptEnvironment* env);
};

Waveform::Waveform(PClip _child, float _window, float _height, float _zoom, bool _under, bool _smooth, bool _anti_alias, IScriptEnvironment* env) : GenericVideoFilter(_child), under(_under), zoom(_zoom), smooth(_smooth), anti_alias(_anti_alias) {
	_window = (std::max)(_window, 0.0f);
	if (_height <= 0 || (!under && _height > vi.height)) env->ThrowError("Waveform: Bad height");
	window = (int)ceilf(_window);
	pixels_per_pane = (int)ceilf((float)vi.width / ((_window * 2) + 1));
	pixels_per_pane += (vi.width ^ pixels_per_pane) & 1; // makes the middle pane perfectly centred
	window = (int)ceilf(((float)(vi.width - pixels_per_pane) / 2) / pixels_per_pane);
	int	n_panes = (window << 1) + 1;

	left_offset = (pixels_per_pane * n_panes - vi.width) >> 1;
	right_missing = (pixels_per_pane * n_panes - left_offset) - vi.width;

	int channel_height_mod = vi.AudioChannels() << 1; // double so height ends up being the same for all colourspaces
	total_waveform_height = (_height <= 1) ? (int)floor(vi.height * _height) : (int)floor(_height);
	total_waveform_height = (total_waveform_height / channel_height_mod) * channel_height_mod;
	if (total_waveform_height < vi.AudioChannels()) env->ThrowError("Waveform: Video height too small");

	channel_height = total_waveform_height / vi.AudioChannels();

	if (under) {
		AVSValue args[5] = { child, 0, 0, 0, total_waveform_height };
		child = env->Invoke("AddBorders", AVSValue(args, 5)).AsClip();
	}

	vi = child->GetVideoInfo();

	waveform_y = vi.height - total_waveform_height;

// planes
	int bits = vi.BitsPerComponent();

	if (!vi.IsPlanar()) { // interleaved RGB(A)
		planes.push_back(0);
		switch (vi.BitsPerPixel()) {
			case 24: darken_mask = _mm_set1_epi32(0x7f7f7f7f); quarter_i = _mm_set1_epi32(0x40404040); break;
			case 32: darken_mask = _mm_set1_epi32(0x007f7f7f); quarter_i = _mm_set1_epi32(0x40404040); alpha_mask = _mm_set1_epi32(0xff000000); break;
			case 48: darken_mask = _mm_set1_epi32(0x7fff7fff); quarter_i = _mm_set1_epi32(0x40004000); break;
			case 64: darken_mask = _mm_set_epi32(0x00007fff, 0x7fff7fff, 0x00007fff, 0x7fff7fff); quarter_i = _mm_set_epi32(0x00004000, 0x40004000, 0x00004000, 0x40004000); alpha_mask = _mm_set_epi32(0xffff0000, 0, 0xffff0000, 0);  break;
		}
		eighth_i = _mm_srli_epi32(quarter_i, 1);
	} else {
		if (vi.IsRGB()) {
			planes.assign({ PLANAR_R, PLANAR_G, PLANAR_B });
			if (vi.IsPlanarRGBA()) planes.push_back(PLANAR_A);
		} else {
			if (vi.NumComponents() == 1) planes.assign({ PLANAR_Y });
			else planes.assign({ PLANAR_Y, PLANAR_U, PLANAR_V });
			if (vi.IsYUVA()) planes.push_back(PLANAR_A);
		}

		switch (bits) {
			case 8: darken_mask = _mm_set1_epi32(0x7f7f7f7f); quarter_i = _mm_set1_epi32(0x40404040); break;
			default: darken_mask = _mm_set1_epi32(0x7fff7fff); quarter_i = _mm_set1_epi16(1 << (bits - 2)); break;
		}

		eighth_i = _mm_srli_epi32(quarter_i, 1);
	}

	eighth_int32 = 1 << (bits - 3);

// vertical separators
	if (window) {
		for (int w = 1; w <= window; ++w) {
			columns.push_back((vi.width >> 1) + pixels_per_pane * w - (pixels_per_pane >> 1) - 1);
			columns.push_back((vi.width >> 1) - pixels_per_pane * w + (pixels_per_pane >> 1));
		}
		columns.push_back(columns[0] + 2);
		columns.push_back(columns[1] - 2);

		if (!vi.IsPlanar()) {
			std::vector<int> temp;
			int m = vi.NumComponents();

			for (auto col : columns) {
				temp.push_back(col * m);
				temp.push_back(col * m + 1);
				temp.push_back(col * m + 2);
			}

			columns = temp;
		}

		std::sort(columns.begin(), columns.end());
	}

// arrays
	mm_a = new min_max[vi.AudioChannels()][4];
	mm_b = new min_max[vi.AudioChannels()][4];
	sst = new min_max[vi.AudioChannels()];
	sst_p = new min_max[vi.AudioChannels()];
	sst_p_draw = new min_max[vi.AudioChannels()];
	avg_totals = new int64_t[vi.AudioChannels()];

// panes
	for (int p = 0; p < (window << 1) + 1; ++p) {
		panes.push_back(new uint8_t[((channel_height + 5) * vi.AudioChannels()) * pixels_per_pane]);
	}

// waveform parameters
	supersamples_per_pane = pixels_per_pane << (smooth || !anti_alias ? 0 : 2);
	resampled_vi = vi;

	while (true) {
		bytes_per_sample = (int)resampled_vi.BytesFromAudioSamples(1);
		min_samples_per_supersample = (int)resampled_vi.AudioSamplesFromFrames(1) / (supersamples_per_pane);
		stretched = !min_samples_per_supersample;
		if (!smooth || !stretched) break;
		float resample = (float)supersamples_per_pane / (int)resampled_vi.AudioSamplesFromFrames(1);
		int pow = (int)ceil(log2f(resample));
		if (!pow || resampled_child) env->ThrowError("Waveform: Smoothing/resampling factor error");
		AVSValue args[2] = { child, resampled_vi.SamplesPerSecond() << pow };
		resampled_child = env->Invoke("ResampleAudio", AVSValue(args, 2)).AsClip();
		resampled_vi = resampled_child->GetVideoInfo();
	}

	if (!resampled_child) resampled_child = child;

	nominal_samples_per_frame = min_samples_per_supersample * supersamples_per_pane;

	buffer_samples = (int)resampled_vi.AudioSamplesFromFrames(1) + 1 + ((min_samples_per_supersample + 1) << 2);
	buffer = malloc((int)resampled_vi.BytesFromAudioSamples(buffer_samples));

	if (!buffer) env->ThrowError("Waveform: Could not allocate buffer");

	subpixel_max_height = (channel_height << (anti_alias ? 2 : 0)) - (anti_alias ? 4 : 1);
	int range, half;

	channel_p = new uint8_t*[resampled_vi.AudioChannels()];

	switch (resampled_vi.SampleType()) {
		case SAMPLE_INT8: range = 0xff; half = 0x80; break;
		case SAMPLE_INT16:
		case SAMPLE_INT24:
		case SAMPLE_INT32: range = 0xffff; half = 0x8000; break;
		case SAMPLE_FLOAT: range = 2; half = 1; break;
	}

	double mul;

	mul = (double)subpixel_max_height / range;
	multiply = zoom * mul;
	add = half * mul - (resampled_vi.SampleType() == SAMPLE_INT8 ? half * multiply1 : 0) + 0.5;

	mul = (double)subpixel_max_height / (min_samples_per_supersample * range);
	multiply1 = zoom * mul;
	add1 = half * min_samples_per_supersample * mul - (resampled_vi.SampleType() == SAMPLE_INT8 ? half * min_samples_per_supersample * multiply1 : 0) + 0.5;

	mul = (double)subpixel_max_height / ((min_samples_per_supersample + 1) * range);
	multiply2 = zoom * mul;
	add2 = half * (min_samples_per_supersample + 1) * mul - (resampled_vi.SampleType() == SAMPLE_INT8 ? half * min_samples_per_supersample * multiply2 : 0) + 0.5;

// grey/white lookup tables
	if (anti_alias) {
		const int bpc = vi.BitsPerComponent();
		for (int c = 0; c < 256; ++c) {
			for (int g = 0; g < 17; ++g) {
				int index = (g << 8) | c;
				float white = sqrtf(65025 - ((65025 - c * c) * ((float)(16 - g) / 16)));
				float grey = sqrtf(65025 - ((65025 - c * c) * ((float)(48 - g) / 48)));
				switch (vi.BitsPerComponent()) {
					case 32: {
						white_lookup[index].f = white / 256;
						grey_lookup[index].f = grey / 256;
					} break;
					default: {
						int _white = (int)white;
						int _grey = (int)grey;
						white_lookup[(g << 8) | c].i = (_white << (bpc - 8)) | (_white >> (16 - bpc));
						grey_lookup[(g << 8) | c].i = (_grey << (bpc - 8)) | (_grey >> (16 - bpc));
					}
				}
			}
		}
	}
}

Waveform::~Waveform() {
	planes.clear();
	columns.clear();

	for (auto pane : panes) {
		delete[] pane;
	}
	panes.clear();

	delete[] mm_a;
	delete[] mm_b;
	delete[] sst;
	delete[] sst_p;
	delete[] sst_p_draw;
	delete[] avg_totals;
	delete[] channel_p;
	free(buffer);
}

void __stdcall Waveform::GetAudio2(void* buffer, __int64 start, __int64 count, IScriptEnvironment* env) {
	if (start >= 0) {
		resampled_child->GetAudio(buffer, start, count, env);
	} else {
		if (start + count > 0) {
			resampled_child->GetAudio((uint8_t*)buffer - start * bytes_per_sample, 0, start + count, env);
			count = -start;
		}

		memset(buffer, resampled_vi.SampleType() == SAMPLE_INT8 ? 127 : 0, (int)(std::min)(-start, count) * bytes_per_sample);
	}
}

void Waveform::GetSamples() {
	int n_samples;

	std::swap(sst, sst_p);

	if (bres < 0) {
		n_samples = min_samples_per_supersample;
		if (smooth) {
			multiply = multiply1;
			add = add1;
		}
	} else {
		n_samples = min_samples_per_supersample + 1;
		bres -= supersamples_per_pane;
		if (smooth) {
			multiply = multiply2;
			add = add2;
		}
	}

	bres += extras;

	if (stretched) {
		if (n_samples == 0 && getsamples_p != 0) {
			for (int c = 0; c < vi.AudioChannels(); ++c) {
				sst[c] = sst_p[c];
			}
			return;
		} else {
			n_samples = 1;
			multiply = multiply2;
			add = add2;
		}
	}

	if (smooth) {
		switch (resampled_vi.SampleType()) {
			case SAMPLE_INT8: {
				for (int c = 0; c < vi.AudioChannels(); ++c) {
					sst[c].min = ((uint8_t*)buffer)[getsamples_p++];
				}
				for (int s = 1; s < n_samples; ++s) {
					for (int c = 0; c < vi.AudioChannels(); ++c) {
						sst[c].min += ((uint8_t*)buffer)[getsamples_p++];
					}
				}
				for (int c = 0; c < vi.AudioChannels(); ++c) sst[c].min = sst[c].max = (int)(sst[c].min * multiply + add);
			} break;
			case SAMPLE_INT16: {
				for (int c = 0; c < vi.AudioChannels(); ++c) {
					sst[c].min = ((int16_t*)buffer)[getsamples_p++];
				}
				for (int s = 1; s < n_samples; ++s) {
					for (int c = 0; c < vi.AudioChannels(); ++c) {
						sst[c].min += ((int16_t*)buffer)[getsamples_p++];
					}
				}
				for (int c = 0; c < vi.AudioChannels(); ++c) sst[c].min = sst[c].max = (int)(sst[c].min * multiply + add);
			} break;
			case SAMPLE_INT24: {
				for (int c = 0; c < vi.AudioChannels(); ++c) {
					sst[c].min = *(int16_t*)&((uint8_t*)buffer)[++getsamples_p];
					getsamples_p += 2;
				}
				for (int s = 1; s < n_samples; ++s) {
					for (int c = 0; c < vi.AudioChannels(); ++c) {
						sst[c].min += *(int16_t*)&((uint8_t*)buffer)[++getsamples_p];
						getsamples_p += 2;
					}
				}
				for (int c = 0; c < vi.AudioChannels(); ++c) sst[c].min = sst[c].max = (int)(sst[c].min * multiply + add);
			} break;
			case SAMPLE_INT32: {
				for (int c = 0; c < vi.AudioChannels(); ++c) {
					++getsamples_p;
					sst[c].min = ((int16_t*)buffer)[getsamples_p++];
				}
				for (int s = 1; s < n_samples; ++s) {
					for (int c = 0; c < vi.AudioChannels(); ++c) {
						++getsamples_p;
						sst[c].min += ((int16_t*)buffer)[getsamples_p++];
					}
				}
				for (int c = 0; c < vi.AudioChannels(); ++c) sst[c].min = sst[c].max = (int)(sst[c].min * multiply + add);
			} break;
			case SAMPLE_FLOAT: {
				for (int c = 0; c < vi.AudioChannels(); ++c) {
					((f_min_max*)sst)[c].min = ((float*)buffer)[getsamples_p++];
				}
				for (int s = 1; s < n_samples; ++s) {
					for (int c = 0; c < vi.AudioChannels(); ++c) {
						((f_min_max*)sst)[c].min += ((float*)buffer)[getsamples_p++];
					}
				}
				for (int c = 0; c < vi.AudioChannels(); ++c) sst[c].min = sst[c].max = (int)(((f_min_max*)sst)[c].min * multiply + add);
			} break;
		}
	} else {
		switch (resampled_vi.SampleType()) {
			case SAMPLE_INT8: {
				for (int c = 0; c < vi.AudioChannels(); ++c) {
					sst[c].min = sst[c].max = ((uint8_t*)buffer)[getsamples_p++];
				}
				for (int s = 1; s < n_samples; ++s) {
					for (int c = 0; c < vi.AudioChannels(); ++c) {
						int v = ((uint8_t*)buffer)[getsamples_p++];
						if (v < sst[c].min) sst[c].min = v;
						else if (v > sst[c].max) sst[c].max = v;
					}
				}
				for (int c = 0; c < vi.AudioChannels(); ++c) sst[c] = { (int)(sst[c].min * multiply + add), (int)(sst[c].max * multiply + add) };
			} break;
			case SAMPLE_INT16: {
				for (int c = 0; c < vi.AudioChannels(); ++c) {
					sst[c].min = sst[c].max = ((int16_t*)buffer)[getsamples_p++];
				}
				for (int s = 1; s < n_samples; ++s) {
					for (int c = 0; c < vi.AudioChannels(); ++c) {
						int v = ((int16_t*)buffer)[getsamples_p++];
						if (v < sst[c].min) sst[c].min = v;
						else if (v > sst[c].max) sst[c].max = v;
					}
				}
				for (int c = 0; c < vi.AudioChannels(); ++c) sst[c] = { (int)(sst[c].min * multiply + add), (int)(sst[c].max * multiply + add) };
			} break;
			case SAMPLE_INT24: {
				for (int c = 0; c < vi.AudioChannels(); ++c) {
					sst[c].min = sst[c].max = *(int16_t*)&((uint8_t*)buffer)[++getsamples_p];
					getsamples_p += 2;
				}
				for (int s = 1; s < n_samples; ++s) {
					for (int c = 0; c < vi.AudioChannels(); ++c) {
						int v = *(int16_t*)&((uint8_t*)buffer)[++getsamples_p];
						if (v < sst[c].min) sst[c].min = v;
						else if (v > sst[c].max) sst[c].max = v;
						getsamples_p += 2;
					}
				}
				for (int c = 0; c < vi.AudioChannels(); ++c) sst[c] = { (int)(sst[c].min * multiply + add), (int)(sst[c].max * multiply + add) };
			} break;
			case SAMPLE_INT32: {
				for (int c = 0; c < vi.AudioChannels(); ++c) {
					++getsamples_p;
					sst[c].min = sst[c].max = ((int16_t*)buffer)[getsamples_p++];
				}
				for (int s = 1; s < n_samples; ++s) {
					for (int c = 0; c < vi.AudioChannels(); ++c) {
						++getsamples_p;
						int v = ((int16_t*)buffer)[getsamples_p++];
						if (v < sst[c].min) sst[c].min = v;
						else if (v > sst[c].max) sst[c].max = v;
					}
				}
				for (int c = 0; c < vi.AudioChannels(); ++c) sst[c] = { (int)(sst[c].min * multiply + add), (int)(sst[c].max * multiply + add) };
			} break;
			case SAMPLE_FLOAT: {
				for (int c = 0; c < vi.AudioChannels(); ++c) {
					((f_min_max*)sst)[c].min = ((f_min_max*)sst)[c].max = ((float*)buffer)[getsamples_p++];
				}
				for (int s = 1; s < n_samples; ++s) {
					for (int c = 0; c < vi.AudioChannels(); ++c) {
						float v = ((float*)buffer)[getsamples_p++];
						if (v < ((f_min_max*)sst)[c].min) ((f_min_max*)sst)[c].min = v;
						else if (v > ((f_min_max*)sst)[c].max) ((f_min_max*)sst)[c].max = v;
					}
				}
				for (int c = 0; c < vi.AudioChannels(); ++c) sst[c] = { (int)(((f_min_max*)sst)[c].min * multiply + add), (int)(((f_min_max*)sst)[c].max * multiply + add) };
			} break;
		}
	}
}

/***********************************************************************
************************************************************************
* Pane
************************************************************************
***********************************************************************/
void Waveform::Pane(int frame, uint8_t* pane, IScriptEnvironment* env) {
	getsamples_p = 0;

// cycle starts
	if (last_pane == frame - 1) {
		starts[0] = starts[1];
		starts[1] = starts[2];
		starts[2] = starts[3];
		start_sample = end_sample;
	} else {
		starts[0] = resampled_vi.AudioSamplesFromFrames(frame - 1);
		starts[1] = resampled_vi.AudioSamplesFromFrames(frame);
		starts[2] = resampled_vi.AudioSamplesFromFrames(frame + 1);
	}
	starts[3] = resampled_vi.AudioSamplesFromFrames(frame + 2);

// calculate end_sample
	if (!anti_alias) {
		end_sample = starts[2];
	} else {
		int shift = smooth ? 0 : 1;
		int temp_extras = (int)(starts[3] - starts[2]) - nominal_samples_per_frame;
		int temp_bres = temp_extras - (supersamples_per_pane >> 1);
		temp_bres += temp_extras << shift;
		int extras_done = (int)floor((float)(temp_bres - temp_extras) / supersamples_per_pane) + 1;
		end_sample = starts[2] + (min_samples_per_supersample << shift) + extras_done; // ... // plus first two supersamples of next frame
	}

// calculate start_sample and set initial state if required
	if (last_pane != frame - 1) {
		int offset = (!smooth && anti_alias) ? 2 : 1;
		extras = (int)(starts[1] - starts[0]) - nominal_samples_per_frame;
		bres = extras - (supersamples_per_pane >> 1);
		bres += (supersamples_per_pane - offset) * extras;
		int extras_done = (int)floor((float)(bres - extras) / supersamples_per_pane) + 1;
		bres -= extras_done * supersamples_per_pane;
		start_sample = starts[0] + (supersamples_per_pane - offset) * min_samples_per_supersample + extras_done; // calculate position of last two supersamples of previous frame;

		GetAudio2(buffer, start_sample, end_sample - start_sample + stretched, env);

		if (!anti_alias) {
			GetSamples();
			memcpy(sst_p_draw, sst, sizeof(min_max) * vi.AudioChannels());

			extras = (int)(starts[2] - starts[1]) - nominal_samples_per_frame;
			bres = extras - (supersamples_per_pane >> 1);
		} else {
			if (smooth) {
				GetSamples();

				extras = (int)(starts[2] - starts[1]) - nominal_samples_per_frame;
				bres = extras - (supersamples_per_pane >> 1);

				GetSamples();

				for (int c = 0; c < vi.AudioChannels(); ++c) {
					if (sst_p[c].min == sst[c].min) {
						mm_a[c][0] = mm_a[c][1] = mm_a[c][2] = mm_a[c][3] = { sst[c].min, sst[c].min };
					} else {
						int x = (sst_p[c].min * 3 + sst[c].min + 2) >> 2;
						int y = (sst_p[c].min + sst[c].min + 1) >> 1;
						int z = (sst_p[c].min + sst[c].min * 3 + 2) >> 2;

						if (sst[c].min < sst_p[c].min) {
							mm_a[c][0] = { (std::min)(z, sst[c].min + 1), (std::max)(sst_p[c].min - 1, x) };
							mm_a[c][1] = { sst[c].min, (std::max)(x - 1, y) };
							mm_a[c][2] = { sst[c].min, (std::max)(y - 1, z) };
							mm_a[c][3] = { sst[c].min, (std::max)(z - 1, sst[c].min) };
						} else {
							mm_a[c][0] = { (std::min)(sst_p[c].min + 1, x), (std::max)(z, sst[c].min - 1) - 1 };
							mm_a[c][1] = { (std::min)(x + 1, y), sst[c].min };
							mm_a[c][2] = { (std::min)(y + 1, z), sst[c].min };
							mm_a[c][3] = { (std::min)(z + 1, sst[c].min), sst[c].min };
						}
					}
				}
			} else { // not smooth
				GetSamples();
				for (int c = 0; c < vi.AudioChannels(); ++c) {
					mm_a[c][0] = { sst[c].min + 1, sst[c].max - 1 };
				}

				GetSamples();
				for (int c = 0; c < vi.AudioChannels(); ++c) {
					if (sst[c].min <= sst_p[c].min) mm_a[c][0].min = sst[c].min;
					if (sst[c].max >= sst_p[c].max) mm_a[c][0].max = sst[c].max;
					mm_a[c][1] = { sst[c].min + 1, sst[c].max - 1 };
				}

				extras = (int)(starts[2] - starts[1]) - nominal_samples_per_frame;
				bres = extras - (supersamples_per_pane >> 1);

				GetSamples();
				for (int c = 0; c < vi.AudioChannels(); ++c) {
					if (sst[c].min <= sst_p[c].min) {
						mm_a[c][0].min = (std::min)(mm_a[c][0].min, sst[c].min);
						mm_a[c][1].min = sst[c].min;
					}
					if (sst[c].max >= sst_p[c].max) {
						mm_a[c][0].max = (std::max)(mm_a[c][0].max, sst[c].max);
						mm_a[c][1].max = sst[c].max;
					}
					mm_a[c][2] = { sst[c].min + 1, sst[c].max - 1 };
				}

				GetSamples();
				for (int c = 0; c < vi.AudioChannels(); ++c) {
					if (sst[c].min <= sst_p[c].min) {
						mm_a[c][0].min = (std::min)(mm_a[c][0].min, sst[c].min);
						mm_a[c][1].min = (std::min)(mm_a[c][1].min, sst[c].min);
						mm_a[c][2].min = sst[c].min;
					}
					if (sst[c].max >= sst_p[c].max) {
						mm_a[c][0].max = (std::max)(mm_a[c][0].max, sst[c].max);
						mm_a[c][1].max = (std::max)(mm_a[c][1].max, sst[c].max);
						mm_a[c][2].max = sst[c].max;
					}
					mm_a[c][3] = { sst[c].min + 1, sst[c].max - 1 };
				}
			}
		}
	} else {
		GetAudio2(buffer, start_sample, end_sample - start_sample + stretched, env);
	}

// record
	int wp = 0;

	if (!anti_alias) {
		for (int r = 0; r < pixels_per_pane; ++r) {
			GetSamples();
			for (int c = 0; c < vi.AudioChannels(); ++c) {
				if (sst[c].min > sst_p_draw[c].max) {
					((int*)pane)[wp++] = (std::max)(0, sst_p[c].max + 1);
					((int*)pane)[wp++] = (std::min)(subpixel_max_height, sst[c].max) + 1;
					sst_p_draw[c] = { sst_p[c].max + 1, sst[c].max };
				} else if (sst[c].max < sst_p_draw[c].min) {
					((int*)pane)[wp++] = (std::max)(0, sst[c].min);
					((int*)pane)[wp++] = (std::min)(subpixel_max_height + 1, sst_p[c].min);
					sst_p_draw[c] = { sst[c].min, sst_p[c].min - 1 };
				} else {
					((int*)pane)[wp++] = (std::max)(0, sst[c].min);
					((int*)pane)[wp++] = (std::min)(subpixel_max_height, sst[c].max) + 1;
					sst_p_draw[c] = sst[c];
				}
			}
		}

		extras = (int)(starts[3] - starts[2]) - nominal_samples_per_frame;
		bres = extras - (supersamples_per_pane >> 1);
	} else {
		for (int r = 0; r < pixels_per_pane; ++r) {
			if (smooth) {
				if (r == pixels_per_pane - 1) {
					extras = (int)(starts[3] - starts[2]) - nominal_samples_per_frame;
					bres = extras - (supersamples_per_pane >> 1);
				}

				GetSamples();

				for (int c = 0; c < vi.AudioChannels(); ++c) {
					if (sst[c].min != sst_p[c].min) {
						int x = (sst_p[c].min * 3 + sst[c].min + 2) >> 2;
						int y = (sst_p[c].min + sst[c].min + 1) >> 1;
						int z = (sst_p[c].min + sst[c].min * 3 + 2) >> 2;

						if (sst[c].min < sst_p[c].min) { // min problem
							mm_a[c][1] = { (std::min)({ mm_a[c][1].min, x + 1, y }),          (std::max)(mm_a[c][1].max, sst[c].min) };
							mm_a[c][2] = { (std::min)({ mm_a[c][2].min, y + 1, z }),          (std::max)(mm_a[c][2].max, sst[c].min) };
							mm_a[c][3] = { (std::min)({ mm_a[c][3].min, z + 1, sst[c].min }), (std::max)(mm_a[c][3].max, sst[c].min) };
							mm_b[c][0] = { (std::min)(z, sst[c].min + 1), (std::max)(sst_p[c].min - 1, x) };
							mm_b[c][1] = { sst[c].min, (std::max)(x - 1, y) };
							mm_b[c][2] = { sst[c].min, (std::max)(y - 1, z) };
							mm_b[c][3] = { sst[c].min, (std::max)(z - 1, sst[c].min) };
						} else if (sst[c].min > sst_p[c].min) { // max problem
							mm_a[c][1] = { (std::min)(mm_a[c][1].min, sst[c].min), (std::max)({ mm_a[c][1].max, x - 1, y }) };
							mm_a[c][2] = { (std::min)(mm_a[c][2].min, sst[c].min), (std::max)({ mm_a[c][2].max, y - 1, z }) };
							mm_a[c][3] = { (std::min)(mm_a[c][3].min, sst[c].min), (std::max)({ mm_a[c][3].max, z - 1, sst[c].min }) };
							mm_b[c][0] = { (std::min)(sst_p[c].min + 1, x), (std::max)(z, sst[c].min - 1) };
							mm_b[c][1] = { (std::min)(x + 1, y), sst[c].min };
							mm_b[c][2] = { (std::min)(y + 1, z), sst[c].min };
							mm_b[c][3] = { (std::min)(z + 1, sst[c].min), sst[c].min };
						}
					} else {
						mm_b[c][0] = mm_b[c][1] = mm_b[c][2] = { sst[c].min, sst[c].min };
						mm_b[c][3] = { sst[c].min + 1, sst[c].min - 1 };
					}
				}
			} else {
				GetSamples();
				for (int c = 0; c < vi.AudioChannels(); ++c) {
					if (sst[c].min <= sst_p[c].min) {
						mm_a[c][0].min = (std::min)(mm_a[c][0].min, sst[c].min);
						mm_a[c][1].min = (std::min)(mm_a[c][1].min, sst[c].min);
						mm_a[c][2].min = (std::min)(mm_a[c][2].min, sst[c].min);
						mm_a[c][3].min = sst[c].min;
					}
					if (sst[c].max >= sst_p[c].max) {
						mm_a[c][0].max = (std::max)(mm_a[c][0].max, sst[c].max);
						mm_a[c][1].max = (std::max)(mm_a[c][1].max, sst[c].max);
						mm_a[c][2].max = (std::max)(mm_a[c][2].max, sst[c].max);
						mm_a[c][3].max = sst[c].max;
					}
					mm_b[c][0] = { sst[c].min + 1, sst[c].max - 1 };
				}

				GetSamples();
				for (int c = 0; c < vi.AudioChannels(); ++c) {
					if (sst[c].min <= sst_p[c].min) {
						mm_a[c][1].min = (std::min)(mm_a[c][1].min, sst[c].min);
						mm_a[c][2].min = (std::min)(mm_a[c][2].min, sst[c].min);
						mm_a[c][3].min = (std::min)(mm_a[c][3].min, sst[c].min);
						mm_b[c][0].min = sst[c].min;
					}
					if (sst[c].max >= sst_p[c].max) {
						mm_a[c][1].max = (std::max)(mm_a[c][1].max, sst[c].max);
						mm_a[c][2].max = (std::max)(mm_a[c][2].max, sst[c].max);
						mm_a[c][3].max = (std::max)(mm_a[c][3].max, sst[c].max);
						mm_b[c][0].max = sst[c].max;
					}
					mm_b[c][1] = { sst[c].min + 1, sst[c].max - 1 };
				}

				if (r == pixels_per_pane - 1) {
					extras = (int)(starts[3] - starts[2]) - nominal_samples_per_frame;
					bres = extras - (supersamples_per_pane >> 1);
				}

				GetSamples();
				for (int c = 0; c < vi.AudioChannels(); ++c) {
					if (sst[c].min <= sst_p[c].min) {
						mm_a[c][2].min = (std::min)(mm_a[c][2].min, sst[c].min);
						mm_a[c][3].min = (std::min)(mm_a[c][3].min, sst[c].min);
						mm_b[c][0].min = (std::min)(mm_b[c][0].min, sst[c].min);
						mm_b[c][1].min = sst[c].min;
					}
					if (sst[c].max >= sst_p[c].max) {
						mm_a[c][2].max = (std::max)(mm_a[c][2].max, sst[c].max);
						mm_a[c][3].max = (std::max)(mm_a[c][3].max, sst[c].max);
						mm_b[c][0].max = (std::max)(mm_b[c][0].max, sst[c].max);
						mm_b[c][1].max = sst[c].max;
					}
					mm_b[c][2] = { sst[c].min + 1, sst[c].max - 1 };
				}

				GetSamples();
				for (int c = 0; c < vi.AudioChannels(); ++c) {
					if (sst[c].min <= sst_p[c].min) {
						mm_a[c][3].min = (std::min)(mm_a[c][3].min, sst[c].min);
						mm_b[c][0].min = (std::min)(mm_b[c][0].min, sst[c].min);
						mm_b[c][1].min = (std::min)(mm_b[c][1].min, sst[c].min);
						mm_b[c][2].min = sst[c].min;
					}
					if (sst[c].max >= sst_p[c].max) {
						mm_a[c][3].max = (std::max)(mm_a[c][3].max, sst[c].max);
						mm_b[c][0].max = (std::max)(mm_b[c][0].max, sst[c].max);
						mm_b[c][1].max = (std::max)(mm_b[c][1].max, sst[c].max);
						mm_b[c][2].max = sst[c].max;
					}
					mm_b[c][3] = { sst[c].min + 1, sst[c].max - 1 };
				}
			}

			for (int c = 0; c < vi.AudioChannels(); ++c) {
				mm_a[c][0].min = (std::max)(0, mm_a[c][0].min);
				mm_a[c][1].min = (std::max)(0, mm_a[c][1].min);
				mm_a[c][2].min = (std::max)(0, mm_a[c][2].min);
				mm_a[c][3].min = (std::max)(0, mm_a[c][3].min);

				mm_a[c][0].max = (std::min)(mm_a[c][0].max, subpixel_max_height);
				mm_a[c][1].max = (std::min)(mm_a[c][1].max, subpixel_max_height);
				mm_a[c][2].max = (std::min)(mm_a[c][2].max, subpixel_max_height);
				mm_a[c][3].max = (std::min)(mm_a[c][3].max, subpixel_max_height);

				int _min = (std::max)(0, (std::min)({ mm_a[c][0].min, mm_a[c][1].min, mm_a[c][2].min, mm_a[c][3].min })) & 0xfffc;

				*(uint32_t*)&pane[wp] = _min >> 2;
				wp += 4;

				mm_a[c][0].min -= _min;
				mm_a[c][1].min -= _min;
				mm_a[c][2].min -= _min;
				mm_a[c][3].min -= _min;
				mm_a[c][0].max -= _min;
				mm_a[c][1].max -= _min;
				mm_a[c][2].max -= _min;
				mm_a[c][3].max -= _min;

				while (true) {
					int t = 0;
					for (int s = 0; s < 4; ++s) {
						if (mm_a[c][s].max < mm_a[c][s].min) continue;
						if (mm_a[c][s].min > 3) {
							mm_a[c][s].min -= 4;
							mm_a[c][s].max -= 4;
						} else if (mm_a[c][s].min >= 0) {
							t += 4 - mm_a[c][s].min;
							mm_a[c][s].min -= 4;
							mm_a[c][s].max -= 4;
						} else if (mm_a[c][s].max > 0) {
							t += 4;
							mm_a[c][s].max -= 4;
						} else if (mm_a[c][s].max > -4) {
							t += mm_a[c][s].max + 4;
							mm_a[c][s].max -= 4;
						}
					}

					pane[wp++] = t;
					if (!t) break;
				}
			}

			std::swap(mm_a, mm_b);
		} // end loop
	}

	last_pane = frame;
}

/***********************************************************************
************************************************************************
* GetFrame
************************************************************************
***********************************************************************/
PVideoFrame __stdcall Waveform::GetFrame(int n, IScriptEnvironment* env) {
	PVideoFrame src = child->GetFrame(n, env);
	PVideoFrame dst = src->IsWritable() ? NULL : (GetVersion() >= 8 ? env->NewVideoFrameP(vi, &src) : env->NewVideoFrame(vi));

	__m128 half = _mm_set1_ps(0.5f);
	BYTE* waveform_p = NULL;
	int waveform_pitch;

/***********************************************************************
* Update panes
***********************************************************************/
	if (window) {
		int diff = n - last_frame;

		auto start_pane = panes.begin();
		auto end_pane = panes.end();
		int _n = n - window;

		if (diff != 0 && window) {
			if (diff > 0 && diff < (int)panes.size()) {
				std::rotate(panes.begin(), panes.begin() + diff, panes.end());
				start_pane = end_pane - diff;
				_n += ((int)panes.size() - diff);
			} else if (diff < 0 && diff > -(int)panes.size()) {
				std::rotate(panes.begin(), panes.end() + diff, panes.end());
				end_pane = start_pane - diff;
			}
		}

		for (auto p = start_pane; p < end_pane; ++p) {
			Pane(_n++, *p, env);
		}
	} else {
		Pane(n, panes[0], env);
	}

/***********************************************************************
* Copy/darken planes
***********************************************************************/
	for (auto plane : planes) {
		int type = (vi.BitsPerComponent() == 32) ? 1 : (vi.IsRGB32() || vi.IsRGB64()) ? 2 : (plane == PLANAR_U || plane == PLANAR_V) ? 3 : (plane == PLANAR_A) ? 5 : 4;
		bool uva = (plane == PLANAR_U || plane == PLANAR_V || plane == PLANAR_A);

		int width_sub, height_sub;
		if (plane == PLANAR_U || plane == PLANAR_V) {
			width_sub = vi.GetPlaneWidthSubsampling(plane);
			height_sub = vi.GetPlaneHeightSubsampling(plane);
		} else {
			width_sub = height_sub = 0;
		}

		int y = 0;
		int height = vi.height >> height_sub;

		BYTE* src_p = (BYTE*)src->GetReadPtr(plane);
		int src_pitch = src->GetPitch(plane);

		BYTE* dst_p;
		int dst_pitch;

		if (!plane) { // interleaved RGB is upside down
			src_p += (height - 1) * src_pitch;
			src_pitch = -src_pitch;
		}

		int cut_off = (plane == PLANAR_A || under) ? height : (vi.height - total_waveform_height) >> height_sub;

// copy
		if (dst) {
			int copy_bytes = vi.BytesFromPixels(vi.width >> width_sub);

			dst_p = dst->GetWritePtr(plane);
			dst_pitch = dst->GetPitch(plane);

			if (!plane) { // interleaved RGB is upside down
				dst_p += (height - 1) * dst_pitch;
				dst_pitch = -dst_pitch;
			}

			if (!uva) {
				waveform_p = dst_p + waveform_y * dst_pitch;
				waveform_pitch = dst_pitch;
			}

			for (y = 0; y < cut_off; ++y) {
				memcpy(dst_p, src_p, copy_bytes);
				src_p += src_pitch;
				dst_p += dst_pitch;
			}
		} else {
			if (!uva) {
				waveform_p = src_p + waveform_y * src_pitch;
				waveform_pitch = src_pitch;
			}

			src_p += src_pitch * cut_off;
			dst_p = src_p;
			dst_pitch = src_pitch;
			y = cut_off;
		}

		int m128_pitch = abs(dst_pitch) >> 4;

// darken
		for (; y < height; ++y) {
			switch (type) {
				case 1: { // float
					for (int x = 0; x < m128_pitch; ++x) {
						((__m128*)dst_p)[x] = _mm_mul_ps(((__m128*)src_p)[x], half);
					}
				} break;
				case 2: { // rgb32/rgb64
					for (int x = 0; x < m128_pitch; ++x) {
						__m128i a = ((__m128i*)src_p)[x];
						((__m128i*)dst_p)[x] = _mm_or_si128(_mm_and_si128(_mm_srli_epi32(a, 1), darken_mask), _mm_and_si128(a, alpha_mask));
					}
				} break;
				case 3: { // PLANAR_U/PLANAR_V
					for (int x = 0; x < m128_pitch; ++x) {
						((__m128i*)dst_p)[x] = _mm_add_epi32(_mm_and_si128(_mm_srli_epi32(((__m128i*)src_p)[x], 1), darken_mask), quarter_i);
					}
				} break;
				case 4: { // other
					for (int x = 0; x < m128_pitch; ++x) {
						((__m128i*)dst_p)[x] = _mm_and_si128(_mm_srli_epi32(((__m128i*)src_p)[x], 1), darken_mask);
					}
				} break;
			}

			src_p += src_pitch;
			dst_p += dst_pitch;
		}

		if (!uva) {
// horizontal separators
			int m128_pitch = abs(waveform_pitch) >> 4;

			if (vi.BitsPerComponent() == 32) {
				__m128* row = (__m128*)(waveform_p + (channel_height - vi.IsPlanar()) * waveform_pitch);
				for (int c = 0; c < vi.AudioChannels() - 1; ++c) {
					for (int x = 0; x < m128_pitch << 1; ++x) {
						row[x] = _mm_add_ps(row[x], eighth);
					}
					row += m128_pitch * channel_height;
				}
			} else {
				__m128i* row = (__m128i*)(waveform_p + (channel_height - vi.IsPlanar()) * waveform_pitch);
				for (int c = 0; c < vi.AudioChannels() - 1; ++c) {
					for (int x = 0; x < m128_pitch << 1; ++x) {
						row[x] = _mm_add_epi32(row[x], eighth_i);
					}
					row += (plane == 0 ? -m128_pitch : m128_pitch) * channel_height;
				}
			}

// vertical separators
			if (window) {
				uint8_t* row = waveform_p;
				switch (vi.BitsPerComponent()) {
					case 32: {
						for (int y = 0; y < total_waveform_height; ++y) {
							for (auto col : columns) {
								((float*)row)[col] += 0.125;
							}
							row += waveform_pitch;
						}
					} break;
					case 8: {
						for (int y = 0; y < total_waveform_height; ++y) {
							for (auto col : columns) {
								((uint8_t*)row)[col] += eighth_int32;
							}
							row += waveform_pitch;
						}
					} break;
					default: {
						for (int y = 0; y < total_waveform_height; ++y) {
							for (auto col : columns) {
								((uint16_t*)row)[col] += eighth_int32;
							}
							row += waveform_pitch;
						}
					}
				}
			}

/***********************************************************************
* Draw
***********************************************************************/
			for (int c = 0; c < vi.AudioChannels(); ++c) {
				channel_p[c] = (uint8_t*)waveform_p + (channel_height * (c + 1) - 1) * waveform_pitch;
			}

			int xx = 0;

			for (int p = 0; p < (int)panes.size(); ++p) {
				int rp = 0;

				int ppp = pixels_per_pane;
				if (p == 0) {
					ppp -= left_offset;
					if (!anti_alias) {
						rp += (left_offset << 1) * vi.AudioChannels();
					} else {
						for (int l = left_offset*vi.AudioChannels(); l > 0; --l) {
							rp += 4;
							while (panes[p][rp++]);
						}
					}
				} else {
					if (p == panes.size() - 1) ppp -= right_missing;
				}

// interleaved RGB
				if (!vi.IsPlanar()) {
					const int step = vi.NumComponents();

					if (!anti_alias) {
						switch (vi.BitsPerComponent()) {
							case 8: {
								uint8_t col = (p == window) ? 0xff : 0x90;
								for (int x = 0; x < ppp; ++x) {
									for (int c = 0; c < vi.AudioChannels(); ++c) {
										int sy = ((int*)panes[p])[rp++];
										int ey = ((int*)panes[p])[rp++];
										uint8_t* pointer = channel_p[c] - sy * waveform_pitch;
										for (int y = sy; y < ey; ++y) {
											pointer[xx + 0] = col;
											pointer[xx + 1] = col;
											pointer[xx + 2] = col;
											pointer -= waveform_pitch;
										}
									}
									xx += step;
								}
							} break;
							case 16: {
								uint16_t col = (p == window) ? 0xffff : 0x9090;
								for (int x = 0; x < ppp; ++x) {
									for (int c = 0; c < vi.AudioChannels(); ++c) {
										int sy = ((int*)panes[p])[rp++];
										int ey = ((int*)panes[p])[rp++];
										uint8_t* pointer = channel_p[c] - sy * waveform_pitch;
										for (int y = sy; y < ey; ++y) {
											((uint16_t*)pointer)[xx + 0] = col;
											((uint16_t*)pointer)[xx + 1] = col;
											((uint16_t*)pointer)[xx + 2] = col;
											pointer -= waveform_pitch;
										}
									}
									xx += step;
								}
							} break;
						}
					} else {
						auto lookup = (p == window) ? white_lookup : grey_lookup;
						switch (vi.BitsPerComponent()) {
							case 8: {
								for (int x = 0; x < ppp; ++x) {
									for (int c = 0; c < vi.AudioChannels(); ++c) {
										int y = *(uint32_t*)&panes[p][rp];
										rp += 4;
										uint8_t* pointer = channel_p[c] - y * waveform_pitch;
										while (int g = panes[p][rp++]) {
											pointer[xx + 0] = lookup[(g << 8) | pointer[xx + 0]].i;
											pointer[xx + 1] = lookup[(g << 8) | pointer[xx + 1]].i;
											pointer[xx + 2] = lookup[(g << 8) | pointer[xx + 2]].i;
											pointer -= waveform_pitch;
										}
									}
									xx += step;
								}
							} break;
							case 16: {
								for (int x = 0; x < ppp; ++x) {
									for (int c = 0; c < vi.AudioChannels(); ++c) {
										int y = *(uint32_t*)&panes[p][rp];
										rp += 4;
										uint8_t* pointer = channel_p[c] - y * waveform_pitch;
										while (int g = panes[p][rp++]) {
											((uint16_t*)pointer)[xx + 0] = lookup[(g << 8) | (((uint16_t*)pointer)[xx + 0] >> 8)].i;
											((uint16_t*)pointer)[xx + 1] = lookup[(g << 8) | (((uint16_t*)pointer)[xx + 1] >> 8)].i;
											((uint16_t*)pointer)[xx + 2] = lookup[(g << 8) | (((uint16_t*)pointer)[xx + 2] >> 8)].i;
											pointer -= waveform_pitch;
										}
									}
									xx += step;
								}
							} break;
						}
					}
				} else { // planar formats
					if (!anti_alias) {
						switch (vi.BitsPerComponent()) {
							case 8: {
								uint8_t col = (p == window) ? 0xff : 0x90;
								for (int x = 0; x < ppp; ++x) {
									for (int c = 0; c < vi.AudioChannels(); ++c) {
										int sy = ((int*)panes[p])[rp++];
										int ey = ((int*)panes[p])[rp++];
										uint8_t* pointer = channel_p[c] - sy * waveform_pitch;
										for (int y = sy; y < ey; ++y) {
											pointer[xx] = col;
											pointer -= waveform_pitch;
										}
									}
									++xx;
								}
							} break;
							case 10:
							case 12:
							case 14:
							case 16: {
								uint16_t col = ((p == window) ? 0xffff : 0x9090) >> (16 - vi.BitsPerComponent());
								for (int x = 0; x < ppp; ++x) {
									for (int c = 0; c < vi.AudioChannels(); ++c) {
										int sy = ((int*)panes[p])[rp++];
										int ey = ((int*)panes[p])[rp++];
										uint8_t* pointer = channel_p[c] - sy * waveform_pitch;
										for (int y = sy; y < ey; ++y) {
											((uint16_t*)pointer)[xx] = col;
											pointer -= waveform_pitch;
										}
									}
									++xx;
								}
							} break;
							case 32: {
								float col = (p == window) ? 1 : 0.5625f;
								for (int x = 0; x < ppp; ++x) {
									for (int c = 0; c < vi.AudioChannels(); ++c) {
										int sy = ((int*)panes[p])[rp++];
										int ey = ((int*)panes[p])[rp++];
										uint8_t* pointer = channel_p[c] - sy * waveform_pitch;
										for (int y = sy; y < ey; ++y) {
											((float*)pointer)[xx] = col;
											pointer -= waveform_pitch;
										}
									}
									++xx;
								}
							}
						}
					} else {
						auto lookup = (p == window) ? white_lookup : grey_lookup;
						switch (vi.BitsPerComponent()) {
							case 8: {
								for (int x = 0; x < ppp; ++x) {
									for (int c = 0; c < vi.AudioChannels(); ++c) {
										int y = *(uint32_t*)&panes[p][rp];
										rp += 4;
										uint8_t* pointer = channel_p[c] - y * waveform_pitch;
										while (int g = panes[p][rp++]) {
											int c = pointer[xx];
											pointer[xx] = lookup[(g << 8) | c].i;
											pointer -= waveform_pitch;
										}
									}
									++xx;
								}
							} break;
							case 16: {
								for (int x = 0; x < ppp; ++x) {
									for (int c = 0; c < vi.AudioChannels(); ++c) {
										int y = *(uint32_t*)&panes[p][rp];
										rp += 4;
										uint8_t* pointer = channel_p[c] - y * waveform_pitch;
										while (int g = panes[p][rp++]) {
											uint16_t c = ((uint16_t*)pointer)[xx] >> 8;
											((uint16_t*)pointer)[xx] = lookup[(g << 8) | c].i;
											pointer -= waveform_pitch;
										}
									}
									++xx;
								}
							} break;
							case 32: {
								for (int x = 0; x < ppp; ++x) {
									for (int c = 0; c < vi.AudioChannels(); ++c) {
										int y = *(uint32_t*)&panes[p][rp];
										rp += 4;
										uint8_t* pointer = channel_p[c] - y * waveform_pitch;
										while (int g = panes[p][rp++]) {
											uint8_t c = std::clamp((int)((float*)pointer)[xx], 0, 255);
											((float*)pointer)[xx] = lookup[(g << 8) | c].f;
											pointer -= waveform_pitch;
										}
									}
									++xx;
								}
							} break;
							default: {
								int shift_down = vi.BitsPerComponent() - 8;
								for (int x = 0; x < ppp; ++x) {
									for (int c = 0; c < vi.AudioChannels(); ++c) {
										int y = *(uint32_t*)&panes[p][rp];
										rp += 4;
										uint8_t* pointer = channel_p[c] - y * waveform_pitch;
										while (int g = panes[p][rp++]) {
											uint8_t c = ((uint16_t*)pointer)[xx] >> shift_down;
											((uint16_t*)pointer)[xx] = lookup[(g << 8) | (std::min)(c, (uint8_t)255)].i;
											pointer -= waveform_pitch;
										}
									}
									++xx;
								}
							}
						}
					}
				}
			}
		}
	}

	last_frame = n;

	return dst ? dst : src;
}

AVSValue __cdecl CreateWaveform(AVSValue args, void* user_data, IScriptEnvironment* env) {
	PClip clip = args[0].AsClip();

	if (!clip->GetVideoInfo().AudioChannels()) {
		AVSValue args[4] = { clip, "Waveform: No audio", 5, 0x00ffffff };
		const char* names[4] = { 0, 0, "align", "text_color" };
		AVSValue out = env->Invoke("Subtitle", AVSValue(args, 4), names).AsClip();
		return out;
	}

	bool yuy2 = clip->GetVideoInfo().IsYUY2();

	if (yuy2) clip = env->Invoke("ConverttoYV16", clip).AsClip();

	AVSValue out = new Waveform(
		clip,
		args[1].AsFloatf(0),
		args[2].AsFloatf(1.0f / 3),
		args[3].AsFloatf(1),
		args[4].AsBool(false),
		args[5].AsBool(false),
		args[6].AsBool(true),
		env
	);

	if (yuy2) out = env->Invoke("ConverttoYUY2", out.AsClip()).AsClip();

	return out;
}

/*#ifndef AVS_LINKAGE_DLLIMPORT
const AVS_Linkage *AVS_linkage = 0;
#endif

extern "C" __declspec(dllexport) const char* __stdcall AvisynthPluginInit3(IScriptEnvironment* env, const AVS_Linkage* const vectors) {
#ifndef AVS_LINKAGE_DLLIMPORT
	AVS_linkage = vectors;
#endif
	env->AddFunction("Waveform", "c[window]f[height]f[zoom]f[under]b[smooth]b[aa]b", CreateWaveform, 0);
	env->AddFunction("GetAudio", "c", CreateGetAudio, 0);

	return "Waveform: Audio waveform display plugin";
}*/
