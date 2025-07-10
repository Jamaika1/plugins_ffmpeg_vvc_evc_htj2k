#include <stddef.h>
#include "vulkan_source.h"

const char *ff_source_common_comp = "\
layout(buffer_reference, buffer_reference_align = 1) buffer u8buf {\n\
    uint8_t v;\n\
};\n\
\n\
layout(buffer_reference, buffer_reference_align = 1) buffer u8vec2buf {\n\
    u8vec2 v;\n\
};\n\
\n\
layout(buffer_reference, buffer_reference_align = 1) buffer u8vec4buf {\n\
    u8vec4 v;\n\
};\n\
\n\
layout(buffer_reference, buffer_reference_align = 2) buffer u16buf {\n\
    uint16_t v;\n\
};\n\
\n\
layout(buffer_reference, buffer_reference_align = 4) buffer u32buf {\n\
    uint32_t v;\n\
};\n\
\n\
layout(buffer_reference, buffer_reference_align = 4) buffer u32vec2buf {\n\
    u32vec2 v;\n\
};\n\
\n\
layout(buffer_reference, buffer_reference_align = 8) buffer u64buf {\n\
    uint64_t v;\n\
};\n\
\n\
#define OFFBUF(type, b, l) \\\n\
    type(uint64_t(b) + uint64_t(l))\n\
\n\
#define zero_extend(a, p) \\\n\
    ((a) & ((1 << (p)) - 1))\n\
\n\
#define sign_extend(val, bits) \\\n\
    bitfieldExtract(val, 0, bits)\n\
\n\
#define fold(diff, bits) \\\n\
    sign_extend(diff, bits)\n\
\n\
#define mid_pred(a, b, c) \\\n\
    max(min((a), (b)), min(max((a), (b)), (c)))\n\
\n\
/* TODO: optimize */\n\
uint align(uint src, uint a)\n\
{\n\
    uint res = src % a;\n\
    if (res == 0)\n\
        return src;\n\
    return src + a - res;\n\
}\n\
\n\
/* TODO: optimize */\n\
uint64_t align64(uint64_t src, uint64_t a)\n\
{\n\
    uint64_t res = src % a;\n\
    if (res == 0)\n\
        return src;\n\
    return src + a - res;\n\
}\n\
\n\
#define reverse4(src) \\\n\
    (pack32(unpack8(uint32_t(src)).wzyx))\n\
\n\
u32vec2 reverse8(uint64_t src)\n\
{\n\
    u32vec2 tmp = unpack32(src);\n\
    tmp.x = reverse4(tmp.x);\n\
    tmp.y = reverse4(tmp.y);\n\
    return tmp.yx;\n\
}\n\
\n\
#ifdef PB_32\n\
#define BIT_BUF_TYPE uint32_t\n\
#define BUF_TYPE u32buf\n\
#define BUF_REVERSE(src) reverse4(src)\n\
#define BUF_BITS uint8_t(32)\n\
#define BUF_BYTES uint8_t(4)\n\
#define BYTE_EXTRACT(src, byte_off) \\\n\
    (uint8_t(bitfieldExtract((src), ((byte_off) << 3), 8)))\n\
#else\n\
#define BIT_BUF_TYPE uint64_t\n\
#define BUF_TYPE u32vec2buf\n\
#define BUF_REVERSE(src) reverse8(src)\n\
#define BUF_BITS uint8_t(64)\n\
#define BUF_BYTES uint8_t(8)\n\
#define BYTE_EXTRACT(src, byte_off) \\\n\
    (uint8_t(((src) >> ((byte_off) << 3)) & 0xFF))\n\
#endif\n\
\n\
struct PutBitContext {\n\
    uint64_t buf_start;\n\
    uint64_t buf;\n\
\n\
    BIT_BUF_TYPE bit_buf;\n\
    uint8_t bit_left;\n\
};\n\
\n\
void put_bits(inout PutBitContext pb, const uint32_t n, uint32_t value)\n\
{\n\
    if (n < pb.bit_left) {\n\
        pb.bit_buf = (pb.bit_buf << n) | value;\n\
        pb.bit_left -= uint8_t(n);\n\
    } else {\n\
        pb.bit_buf <<= pb.bit_left;\n\
        pb.bit_buf |= (value >> (n - pb.bit_left));\n\
\n\
#ifdef PB_UNALIGNED\n\
        u8buf bs = u8buf(pb.buf);\n\
        [[unroll]]\n\
        for (uint8_t i = uint8_t(0); i < BUF_BYTES; i++)\n\
            bs[i].v = BYTE_EXTRACT(pb.bit_buf, BUF_BYTES - uint8_t(1) - i);\n\
#else\n\
#ifdef DEBUG\n\
        if ((pb.buf % BUF_BYTES) != 0)\n\
            debugPrintfEXT(\"put_bits buffer is not aligned!\");\n\
#endif\n\
\n\
        BUF_TYPE bs = BUF_TYPE(pb.buf);\n\
        bs.v = BUF_REVERSE(pb.bit_buf);\n\
#endif\n\
        pb.buf = uint64_t(bs) + BUF_BYTES;\n\
\n\
        pb.bit_left += BUF_BITS - uint8_t(n);\n\
        pb.bit_buf = value;\n\
    }\n\
}\n\
\n\
uint32_t flush_put_bits(inout PutBitContext pb)\n\
{\n\
    /* Align bits to MSBs */\n\
    if (pb.bit_left < BUF_BITS)\n\
        pb.bit_buf <<= pb.bit_left;\n\
\n\
    if (pb.bit_left < BUF_BITS) {\n\
        uint to_write = ((BUF_BITS - pb.bit_left - 1) >> 3) + 1;\n\
\n\
        u8buf bs = u8buf(pb.buf);\n\
        for (int i = 0; i < to_write; i++)\n\
            bs[i].v = BYTE_EXTRACT(pb.bit_buf, BUF_BYTES - uint8_t(1) - i);\n\
        pb.buf = uint64_t(bs) + to_write;\n\
    }\n\
\n\
    pb.bit_left = BUF_BITS;\n\
    pb.bit_buf = 0x0;\n\
\n\
    return uint32_t(pb.buf - pb.buf_start);\n\
}\n\
\n\
void init_put_bits(out PutBitContext pb, u8buf data, uint64_t len)\n\
{\n\
    pb.buf_start = uint64_t(data);\n\
    pb.buf = uint64_t(data);\n\
\n\
    pb.bit_buf = 0;\n\
    pb.bit_left = BUF_BITS;\n\
}\n\
\n\
uint64_t put_bits_count(in PutBitContext pb)\n\
{\n\
    return (pb.buf - pb.buf_start)*8 + BUF_BITS - pb.bit_left;\n\
}\n\
\n\
uint32_t put_bytes_count(in PutBitContext pb)\n\
{\n\
    uint64_t num_bytes = (pb.buf - pb.buf_start) + ((BUF_BITS - pb.bit_left) >> 3);\n\
    return uint32_t(num_bytes);\n\
}\n\
\n\
struct GetBitContext {\n\
    uint64_t buf_start;\n\
    uint64_t buf;\n\
    uint64_t buf_end;\n\
\n\
    uint64_t bits;\n\
    uint bits_valid;\n\
    uint size_in_bits;\n\
};\n\
\n\
#define LOAD64()                                       \\\n\
    {                                                  \\\n\
        u8vec4buf ptr = u8vec4buf(gb.buf);             \\\n\
        uint32_t rf1 = pack32((ptr[0].v).wzyx);        \\\n\
        uint32_t rf2 = pack32((ptr[1].v).wzyx);        \\\n\
        gb.buf += 8;                                   \\\n\
        gb.bits = uint64_t(rf1) << 32 | uint64_t(rf2); \\\n\
        gb.bits_valid = 64;                            \\\n\
    }\n\
\n\
#define RELOAD32()                                                \\\n\
    {                                                             \\\n\
        u8vec4buf ptr = u8vec4buf(gb.buf);                        \\\n\
        uint32_t rf = pack32((ptr[0].v).wzyx);                    \\\n\
        gb.buf += 4;                                              \\\n\
        gb.bits = uint64_t(rf) << (32 - gb.bits_valid) | gb.bits; \\\n\
        gb.bits_valid += 32;                                      \\\n\
    }\n\
\n\
void init_get_bits(inout GetBitContext gb, u8buf data, uint64_t len)\n\
{\n\
    gb.buf = gb.buf_start = uint64_t(data);\n\
    gb.buf_end = uint64_t(data) + len;\n\
    gb.size_in_bits = uint(len) * 8;\n\
\n\
    /* Preload */\n\
    LOAD64()\n\
}\n\
\n\
bool get_bit(inout GetBitContext gb)\n\
{\n\
    if (gb.bits_valid == 0)\n\
        LOAD64()\n\
\n\
    bool val = bool(gb.bits >> (64 - 1));\n\
    gb.bits <<= 1;\n\
    gb.bits_valid--;\n\
    return val;\n\
}\n\
\n\
uint get_bits(inout GetBitContext gb, uint n)\n\
{\n\
    if (n == 0)\n\
        return 0;\n\
\n\
    if (n > gb.bits_valid)\n\
        RELOAD32()\n\
\n\
    uint val = uint(gb.bits >> (64 - n));\n\
    gb.bits <<= n;\n\
    gb.bits_valid -= n;\n\
    return val;\n\
}\n\
\n\
uint show_bits(inout GetBitContext gb, uint n)\n\
{\n\
    if (n > gb.bits_valid)\n\
        RELOAD32()\n\
\n\
    return uint(gb.bits >> (64 - n));\n\
}\n\
\n\
void skip_bits(inout GetBitContext gb, uint n)\n\
{\n\
    if (n > gb.bits_valid)\n\
        RELOAD32()\n\
\n\
    gb.bits <<= n;\n\
    gb.bits_valid -= n;\n\
}\n\
\n\
uint tell_bits(in GetBitContext gb)\n\
{\n\
    return uint(gb.buf - gb.buf_start) * 8 - gb.bits_valid;\n\
}\n\
\n\
uint left_bits(in GetBitContext gb)\n\
{\n\
    return gb.size_in_bits - uint(gb.buf - gb.buf_start) * 8 + gb.bits_valid;\n\
}";

const char *ff_source_rangecoder_comp = "\
struct RangeCoder {\n\
    uint64_t bytestream_start;\n\
    uint64_t bytestream;\n\
    uint64_t bytestream_end;\n\
\n\
    int low;\n\
    int range;\n\
    uint16_t outstanding_count;\n\
    uint8_t outstanding_byte;\n\
};\n\
\n\
#ifdef FULL_RENORM\n\
/* Full renorm version that can handle outstanding_byte == 0xFF */\n\
void renorm_encoder(inout RangeCoder c)\n\
{\n\
    int bs_cnt = 0;\n\
    u8buf bytestream = u8buf(c.bytestream);\n\
\n\
    if (c.outstanding_byte == 0xFF) {\n\
        c.outstanding_byte = uint8_t(c.low >> 8);\n\
    } else if (c.low <= 0xFF00) {\n\
        bytestream[bs_cnt++].v = c.outstanding_byte;\n\
        uint16_t cnt = c.outstanding_count;\n\
        for (; cnt > 0; cnt--)\n\
            bytestream[bs_cnt++].v = uint8_t(0xFF);\n\
        c.outstanding_count = uint16_t(0);\n\
        c.outstanding_byte = uint8_t(c.low >> 8);\n\
    } else if (c.low >= 0x10000) {\n\
        bytestream[bs_cnt++].v = c.outstanding_byte + uint8_t(1);\n\
        uint16_t cnt = c.outstanding_count;\n\
        for (; cnt > 0; cnt--)\n\
            bytestream[bs_cnt++].v = uint8_t(0x00);\n\
        c.outstanding_count = uint16_t(0);\n\
        c.outstanding_byte = uint8_t(bitfieldExtract(c.low, 8, 8));\n\
    } else {\n\
        c.outstanding_count++;\n\
    }\n\
\n\
    c.bytestream += bs_cnt;\n\
    c.range <<= 8;\n\
    c.low = bitfieldInsert(0, c.low, 8, 8);\n\
}\n\
\n\
#else\n\
\n\
/* Cannot deal with outstanding_byte == -1 in the name of speed */\n\
void renorm_encoder(inout RangeCoder c)\n\
{\n\
    uint16_t oc = c.outstanding_count + uint16_t(1);\n\
    int low = c.low;\n\
\n\
    c.range <<= 8;\n\
    c.low = bitfieldInsert(0, low, 8, 8);\n\
\n\
    if (low > 0xFF00 && low < 0x10000) {\n\
        c.outstanding_count = oc;\n\
        return;\n\
    }\n\
\n\
    u8buf bs = u8buf(c.bytestream);\n\
    uint8_t outstanding_byte = c.outstanding_byte;\n\
\n\
    c.bytestream        = uint64_t(bs) + oc;\n\
    c.outstanding_count = uint16_t(0);\n\
    c.outstanding_byte  = uint8_t(low >> 8);\n\
\n\
    uint8_t obs = uint8_t(low > 0xFF00);\n\
    uint8_t fill = obs - uint8_t(1); /* unsigned underflow */\n\
\n\
    bs[0].v = outstanding_byte + obs;\n\
    for (int i = 1; i < oc; i++)\n\
        bs[i].v = fill;\n\
}\n\
#endif\n\
\n\
void put_rac_internal(inout RangeCoder c, const int range1, bool bit)\n\
{\n\
#ifdef DEBUG\n\
    if (range1 >= c.range)\n\
        debugPrintfEXT(\"Error: range1 >= c.range\");\n\
    if (range1 <= 0)\n\
        debugPrintfEXT(\"Error: range1 <= 0\");\n\
#endif\n\
\n\
    int ranged = c.range - range1;\n\
    c.low += bit ? ranged : 0;\n\
    c.range = bit ? range1 : ranged;\n\
\n\
    if (expectEXT(c.range < 0x100, false))\n\
        renorm_encoder(c);\n\
}\n\
\n\
void put_rac_direct(inout RangeCoder c, inout uint8_t state, bool bit)\n\
{\n\
    put_rac_internal(c, (c.range * state) >> 8, bit);\n\
    state = zero_one_state[(uint(bit) << 8) + state];\n\
}\n\
\n\
void put_rac(inout RangeCoder c, uint64_t state, bool bit)\n\
{\n\
    put_rac_direct(c, u8buf(state).v, bit);\n\
}\n\
\n\
/* Equiprobable bit */\n\
void put_rac_equi(inout RangeCoder c, bool bit)\n\
{\n\
    put_rac_internal(c, c.range >> 1, bit);\n\
}\n\
\n\
void put_rac_terminate(inout RangeCoder c)\n\
{\n\
    int range1 = (c.range * 129) >> 8;\n\
\n\
#ifdef DEBUG\n\
    if (range1 >= c.range)\n\
        debugPrintfEXT(\"Error: range1 >= c.range\");\n\
    if (range1 <= 0)\n\
        debugPrintfEXT(\"Error: range1 <= 0\");\n\
#endif\n\
\n\
    c.range -= range1;\n\
    if (expectEXT(c.range < 0x100, false))\n\
        renorm_encoder(c);\n\
}\n\
\n\
/* Return the number of bytes written. */\n\
uint32_t rac_terminate(inout RangeCoder c)\n\
{\n\
    put_rac_terminate(c);\n\
    c.range = uint16_t(0xFF);\n\
    c.low  += 0xFF;\n\
    renorm_encoder(c);\n\
    c.range = uint16_t(0xFF);\n\
    renorm_encoder(c);\n\
\n\
#ifdef DEBUG\n\
    if (c.low != 0)\n\
        debugPrintfEXT(\"Error: c.low != 0\");\n\
    if (c.range < 0x100)\n\
        debugPrintfEXT(\"Error: range < 0x100\");\n\
#endif\n\
\n\
    return uint32_t(uint64_t(c.bytestream) - uint64_t(c.bytestream_start));\n\
}\n\
\n\
void rac_init(out RangeCoder r, u8buf data, uint buf_size)\n\
{\n\
    r.bytestream_start = uint64_t(data);\n\
    r.bytestream = uint64_t(data);\n\
    r.bytestream_end = uint64_t(data) + buf_size;\n\
    r.low = 0;\n\
    r.range = 0xFF00;\n\
    r.outstanding_count = uint16_t(0);\n\
    r.outstanding_byte = uint8_t(0xFF);\n\
}\n\
\n\
/* Decoder */\n\
uint overread = 0;\n\
bool corrupt = false;\n\
\n\
void rac_init_dec(out RangeCoder r, u8buf data, uint buf_size)\n\
{\n\
    overread = 0;\n\
    corrupt = false;\n\
\n\
    /* Skip priming bytes */\n\
    rac_init(r, OFFBUF(u8buf, data, 2), buf_size - 2);\n\
\n\
    u8vec2 prime = u8vec2buf(data).v;\n\
    /* Switch endianess of the priming bytes */\n\
    r.low = pack16(prime.yx);\n\
\n\
    if (r.low >= 0xFF00) {\n\
        r.low = 0xFF00;\n\
        r.bytestream_end = uint64_t(data) + 2;\n\
    }\n\
}\n\
\n\
void refill(inout RangeCoder c)\n\
{\n\
    c.range <<= 8;\n\
    c.low   <<= 8;\n\
    if (expectEXT(c.bytestream < c.bytestream_end, false)) {\n\
        c.low |= u8buf(c.bytestream).v;\n\
        c.bytestream++;\n\
    } else {\n\
        overread++;\n\
    }\n\
}\n\
\n\
bool get_rac_internal(inout RangeCoder c, const int range1)\n\
{\n\
    int ranged = c.range - range1;\n\
    bool bit = c.low >= ranged;\n\
    c.low -= bit ? ranged : 0;\n\
    c.range = (bit ? 0 : ranged) + (bit ? range1 : 0);\n\
\n\
    if (expectEXT(c.range < 0x100, false))\n\
        refill(c);\n\
\n\
    return bit;\n\
}\n\
\n\
bool get_rac_direct(inout RangeCoder c, inout uint8_t state)\n\
{\n\
    bool bit = get_rac_internal(c, c.range * state >> 8);\n\
    state = zero_one_state[state + (bit ? 256 : 0)];\n\
    return bit;\n\
}\n\
\n\
bool get_rac(inout RangeCoder c, uint64_t state)\n\
{\n\
    return get_rac_direct(c, u8buf(state).v);\n\
}\n\
\n\
bool get_rac_equi(inout RangeCoder c)\n\
{\n\
    return get_rac_internal(c, c.range >> 1);\n\
}";

const char *ff_source_ffv1_vlc_comp = "\
#define VLC_STATE_SIZE 8\n\
layout(buffer_reference, buffer_reference_align = VLC_STATE_SIZE) buffer VlcState {\n\
    uint32_t error_sum;\n\
    int16_t  drift;\n\
    int8_t   bias;\n\
    uint8_t  count;\n\
};\n\
\n\
void update_vlc_state(inout VlcState state, const int v)\n\
{\n\
    int drift = state.drift;\n\
    int count = state.count;\n\
    int bias = state.bias;\n\
    state.error_sum += uint16_t(abs(v));\n\
    drift           += v;\n\
\n\
    if (count == 128) { // FIXME: variable\n\
        count           >>= 1;\n\
        drift           >>= 1;\n\
        state.error_sum >>= 1;\n\
    }\n\
    count++;\n\
\n\
    if (drift <= -count) {\n\
        bias = max(bias - 1, -128);\n\
        drift = max(drift + count, -count + 1);\n\
    } else if (drift > 0) {\n\
        bias = min(bias + 1, 127);\n\
        drift = min(drift - count, 0);\n\
    }\n\
\n\
    state.bias = int8_t(bias);\n\
    state.drift = int16_t(drift);\n\
    state.count = uint8_t(count);\n\
}\n\
\n\
struct Symbol {\n\
    uint32_t bits;\n\
    uint32_t val;\n\
};\n\
\n\
Symbol set_ur_golomb(int i, int k, int limit, int esc_len)\n\
{\n\
    int e;\n\
    Symbol sym;\n\
\n\
#ifdef DEBUG\n\
    if (i < 0)\n\
        debugPrintfEXT(\"Error: i is zero!\");\n\
#endif\n\
\n\
    e = i >> k;\n\
    if (e < limit) {\n\
        sym.bits = e + k + 1;\n\
        sym.val = (1 << k) + zero_extend(i, k);\n\
    } else {\n\
        sym.bits = limit + esc_len;\n\
        sym.val = i - limit + 1;\n\
    }\n\
\n\
    return sym;\n\
}\n\
\n\
/**\n\
 * write signed golomb rice code (ffv1).\n\
 */\n\
Symbol set_sr_golomb(int i, int k, int limit, int esc_len)\n\
{\n\
    int v;\n\
\n\
    v  = -2 * i - 1;\n\
    v ^= (v >> 31);\n\
\n\
    return set_ur_golomb(v, k, limit, esc_len);\n\
}\n\
\n\
Symbol get_vlc_symbol(inout VlcState state, int v, int bits)\n\
{\n\
    int i, k, code;\n\
    Symbol sym;\n\
    v = fold(v - int(state.bias), bits);\n\
\n\
    i = state.count;\n\
    k = 0;\n\
    while (i < state.error_sum) { // FIXME: optimize\n\
        k++;\n\
        i += i;\n\
    }\n\
\n\
#ifdef DEBUG\n\
    if (k > 16)\n\
        debugPrintfEXT(\"Error: k > 16!\");\n\
#endif\n\
\n\
    code = v ^ ((2 * state.drift + state.count) >> 31);\n\
\n\
    update_vlc_state(state, v);\n\
\n\
    return set_sr_golomb(code, k, 12, bits);\n\
}\n\
\n\
uint get_ur_golomb(inout GetBitContext gb, uint k, int limit, int esc_len)\n\
{\n\
    for (uint i = 0; i < 12; i++)\n\
        if (get_bit(gb))\n\
            return get_bits(gb, k) + (i << k);\n\
\n\
    return get_bits(gb, esc_len) + 11;\n\
}\n\
\n\
int get_sr_golomb(inout GetBitContext gb, uint k, int limit, int esc_len)\n\
{\n\
    int v = int(get_ur_golomb(gb, k, limit, esc_len));\n\
    return (v >> 1) ^ -(v & 1);\n\
}\n\
\n\
int read_vlc_symbol(inout GetBitContext gb, inout VlcState state, int bits)\n\
{\n\
    int k, i, v, ret;\n\
\n\
    i = state.count;\n\
    k = 0;\n\
    while (i < state.error_sum) { // FIXME: optimize\n\
        k++;\n\
        i += i;\n\
    }\n\
\n\
    v = get_sr_golomb(gb, k, 12, bits);\n\
\n\
    v ^= ((2 * state.drift + state.count) >> 31);\n\
\n\
    ret = fold(v + state.bias, bits);\n\
\n\
    update_vlc_state(state, v);\n\
\n\
    return ret;\n\
}";

const char *ff_source_ffv1_common_comp = "\
struct SliceContext {\n\
    RangeCoder c;\n\
\n\
#if !defined(DECODE)\n\
    PutBitContext pb; /* 8*8 bytes */\n\
#else\n\
    GetBitContext gb;\n\
#endif\n\
\n\
    ivec2 slice_dim;\n\
    ivec2 slice_pos;\n\
    ivec2 slice_rct_coef;\n\
    u8vec3 quant_table_idx;\n\
\n\
    uint hdr_len; // only used for golomb\n\
\n\
    uint slice_coding_mode;\n\
    bool slice_reset_contexts;\n\
};\n\
\n\
/* -1, { -1, 0 } */\n\
int predict(int L, ivec2 top)\n\
{\n\
    return mid_pred(L, L + top[1] - top[0], top[1]);\n\
}\n\
\n\
/* { -2, -1 }, { -1, 0, 1 }, 0 */\n\
int get_context(VTYPE2 cur_l, VTYPE3 top_l, TYPE top2, uint8_t quant_table_idx)\n\
{\n\
    const int LT = top_l[0]; /* -1 */\n\
    const int T  = top_l[1]; /*  0 */\n\
    const int RT = top_l[2]; /*  1 */\n\
    const int L  = cur_l[1]; /* -1 */\n\
\n\
    int base = quant_table[quant_table_idx][0][(L - LT) & MAX_QUANT_TABLE_MASK] +\n\
               quant_table[quant_table_idx][1][(LT - T) & MAX_QUANT_TABLE_MASK] +\n\
               quant_table[quant_table_idx][2][(T - RT) & MAX_QUANT_TABLE_MASK];\n\
\n\
    if ((quant_table[quant_table_idx][3][127] == 0) &&\n\
        (quant_table[quant_table_idx][4][127] == 0))\n\
        return base;\n\
\n\
    const int TT = top2;     /* -2 */\n\
    const int LL = cur_l[0]; /* -2 */\n\
    return base +\n\
           quant_table[quant_table_idx][3][(LL - L) & MAX_QUANT_TABLE_MASK] +\n\
           quant_table[quant_table_idx][4][(TT - T) & MAX_QUANT_TABLE_MASK];\n\
}\n\
\n\
const uint32_t log2_run[41] = {\n\
     0,  0,  0,  0,  1,  1,  1,  1,\n\
     2,  2,  2,  2,  3,  3,  3,  3,\n\
     4,  4,  5,  5,  6,  6,  7,  7,\n\
     8,  9, 10, 11, 12, 13, 14, 15,\n\
    16, 17, 18, 19, 20, 21, 22, 23,\n\
    24,\n\
};\n\
\n\
uint slice_coord(uint width, uint sx, uint num_h_slices, uint chroma_shift)\n\
{\n\
    uint mpw = 1 << chroma_shift;\n\
    uint awidth = align(width, mpw);\n\
\n\
    if ((version < 4) || ((version == 4) && (micro_version < 3)))\n\
        return width * sx / num_h_slices;\n\
\n\
    sx = (2 * awidth * sx + num_h_slices * mpw) / (2 * num_h_slices * mpw) * mpw;\n\
    if (sx == awidth)\n\
        sx = width;\n\
\n\
    return sx;\n\
}\n\
\n\
#ifdef RGB\n\
#define RGB_LBUF (RGB_LINECACHE - 1)\n\
#define LADDR(p) (ivec2((p).x, ((p).y & RGB_LBUF)))\n\
\n\
ivec2 get_pred(readonly uimage2D pred, ivec2 sp, ivec2 off,\n\
               int comp, int sw, uint8_t quant_table_idx, bool extend_lookup)\n\
{\n\
    const ivec2 yoff_border1 = expectEXT(off.x == 0, false) ? off + ivec2(1, -1) : off;\n\
\n\
    /* Thanks to the same coincidence as below, we can skip checking if off == 0, 1 */\n\
    VTYPE3 top  = VTYPE3(TYPE(imageLoad(pred, sp + LADDR(yoff_border1 + ivec2(-1, -1)))[comp]),\n\
                         TYPE(imageLoad(pred, sp + LADDR(off + ivec2(0, -1)))[comp]),\n\
                         TYPE(imageLoad(pred, sp + LADDR(off + ivec2(min(1, sw - off.x - 1), -1)))[comp]));\n\
\n\
    /* Normally, we'd need to check if off != ivec2(0, 0) here, since otherwise, we must\n\
     * return zero. However, ivec2(-1,  0) + ivec2(1, -1) == ivec2(0, -1), e.g. previous\n\
     * row, 0 offset, same slice, which is zero since we zero out the buffer for RGB */\n\
    TYPE cur = TYPE(imageLoad(pred, sp + LADDR(yoff_border1 + ivec2(-1,  0)))[comp]);\n\
\n\
    int base = quant_table[quant_table_idx][0][(cur    - top[0]) & MAX_QUANT_TABLE_MASK] +\n\
               quant_table[quant_table_idx][1][(top[0] - top[1]) & MAX_QUANT_TABLE_MASK] +\n\
               quant_table[quant_table_idx][2][(top[1] - top[2]) & MAX_QUANT_TABLE_MASK];\n\
\n\
    if (expectEXT(extend_lookup, false)) {\n\
        TYPE cur2 = TYPE(0);\n\
        if (expectEXT(off.x > 0, true)) {\n\
            const ivec2 yoff_border2 = expectEXT(off.x == 1, false) ? ivec2(-1, -1) : ivec2(-2, 0);\n\
            cur2 = TYPE(imageLoad(pred, sp + LADDR(off + yoff_border2))[comp]);\n\
        }\n\
        base += quant_table[quant_table_idx][3][(cur2 - cur) & MAX_QUANT_TABLE_MASK];\n\
\n\
        /* top-2 became current upon swap */\n\
        TYPE top2 = TYPE(imageLoad(pred, sp + LADDR(off))[comp]);\n\
        base += quant_table[quant_table_idx][4][(top2 - top[1]) & MAX_QUANT_TABLE_MASK];\n\
    }\n\
\n\
    /* context, prediction */\n\
    return ivec2(base, predict(cur, VTYPE2(top)));\n\
}\n\
\n\
#else /* RGB */\n\
\n\
#define LADDR(p) (p)\n\
\n\
ivec2 get_pred(readonly uimage2D pred, ivec2 sp, ivec2 off,\n\
               int comp, int sw, uint8_t quant_table_idx, bool extend_lookup)\n\
{\n\
    const ivec2 yoff_border1 = off.x == 0 ? ivec2(1, -1) : ivec2(0, 0);\n\
    sp += off;\n\
\n\
    VTYPE3 top  = VTYPE3(TYPE(0),\n\
                         TYPE(0),\n\
                         TYPE(0));\n\
    if (off.y > 0 && off != ivec2(0, 1))\n\
        top[0] = TYPE(imageLoad(pred, sp + ivec2(-1, -1) + yoff_border1)[comp]);\n\
    if (off.y > 0) {\n\
        top[1] = TYPE(imageLoad(pred, sp + ivec2(0, -1))[comp]);\n\
        top[2] = TYPE(imageLoad(pred, sp + ivec2(min(1, sw - off.x - 1), -1))[comp]);\n\
    }\n\
\n\
    TYPE cur = TYPE(0);\n\
    if (off != ivec2(0, 0))\n\
        cur = TYPE(imageLoad(pred, sp + ivec2(-1,  0) + yoff_border1)[comp]);\n\
\n\
    int base = quant_table[quant_table_idx][0][(cur - top[0]) & MAX_QUANT_TABLE_MASK] +\n\
               quant_table[quant_table_idx][1][(top[0] - top[1]) & MAX_QUANT_TABLE_MASK] +\n\
               quant_table[quant_table_idx][2][(top[1] - top[2]) & MAX_QUANT_TABLE_MASK];\n\
\n\
    if (expectEXT(extend_lookup, false)) {\n\
        TYPE cur2 = TYPE(0);\n\
        if (off.x > 0 && off != ivec2(1, 0)) {\n\
            const ivec2 yoff_border2 = off.x == 1 ? ivec2(1, -1) : ivec2(0, 0);\n\
            cur2 = TYPE(imageLoad(pred, sp + ivec2(-2,  0) + yoff_border2)[comp]);\n\
        }\n\
        base += quant_table[quant_table_idx][3][(cur2 - cur) & MAX_QUANT_TABLE_MASK];\n\
\n\
        TYPE top2 = TYPE(0);\n\
        if (off.y > 1)\n\
            top2 = TYPE(imageLoad(pred, sp + ivec2(0, -2))[comp]);\n\
        base += quant_table[quant_table_idx][4][(top2 - top[1]) & MAX_QUANT_TABLE_MASK];\n\
    }\n\
\n\
    /* context, prediction */\n\
    return ivec2(base, predict(cur, VTYPE2(top)));\n\
}\n\
#endif";

const char *ff_source_ffv1_reset_comp = "\
void main(void)\n\
{\n\
    const uint slice_idx = gl_WorkGroupID.y*gl_NumWorkGroups.x + gl_WorkGroupID.x;\n\
\n\
    if (key_frame == 0 &&\n\
        slice_ctx[slice_idx].slice_reset_contexts == false)\n\
        return;\n\
\n\
    const uint8_t qidx = slice_ctx[slice_idx].quant_table_idx[gl_WorkGroupID.z];\n\
    uint contexts = context_count[qidx];\n\
    uint64_t slice_state_off = uint64_t(slice_state) +\n\
                               slice_idx*plane_state_size*codec_planes;\n\
\n\
#ifdef GOLOMB\n\
    uint64_t start = slice_state_off +\n\
                     (gl_WorkGroupID.z*(plane_state_size/VLC_STATE_SIZE) + gl_LocalInvocationID.x)*VLC_STATE_SIZE;\n\
    for (uint x = gl_LocalInvocationID.x; x < contexts; x += gl_WorkGroupSize.x) {\n\
        VlcState sb = VlcState(start);\n\
        sb.drift     =  int16_t(0);\n\
        sb.error_sum = uint16_t(4);\n\
        sb.bias      =   int8_t(0);\n\
        sb.count     =  uint8_t(1);\n\
        start += gl_WorkGroupSize.x*VLC_STATE_SIZE;\n\
    }\n\
#else\n\
    uint64_t start = slice_state_off +\n\
                     gl_WorkGroupID.z*plane_state_size +\n\
                     (gl_LocalInvocationID.x << 2 /* dwords */); /* Bytes */\n\
    uint count_total = contexts*(CONTEXT_SIZE /* bytes */ >> 2 /* dwords */);\n\
    for (uint x = gl_LocalInvocationID.x; x < count_total; x += gl_WorkGroupSize.x) {\n\
        u32buf(start).v = 0x80808080;\n\
        start += gl_WorkGroupSize.x*(CONTEXT_SIZE >> 3 /* 1/8th of context */);\n\
    }\n\
#endif\n\
}";

const char *ff_source_ffv1_rct_search_comp = "\
ivec3 load_components(ivec2 pos)\n\
{\n\
    ivec3 pix = ivec3(imageLoad(src[0], pos));\n\
    if (planar_rgb != 0) {\n\
        for (int i = 1; i < 3; i++)\n\
            pix[i] = int(imageLoad(src[i], pos)[0]);\n\
    }\n\
\n\
    return ivec3(pix[fmt_lut[0]], pix[fmt_lut[1]], pix[fmt_lut[2]]);\n\
}\n\
\n\
#define NUM_CHECKS 15\n\
const ivec2 rct_y_coeff[NUM_CHECKS] = {\n\
    ivec2(0, 0), //      4G\n\
\n\
    ivec2(0, 1), //      3G +  B\n\
    ivec2(1, 0), //  R + 3G\n\
    ivec2(1, 1), //  R + 2G + B\n\
\n\
    ivec2(0, 2), //      2G + 2B\n\
    ivec2(2, 0), // 2R + 2G\n\
    ivec2(2, 2), // 2R      + 2B\n\
\n\
    ivec2(0, 3), //      1G + 3B\n\
    ivec2(3, 0), // 3R + 1G\n\
\n\
    ivec2(0, 4), //           4B\n\
    ivec2(4, 0), // 4R\n\
\n\
    ivec2(1, 2), //  R +  G + 2B\n\
    ivec2(2, 1), // 2R +  G +  B\n\
\n\
    ivec2(3, 1), // 3R      +  B\n\
    ivec2(1, 3), //  R      + 3B\n\
};\n\
\n\
shared ivec3 pix_buf[gl_WorkGroupSize.x + 1][gl_WorkGroupSize.y + 1] = { };\n\
\n\
ivec3 transform_sample(ivec3 pix, ivec2 rct_coef)\n\
{\n\
    pix.b -= pix.g;\n\
    pix.r -= pix.g;\n\
    pix.g += (pix.r*rct_coef.x + pix.b*rct_coef.y) >> 2;\n\
    pix.b += rct_offset;\n\
    pix.r += rct_offset;\n\
    return pix;\n\
}\n\
\n\
uint get_dist(ivec3 cur)\n\
{\n\
    ivec3 LL = pix_buf[gl_LocalInvocationID.x + 0][gl_LocalInvocationID.y + 1];\n\
    ivec3 TL = pix_buf[gl_LocalInvocationID.x + 0][gl_LocalInvocationID.y + 0];\n\
    ivec3 TT = pix_buf[gl_LocalInvocationID.x + 1][gl_LocalInvocationID.y + 0];\n\
\n\
    ivec3 pred = ivec3(predict(LL.r, ivec2(TL.r, TT.r)),\n\
                       predict(LL.g, ivec2(TL.g, TT.g)),\n\
                       predict(LL.b, ivec2(TL.b, TT.b)));\n\
\n\
    uvec3 c = abs(pred - cur);\n\
    return mid_pred(c.r, c.g, c.b);\n\
}\n\
\n\
shared uint score_cols[gl_WorkGroupSize.y] = { };\n\
shared uint score_mode[16] = { };\n\
\n\
void process(ivec2 pos)\n\
{\n\
    ivec3 pix = load_components(pos);\n\
\n\
    for (int i = 0; i < NUM_CHECKS; i++) {\n\
        ivec3 tx_pix = transform_sample(pix, rct_y_coeff[i]);\n\
        pix_buf[gl_LocalInvocationID.x + 1][gl_LocalInvocationID.y + 1] = tx_pix;\n\
        memoryBarrierShared();\n\
\n\
        uint dist = get_dist(tx_pix);\n\
        atomicAdd(score_mode[i], dist);\n\
    }\n\
}\n\
\n\
void coeff_search(inout SliceContext sc)\n\
{\n\
    uvec2 img_size = imageSize(src[0]);\n\
    uint sxs = slice_coord(img_size.x, gl_WorkGroupID.x + 0,\n\
                           gl_NumWorkGroups.x, 0);\n\
    uint sxe = slice_coord(img_size.x, gl_WorkGroupID.x + 1,\n\
                           gl_NumWorkGroups.x, 0);\n\
    uint sys = slice_coord(img_size.y, gl_WorkGroupID.y + 0,\n\
                           gl_NumWorkGroups.y, 0);\n\
    uint sye = slice_coord(img_size.y, gl_WorkGroupID.y + 1,\n\
                           gl_NumWorkGroups.y, 0);\n\
\n\
    for (uint y = sys + gl_LocalInvocationID.y; y < sye; y += gl_WorkGroupSize.y) {\n\
        for (uint x = sxs + gl_LocalInvocationID.x; x < sxe; x += gl_WorkGroupSize.x) {\n\
            process(ivec2(x, y));\n\
        }\n\
    }\n\
\n\
    if (gl_LocalInvocationID.x == 0 && gl_LocalInvocationID.y == 0) {\n\
        uint min_score = 0xFFFFFFFF;\n\
        uint min_idx = 3;\n\
        for (int i = 0; i < NUM_CHECKS; i++) {\n\
            if (score_mode[i] < min_score) {\n\
                min_score = score_mode[i];\n\
                min_idx = i;\n\
            }\n\
        }\n\
        sc.slice_rct_coef = rct_y_coeff[min_idx];\n\
    }\n\
}\n\
\n\
void main(void)\n\
{\n\
    if (force_pcm == 1)\n\
        return;\n\
    const uint slice_idx = gl_WorkGroupID.y*gl_NumWorkGroups.x + gl_WorkGroupID.x;\n\
    coeff_search(slice_ctx[slice_idx]);\n\
}";

const char *ff_source_ffv1_enc_setup_comp = "\
uint8_t state[CONTEXT_SIZE];\n\
\n\
void init_slice(inout SliceContext sc, const uint slice_idx)\n\
{\n\
    /* Set coordinates */\n\
    uvec2 img_size = imageSize(src[0]);\n\
    uint sxs = slice_coord(img_size.x, gl_WorkGroupID.x + 0,\n\
                           gl_NumWorkGroups.x, chroma_shift.x);\n\
    uint sxe = slice_coord(img_size.x, gl_WorkGroupID.x + 1,\n\
                           gl_NumWorkGroups.x, chroma_shift.x);\n\
    uint sys = slice_coord(img_size.y, gl_WorkGroupID.y + 0,\n\
                           gl_NumWorkGroups.y, chroma_shift.y);\n\
    uint sye = slice_coord(img_size.y, gl_WorkGroupID.y + 1,\n\
                           gl_NumWorkGroups.y, chroma_shift.y);\n\
\n\
    sc.slice_pos = ivec2(sxs, sys);\n\
    sc.slice_dim = ivec2(sxe - sxs, sye - sys);\n\
    sc.slice_coding_mode = int(force_pcm == 1);\n\
    sc.slice_reset_contexts = sc.slice_coding_mode == 1;\n\
    sc.quant_table_idx = u8vec3(context_model);\n\
\n\
    if ((rct_search == 0) || (sc.slice_coding_mode == 1))\n\
        sc.slice_rct_coef = ivec2(1, 1);\n\
\n\
    rac_init(sc.c,\n\
             OFFBUF(u8buf, out_data, slice_idx * slice_size_max),\n\
             slice_size_max);\n\
}\n\
\n\
void put_usymbol(inout RangeCoder c, uint v)\n\
{\n\
    bool is_nil = (v == 0);\n\
    put_rac_direct(c, state[0], is_nil);\n\
    if (is_nil)\n\
        return;\n\
\n\
    const int e = findMSB(v);\n\
\n\
    for (int i = 0; i < e; i++)\n\
        put_rac_direct(c, state[1 + min(i, 9)], true);\n\
    put_rac_direct(c, state[1 + min(e, 9)], false);\n\
\n\
    for (int i = e - 1; i >= 0; i--)\n\
        put_rac_direct(c, state[22 + min(i, 9)], bool(bitfieldExtract(v, i, 1)));\n\
}\n\
\n\
void write_slice_header(inout SliceContext sc)\n\
{\n\
    [[unroll]]\n\
    for (int i = 0; i < CONTEXT_SIZE; i++)\n\
        state[i] = uint8_t(128);\n\
\n\
    put_usymbol(sc.c, gl_WorkGroupID.x);\n\
    put_usymbol(sc.c, gl_WorkGroupID.y);\n\
    put_usymbol(sc.c, 0);\n\
    put_usymbol(sc.c, 0);\n\
\n\
    for (int i = 0; i < codec_planes; i++)\n\
        put_usymbol(sc.c, sc.quant_table_idx[i]);\n\
\n\
    put_usymbol(sc.c, pic_mode);\n\
    put_usymbol(sc.c, sar.x);\n\
    put_usymbol(sc.c, sar.y);\n\
\n\
    if (version >= 4) {\n\
        put_rac_direct(sc.c, state[0], sc.slice_reset_contexts);\n\
        put_usymbol(sc.c, sc.slice_coding_mode);\n\
        if (sc.slice_coding_mode != 1 && colorspace == 1) {\n\
            put_usymbol(sc.c, sc.slice_rct_coef.y);\n\
            put_usymbol(sc.c, sc.slice_rct_coef.x);\n\
        }\n\
    }\n\
}\n\
\n\
void write_frame_header(inout SliceContext sc)\n\
{\n\
    put_rac_equi(sc.c, bool(key_frame));\n\
}\n\
\n\
#ifdef GOLOMB\n\
void init_golomb(inout SliceContext sc)\n\
{\n\
    sc.hdr_len = rac_terminate(sc.c);\n\
    init_put_bits(sc.pb,\n\
                  OFFBUF(u8buf, sc.c.bytestream_start, sc.hdr_len),\n\
                  slice_size_max - sc.hdr_len);\n\
}\n\
#endif\n\
\n\
void main(void)\n\
{\n\
    const uint slice_idx = gl_WorkGroupID.y*gl_NumWorkGroups.x + gl_WorkGroupID.x;\n\
\n\
    init_slice(slice_ctx[slice_idx], slice_idx);\n\
\n\
    if (slice_idx == 0)\n\
        write_frame_header(slice_ctx[slice_idx]);\n\
\n\
    write_slice_header(slice_ctx[slice_idx]);\n\
\n\
#ifdef GOLOMB\n\
    init_golomb(slice_ctx[slice_idx]);\n\
#endif\n\
}";

const char *ff_source_ffv1_enc_comp = "\
#ifndef GOLOMB\n\
#ifdef CACHED_SYMBOL_READER\n\
shared uint8_t state[CONTEXT_SIZE];\n\
#define WRITE(c, off, val) put_rac_direct(c, state[off], val)\n\
#else\n\
#define WRITE(c, off, val) put_rac(c, uint64_t(slice_state) + (state_off + off), val)\n\
#endif\n\
\n\
/* Note - only handles signed values */\n\
void put_symbol(inout RangeCoder c, uint state_off, int v)\n\
{\n\
    bool is_nil = (v == 0);\n\
    WRITE(c, 0, is_nil);\n\
    if (is_nil)\n\
        return;\n\
\n\
    const int a = abs(v);\n\
    const int e = findMSB(a);\n\
\n\
    for (int i = 0; i < e; i++)\n\
        WRITE(c, 1 + min(i, 9), true);\n\
    WRITE(c, 1 + min(e, 9), false);\n\
\n\
    for (int i = e - 1; i >= 0; i--)\n\
        WRITE(c, 22 + min(i, 9), bool(bitfieldExtract(a, i, 1)));\n\
\n\
    WRITE(c, 22 - 11 + min(e, 10), v < 0);\n\
}\n\
\n\
void encode_line_pcm(inout SliceContext sc, readonly uimage2D img,\n\
                     ivec2 sp, int y, int p, int comp, int bits)\n\
{\n\
    int w = sc.slice_dim.x;\n\
\n\
#ifdef CACHED_SYMBOL_READER\n\
    if (gl_LocalInvocationID.x > 0)\n\
        return;\n\
#endif\n\
\n\
#ifndef RGB\n\
    if (p > 0 && p < 3) {\n\
        w >>= chroma_shift.x;\n\
        sp >>= chroma_shift;\n\
    }\n\
#endif\n\
\n\
    for (int x = 0; x < w; x++) {\n\
        uint v = imageLoad(img, sp + LADDR(ivec2(x, y)))[comp];\n\
        for (int i = (bits - 1); i >= 0; i--)\n\
            put_rac_equi(sc.c, bool(bitfieldExtract(v, i, 1)));\n\
    }\n\
}\n\
\n\
void encode_line(inout SliceContext sc, readonly uimage2D img, uint state_off,\n\
                 ivec2 sp, int y, int p, int comp, int bits,\n\
                 uint8_t quant_table_idx, const int run_index)\n\
{\n\
    int w = sc.slice_dim.x;\n\
\n\
#ifndef RGB\n\
    if (p > 0 && p < 3) {\n\
        w >>= chroma_shift.x;\n\
        sp >>= chroma_shift;\n\
    }\n\
#endif\n\
\n\
    for (int x = 0; x < w; x++) {\n\
        ivec2 d = get_pred(img, sp, ivec2(x, y), comp, w,\n\
                           quant_table_idx, extend_lookup[quant_table_idx] > 0);\n\
        d[1] = int(imageLoad(img, sp + LADDR(ivec2(x, y)))[comp]) - d[1];\n\
\n\
        if (d[0] < 0)\n\
            d = -d;\n\
\n\
        d[1] = fold(d[1], bits);\n\
\n\
        uint context_off = state_off + CONTEXT_SIZE*d[0];\n\
#ifdef CACHED_SYMBOL_READER\n\
        u8buf sb = u8buf(uint64_t(slice_state) + context_off + gl_LocalInvocationID.x);\n\
        state[gl_LocalInvocationID.x] = sb.v;\n\
        barrier();\n\
        if (gl_LocalInvocationID.x == 0)\n\
#endif\n\
\n\
            put_symbol(sc.c, context_off, d[1]);\n\
\n\
#ifdef CACHED_SYMBOL_READER\n\
        barrier();\n\
        sb.v = state[gl_LocalInvocationID.x];\n\
#endif\n\
    }\n\
}\n\
\n\
#else /* GOLOMB */\n\
\n\
void encode_line(inout SliceContext sc, readonly uimage2D img, uint state_off,\n\
                 ivec2 sp, int y, int p, int comp, int bits,\n\
                 uint8_t quant_table_idx, inout int run_index)\n\
{\n\
    int w = sc.slice_dim.x;\n\
\n\
#ifndef RGB\n\
    if (p > 0 && p < 3) {\n\
        w >>= chroma_shift.x;\n\
        sp >>= chroma_shift;\n\
    }\n\
#endif\n\
\n\
    int run_count = 0;\n\
    bool run_mode = false;\n\
\n\
    for (int x = 0; x < w; x++) {\n\
        ivec2 d = get_pred(img, sp, ivec2(x, y), comp, w,\n\
                           quant_table_idx, extend_lookup[quant_table_idx] > 0);\n\
        d[1] = int(imageLoad(img, sp + LADDR(ivec2(x, y)))[comp]) - d[1];\n\
\n\
        if (d[0] < 0)\n\
            d = -d;\n\
\n\
        d[1] = fold(d[1], bits);\n\
\n\
        if (d[0] == 0)\n\
            run_mode = true;\n\
\n\
        if (run_mode) {\n\
            if (d[1] != 0) {\n\
                /* A very unlikely loop */\n\
                while (run_count >= 1 << log2_run[run_index]) {\n\
                    run_count -= 1 << log2_run[run_index];\n\
                    run_index++;\n\
                    put_bits(sc.pb, 1, 1);\n\
                }\n\
\n\
                put_bits(sc.pb, 1 + log2_run[run_index], run_count);\n\
                if (run_index != 0)\n\
                    run_index--;\n\
                run_count = 0;\n\
                run_mode  = false;\n\
                if (d[1] > 0)\n\
                    d[1]--;\n\
            } else {\n\
                run_count++;\n\
            }\n\
        }\n\
\n\
        if (!run_mode) {\n\
            VlcState sb = VlcState(uint64_t(slice_state) + state_off + VLC_STATE_SIZE*d[0]);\n\
            Symbol sym = get_vlc_symbol(sb, d[1], bits);\n\
            put_bits(sc.pb, sym.bits, sym.val);\n\
        }\n\
    }\n\
\n\
    if (run_mode) {\n\
        while (run_count >= (1 << log2_run[run_index])) {\n\
            run_count -= 1 << log2_run[run_index];\n\
            run_index++;\n\
            put_bits(sc.pb, 1, 1);\n\
        }\n\
\n\
        if (run_count > 0)\n\
            put_bits(sc.pb, 1, 1);\n\
    }\n\
}\n\
#endif\n\
\n\
#ifdef RGB\n\
ivec4 load_components(ivec2 pos)\n\
{\n\
    ivec4 pix = ivec4(imageLoad(src[0], pos));\n\
    if (planar_rgb != 0) {\n\
        for (int i = 1; i < (3 + transparency); i++)\n\
            pix[i] = int(imageLoad(src[i], pos)[0]);\n\
    }\n\
\n\
    return ivec4(pix[fmt_lut[0]], pix[fmt_lut[1]],\n\
                 pix[fmt_lut[2]], pix[fmt_lut[3]]);\n\
}\n\
\n\
void transform_sample(inout ivec4 pix, ivec2 rct_coef)\n\
{\n\
    pix.b -= pix.g;\n\
    pix.r -= pix.g;\n\
    pix.g += (pix.r*rct_coef.x + pix.b*rct_coef.y) >> 2;\n\
    pix.b += rct_offset;\n\
    pix.r += rct_offset;\n\
}\n\
\n\
void preload_rgb(in SliceContext sc, ivec2 sp, int w, int y, bool apply_rct)\n\
{\n\
    for (uint x = gl_LocalInvocationID.x; x < w; x += gl_WorkGroupSize.x) {\n\
        ivec2 lpos = sp + LADDR(ivec2(x, y));\n\
        ivec2 pos = sc.slice_pos + ivec2(x, y);\n\
\n\
        ivec4 pix = load_components(pos);\n\
\n\
        if (expectEXT(apply_rct, true))\n\
            transform_sample(pix, sc.slice_rct_coef);\n\
\n\
        imageStore(tmp, lpos, pix);\n\
    }\n\
}\n\
#endif\n\
\n\
void encode_slice(inout SliceContext sc, const uint slice_idx)\n\
{\n\
    ivec2 sp = sc.slice_pos;\n\
\n\
#ifndef RGB\n\
    int bits = bits_per_raw_sample;\n\
#else\n\
    int bits = 9;\n\
    if (bits != 8 || sc.slice_coding_mode != 0)\n\
        bits = bits_per_raw_sample + int(sc.slice_coding_mode != 1);\n\
\n\
    sp.y = int(gl_WorkGroupID.y)*RGB_LINECACHE;\n\
#endif\n\
\n\
#ifndef GOLOMB\n\
    if (sc.slice_coding_mode == 1) {\n\
#ifndef RGB\n\
        for (int c = 0; c < components; c++) {\n\
\n\
            int h = sc.slice_dim.y;\n\
            if (c > 0 && c < 3)\n\
                h >>= chroma_shift.y;\n\
\n\
            /* Takes into account dual-plane YUV formats */\n\
            int p = min(c, planes - 1);\n\
            int comp = c - p;\n\
\n\
            for (int y = 0; y < h; y++)\n\
                encode_line_pcm(sc, src[p], sp, y, p, comp, bits);\n\
        }\n\
#else\n\
        for (int y = 0; y < sc.slice_dim.y; y++) {\n\
            preload_rgb(sc, sp, sc.slice_dim.x, y, false);\n\
\n\
            encode_line_pcm(sc, tmp, sp, y, 0, 1, bits);\n\
            encode_line_pcm(sc, tmp, sp, y, 0, 2, bits);\n\
            encode_line_pcm(sc, tmp, sp, y, 0, 0, bits);\n\
            if (transparency == 1)\n\
                encode_line_pcm(sc, tmp, sp, y, 0, 3, bits);\n\
        }\n\
#endif\n\
    } else\n\
#endif\n\
    {\n\
        u8vec4 quant_table_idx = sc.quant_table_idx.xyyz;\n\
        u32vec4 slice_state_off = (slice_idx*codec_planes + uvec4(0, 1, 1, 2))*plane_state_size;\n\
\n\
#ifndef RGB\n\
        for (int c = 0; c < components; c++) {\n\
            int run_index = 0;\n\
\n\
            int h = sc.slice_dim.y;\n\
            if (c > 0 && c < 3)\n\
                h >>= chroma_shift.y;\n\
\n\
            int p = min(c, planes - 1);\n\
            int comp = c - p;\n\
\n\
            for (int y = 0; y < h; y++)\n\
                encode_line(sc, src[p], slice_state_off[c], sp, y, p,\n\
                            comp, bits, quant_table_idx[c], run_index);\n\
        }\n\
#else\n\
        int run_index = 0;\n\
        for (int y = 0; y < sc.slice_dim.y; y++) {\n\
            preload_rgb(sc, sp, sc.slice_dim.x, y, true);\n\
\n\
            encode_line(sc, tmp, slice_state_off[0],\n\
                        sp, y, 0, 1, bits, quant_table_idx[0], run_index);\n\
            encode_line(sc, tmp, slice_state_off[1],\n\
                        sp, y, 0, 2, bits, quant_table_idx[1], run_index);\n\
            encode_line(sc, tmp, slice_state_off[2],\n\
                        sp, y, 0, 0, bits, quant_table_idx[2], run_index);\n\
            if (transparency == 1)\n\
                encode_line(sc, tmp, slice_state_off[3],\n\
                            sp, y, 0, 3, bits, quant_table_idx[3], run_index);\n\
        }\n\
#endif\n\
    }\n\
}\n\
\n\
void finalize_slice(inout SliceContext sc, const uint slice_idx)\n\
{\n\
#ifdef CACHED_SYMBOL_READER\n\
    if (gl_LocalInvocationID.x > 0)\n\
        return;\n\
#endif\n\
\n\
#ifdef GOLOMB\n\
    uint32_t enc_len = sc.hdr_len + flush_put_bits(sc.pb);\n\
#else\n\
    uint32_t enc_len = rac_terminate(sc.c);\n\
#endif\n\
\n\
    u8buf bs = u8buf(sc.c.bytestream_start);\n\
\n\
    /* Append slice length */\n\
    u8vec4 enc_len_p = unpack8(enc_len);\n\
    bs[enc_len + 0].v = enc_len_p.z;\n\
    bs[enc_len + 1].v = enc_len_p.y;\n\
    bs[enc_len + 2].v = enc_len_p.x;\n\
    enc_len += 3;\n\
\n\
    /* Calculate and write CRC */\n\
    if (ec != 0) {\n\
        bs[enc_len].v = uint8_t(0);\n\
        enc_len++;\n\
\n\
        uint32_t crc = crcref;\n\
        for (int i = 0; i < enc_len; i++)\n\
            crc = crc_ieee[(crc & 0xFF) ^ uint32_t(bs[i].v)] ^ (crc >> 8);\n\
\n\
        if (crcref != 0x00000000)\n\
            crc ^= 0x8CD88196;\n\
\n\
        u8vec4 crc_p = unpack8(crc);\n\
        bs[enc_len + 0].v = crc_p.x;\n\
        bs[enc_len + 1].v = crc_p.y;\n\
        bs[enc_len + 2].v = crc_p.z;\n\
        bs[enc_len + 3].v = crc_p.w;\n\
        enc_len += 4;\n\
    }\n\
\n\
    slice_results[slice_idx*2 + 0] = enc_len;\n\
    slice_results[slice_idx*2 + 1] = uint64_t(bs) - uint64_t(out_data);\n\
}\n\
\n\
void main(void)\n\
{\n\
    const uint slice_idx = gl_WorkGroupID.y*gl_NumWorkGroups.x + gl_WorkGroupID.x;\n\
    encode_slice(slice_ctx[slice_idx], slice_idx);\n\
    finalize_slice(slice_ctx[slice_idx], slice_idx);\n\
}";

const char *ff_source_ffv1_rct_comp = "\
ivec4 load_components(ivec2 pos)\n\
{\n\
    ivec4 pix = ivec4(imageLoad(src[0], pos));\n\
    if (planar_rgb != 0) {\n\
        for (int i = 1; i < (3 + transparency); i++)\n\
            pix[i] = int(imageLoad(src[i], pos)[0]);\n\
    }\n\
\n\
    return ivec4(pix[fmt_lut[0]], pix[fmt_lut[1]],\n\
                 pix[fmt_lut[2]], pix[fmt_lut[3]]);\n\
}\n\
\n\
void bypass_sample(ivec2 pos)\n\
{\n\
    imageStore(dst[0], pos, load_components(pos));\n\
}\n\
\n\
void bypass_block(in SliceContext sc)\n\
{\n\
    ivec2 start = ivec2(gl_LocalInvocationID) + sc.slice_pos;\n\
    ivec2 end = sc.slice_pos + sc.slice_dim;\n\
    for (uint y = start.y; y < end.y; y += gl_WorkGroupSize.y)\n\
        for (uint x = start.x; x < end.x; x += gl_WorkGroupSize.x)\n\
            bypass_sample(ivec2(x, y));\n\
}\n\
\n\
void transform_sample(ivec2 pos, ivec2 rct_coef)\n\
{\n\
    ivec4 pix = load_components(pos);\n\
    pix.b -= offset;\n\
    pix.r -= offset;\n\
    pix.g -= (pix.r*rct_coef.x + pix.b*rct_coef.y) >> 2;\n\
    pix.b += pix.g;\n\
    pix.r += pix.g;\n\
    imageStore(dst[0], pos, pix);\n\
}\n\
\n\
void transform_sample(ivec2 pos, ivec2 rct_coef)\n\
{\n\
    ivec4 pix = load_components(pos);\n\
    pix.b -= pix.g;\n\
    pix.r -= pix.g;\n\
    pix.g += (pix.r*rct_coef.x + pix.b*rct_coef.y) >> 2;\n\
    pix.b += offset;\n\
    pix.r += offset;\n\
    imageStore(dst[0], pos, pix);\n\
}\n\
\n\
void transform_block(in SliceContext sc)\n\
{\n\
    const ivec2 rct_coef = sc.slice_rct_coef;\n\
    const ivec2 start = ivec2(gl_LocalInvocationID) + sc.slice_pos;\n\
    const ivec2 end = sc.slice_pos + sc.slice_dim;\n\
\n\
    for (uint y = start.y; y < end.y; y += gl_WorkGroupSize.y)\n\
        for (uint x = start.x; x < end.x; x += gl_WorkGroupSize.x)\n\
            transform_sample(ivec2(x, y), rct_coef);\n\
}\n\
\n\
void main()\n\
{\n\
    const uint slice_idx = gl_WorkGroupID.y*gl_NumWorkGroups.x + gl_WorkGroupID.x;\n\
\n\
    if (slice_ctx[slice_idx].slice_coding_mode == 1)\n\
        bypass_block(slice_ctx[slice_idx]);\n\
    else\n\
        transform_block(slice_ctx[slice_idx]);\n\
}";

const char *ff_source_ffv1_enc_rct_comp = "\
ivec4 load_components(ivec2 pos)\n\
{\n\
    ivec4 pix = ivec4(imageLoad(src[0], pos));\n\
    if (planar_rgb != 0) {\n\
        for (int i = 1; i < (3 + transparency); i++)\n\
            pix[i] = int(imageLoad(src[i], pos)[0]);\n\
    }\n\
\n\
    return ivec4(pix[fmt_lut[0]], pix[fmt_lut[1]],\n\
                 pix[fmt_lut[2]], pix[fmt_lut[3]]);\n\
}\n\
\n\
void bypass_sample(ivec2 pos)\n\
{\n\
    imageStore(dst[0], pos, load_components(pos));\n\
}\n\
\n\
void bypass_block(in SliceContext sc)\n\
{\n\
    ivec2 start = ivec2(gl_LocalInvocationID) + sc.slice_pos;\n\
    ivec2 end = sc.slice_pos + sc.slice_dim;\n\
    for (uint y = start.y; y < end.y; y += gl_WorkGroupSize.y)\n\
        for (uint x = start.x; x < end.x; x += gl_WorkGroupSize.x)\n\
            bypass_sample(ivec2(x, y));\n\
}\n\
\n\
void transform_sample(ivec2 pos, ivec2 rct_coef)\n\
{\n\
    ivec4 pix = load_components(pos);\n\
    pix.b -= pix.g;\n\
    pix.r -= pix.g;\n\
    pix.g += (pix.r*rct_coef.x + pix.b*rct_coef.y) >> 2;\n\
    pix.b += offset;\n\
    pix.r += offset;\n\
    imageStore(dst[0], pos, pix);\n\
}\n\
\n\
void transform_block(in SliceContext sc)\n\
{\n\
    const ivec2 rct_coef = sc.slice_rct_coef;\n\
    const ivec2 start = ivec2(gl_LocalInvocationID) + sc.slice_pos;\n\
    const ivec2 end = sc.slice_pos + sc.slice_dim;\n\
\n\
    for (uint y = start.y; y < end.y; y += gl_WorkGroupSize.y)\n\
        for (uint x = start.x; x < end.x; x += gl_WorkGroupSize.x)\n\
            transform_sample(ivec2(x, y), rct_coef);\n\
}\n\
\n\
void main()\n\
{\n\
    const uint slice_idx = gl_WorkGroupID.y*gl_NumWorkGroups.x + gl_WorkGroupID.x;\n\
\n\
    if (slice_ctx[slice_idx].slice_coding_mode == 1)\n\
        bypass_block(slice_ctx[slice_idx]);\n\
    else\n\
        transform_block(slice_ctx[slice_idx]);\n\
}";

const char *ff_source_ffv1_dec_setup_comp = "\
uint8_t setup_state[CONTEXT_SIZE];\n\
\n\
uint get_usymbol(inout RangeCoder c)\n\
{\n\
    if (get_rac_direct(c, setup_state[0]))\n\
        return 0;\n\
\n\
    int e = 0;\n\
    while (get_rac_direct(c, setup_state[1 + min(e, 9)])) { // 1..10\n\
        e++;\n\
        if (e > 31) {\n\
            corrupt = true;\n\
            return 0;\n\
        }\n\
    }\n\
\n\
    uint a = 1;\n\
    for (int i = e - 1; i >= 0; i--) {\n\
        a <<= 1;\n\
        a |= uint(get_rac_direct(c, setup_state[22 + min(i, 9)]));  // 22..31\n\
    }\n\
\n\
    return a;\n\
}\n\
\n\
bool decode_slice_header(inout SliceContext sc)\n\
{\n\
    [[unroll]]\n\
    for (int i = 0; i < CONTEXT_SIZE; i++)\n\
        setup_state[i] = uint8_t(128);\n\
\n\
    uint sx = get_usymbol(sc.c);\n\
    uint sy = get_usymbol(sc.c);\n\
    uint sw = get_usymbol(sc.c) + 1;\n\
    uint sh = get_usymbol(sc.c) + 1;\n\
\n\
    if (sx < 0 || sy < 0 || sw <= 0 || sh <= 0 ||\n\
        sx > (gl_NumWorkGroups.x - sw) || sy > (gl_NumWorkGroups.y - sh) ||\n\
        corrupt) {\n\
        return true;\n\
    }\n\
\n\
    /* Set coordinates */\n\
    uint sxs = slice_coord(img_size.x, sx     , gl_NumWorkGroups.x, chroma_shift.x);\n\
    uint sxe = slice_coord(img_size.x, sx + sw, gl_NumWorkGroups.x, chroma_shift.x);\n\
    uint sys = slice_coord(img_size.y, sy     , gl_NumWorkGroups.y, chroma_shift.y);\n\
    uint sye = slice_coord(img_size.y, sy + sh, gl_NumWorkGroups.y, chroma_shift.y);\n\
\n\
    sc.slice_pos = ivec2(sxs, sys);\n\
    sc.slice_dim = ivec2(sxe - sxs, sye - sys);\n\
    sc.slice_rct_coef = ivec2(1, 1);\n\
    sc.slice_coding_mode = int(0);\n\
\n\
    for (uint i = 0; i < codec_planes; i++) {\n\
        uint idx = get_usymbol(sc.c);\n\
        if (idx >= quant_table_count)\n\
            return true;\n\
        sc.quant_table_idx[i] = uint8_t(idx);\n\
    }\n\
\n\
    get_usymbol(sc.c);\n\
    get_usymbol(sc.c);\n\
    get_usymbol(sc.c);\n\
\n\
    if (version >= 4) {\n\
        sc.slice_reset_contexts = get_rac_direct(sc.c, setup_state[0]);\n\
        sc.slice_coding_mode = get_usymbol(sc.c);\n\
        if (sc.slice_coding_mode != 1 && colorspace == 1) {\n\
            sc.slice_rct_coef.x = int(get_usymbol(sc.c));\n\
            sc.slice_rct_coef.y = int(get_usymbol(sc.c));\n\
            if (sc.slice_rct_coef.x + sc.slice_rct_coef.y > 4)\n\
                return true;\n\
        }\n\
    }\n\
\n\
    return false;\n\
}\n\
\n\
void golomb_init(inout SliceContext sc)\n\
{\n\
    if (version == 3 && micro_version > 1 || version > 3) {\n\
        setup_state[0] = uint8_t(129);\n\
        get_rac_direct(sc.c, setup_state[0]);\n\
    }\n\
\n\
    uint64_t ac_byte_count = sc.c.bytestream - sc.c.bytestream_start - 1;\n\
    init_get_bits(sc.gb, u8buf(sc.c.bytestream_start + ac_byte_count),\n\
                  sc.c.bytestream_end - sc.c.bytestream_start - ac_byte_count);\n\
}\n\
\n\
void main(void)\n\
{\n\
    const uint slice_idx = gl_WorkGroupID.y*gl_NumWorkGroups.x + gl_WorkGroupID.x;\n\
\n\
    u8buf bs = u8buf(slice_data + slice_offsets[2*slice_idx + 0]);\n\
    uint32_t slice_size = slice_offsets[2*slice_idx + 1];\n\
\n\
    rac_init_dec(slice_ctx[slice_idx].c,\n\
                 bs, slice_size);\n\
\n\
    if (slice_idx == (gl_NumWorkGroups.x*gl_NumWorkGroups.y - 1))\n\
        get_rac_equi(slice_ctx[slice_idx].c);\n\
\n\
    decode_slice_header(slice_ctx[slice_idx]);\n\
\n\
    if (golomb == 1)\n\
        golomb_init(slice_ctx[slice_idx]);\n\
\n\
    if (ec != 0 && check_crc != 0) {\n\
        uint32_t crc = crcref;\n\
        for (int i = 0; i < slice_size; i++)\n\
            crc = crc_ieee[(crc & 0xFF) ^ uint32_t(bs[i].v)] ^ (crc >> 8);\n\
\n\
        slice_status[2*slice_idx + 0] = crc;\n\
    }\n\
\n\
    slice_status[2*slice_idx + 1] = corrupt ? uint32_t(corrupt) : overread;\n\
}";

const char *ff_source_ffv1_dec_comp = "\
#ifndef GOLOMB\n\
#ifdef CACHED_SYMBOL_READER\n\
shared uint8_t state[CONTEXT_SIZE];\n\
#define READ(c, off) get_rac_direct(c, state[off])\n\
#else\n\
#define READ(c, off) get_rac(c, uint64_t(slice_state) + (state_off + off))\n\
#endif\n\
\n\
int get_isymbol(inout RangeCoder c, uint state_off)\n\
{\n\
    if (READ(c, 0))\n\
        return 0;\n\
\n\
    uint e = 1;\n\
    for (; e < 33; e++)\n\
        if (!READ(c, min(e, 10)))\n\
            break;\n\
\n\
    if (expectEXT(e == 1, false)) {\n\
        return READ(c, 11) ? -1 : 1;\n\
    } else if (expectEXT(e == 33, false)) {\n\
        corrupt = true;\n\
        return 0;\n\
    }\n\
\n\
    int a = 1;\n\
    for (uint i = e + 20; i >= 22; i--) {\n\
        a <<= 1;\n\
        a |= int(READ(c, min(i, 31)));\n\
    }\n\
\n\
    return READ(c, min(e + 10, 21)) ? -a : a;\n\
}\n\
\n\
void decode_line_pcm(inout SliceContext sc, ivec2 sp, int w, int y, int p, int bits)\n\
{\n\
#ifdef CACHED_SYMBOL_READER\n\
    if (gl_LocalInvocationID.x > 0)\n\
        return;\n\
#endif\n\
\n\
#ifndef RGB\n\
    if (p > 0 && p < 3) {\n\
        w >>= chroma_shift.x;\n\
        sp >>= chroma_shift;\n\
    }\n\
#endif\n\
\n\
    for (int x = 0; x < w; x++) {\n\
        uint v = 0;\n\
        for (int i = (bits - 1); i >= 0; i--)\n\
            v |= uint(get_rac_equi(sc.c)) << i;\n\
\n\
        imageStore(dec[p], sp + LADDR(ivec2(x, y)), uvec4(v));\n\
    }\n\
}\n\
\n\
void decode_line(inout SliceContext sc, ivec2 sp, int w,\n\
                 int y, int p, int bits, uint state_off,\n\
                 uint8_t quant_table_idx, const int run_index)\n\
{\n\
#ifndef RGB\n\
    if (p > 0 && p < 3) {\n\
        w >>= chroma_shift.x;\n\
        sp >>= chroma_shift;\n\
    }\n\
#endif\n\
\n\
    for (int x = 0; x < w; x++) {\n\
        ivec2 pr = get_pred(dec[p], sp, ivec2(x, y), 0, w,\n\
                            quant_table_idx, extend_lookup[quant_table_idx] > 0);\n\
\n\
        uint context_off = state_off + CONTEXT_SIZE*abs(pr[0]);\n\
#ifdef CACHED_SYMBOL_READER\n\
        u8buf sb = u8buf(uint64_t(slice_state) + context_off + gl_LocalInvocationID.x);\n\
        state[gl_LocalInvocationID.x] = sb.v;\n\
        barrier();\n\
        if (gl_LocalInvocationID.x == 0) {\n\
\n\
#endif\n\
\n\
            int diff = get_isymbol(sc.c, context_off);\n\
            if (pr[0] < 0)\n\
                diff = -diff;\n\
\n\
            uint v = zero_extend(pr[1] + diff, bits);\n\
            imageStore(dec[p], sp + LADDR(ivec2(x, y)), uvec4(v));\n\
\n\
#ifdef CACHED_SYMBOL_READER\n\
        }\n\
\n\
        barrier();\n\
        sb.v = state[gl_LocalInvocationID.x];\n\
#endif\n\
    }\n\
}\n\
\n\
#else /* GOLOMB */\n\
\n\
void decode_line(inout SliceContext sc, ivec2 sp, int w,\n\
                 int y, int p, int bits, uint state_off,\n\
                 uint8_t quant_table_idx, inout int run_index)\n\
{\n\
#ifndef RGB\n\
    if (p > 0 && p < 3) {\n\
        w >>= chroma_shift.x;\n\
        sp >>= chroma_shift;\n\
    }\n\
#endif\n\
\n\
    int run_count = 0;\n\
    int run_mode  = 0;\n\
\n\
    for (int x = 0; x < w; x++) {\n\
        ivec2 pos = sp + ivec2(x, y);\n\
        int diff;\n\
        ivec2 pr = get_pred(dec[p], sp, ivec2(x, y), 0, w,\n\
                            quant_table_idx, extend_lookup[quant_table_idx] > 0);\n\
\n\
        uint context_off = state_off + VLC_STATE_SIZE*abs(pr[0]);\n\
        VlcState sb = VlcState(uint64_t(slice_state) + context_off);\n\
\n\
        if (pr[0] == 0 && run_mode == 0)\n\
            run_mode = 1;\n\
\n\
        if (run_mode != 0) {\n\
            if (run_count == 0 && run_mode == 1) {\n\
                int tmp_idx = int(log2_run[run_index]);\n\
                if (get_bit(sc.gb)) {\n\
                    run_count = 1 << tmp_idx;\n\
                    if (x + run_count <= w)\n\
                        run_index++;\n\
                } else {\n\
                    if (tmp_idx != 0) {\n\
                        run_count = int(get_bits(sc.gb, tmp_idx));\n\
                    } else\n\
                        run_count = 0;\n\
\n\
                    if (run_index != 0)\n\
                        run_index--;\n\
                    run_mode = 2;\n\
                }\n\
            }\n\
\n\
            run_count--;\n\
            if (run_count < 0) {\n\
                run_mode  = 0;\n\
                run_count = 0;\n\
                diff = read_vlc_symbol(sc.gb, sb, bits);\n\
                if (diff >= 0)\n\
                    diff++;\n\
            } else {\n\
                diff = 0;\n\
            }\n\
        } else {\n\
            diff = read_vlc_symbol(sc.gb, sb, bits);\n\
        }\n\
\n\
        if (pr[0] < 0)\n\
            diff = -diff;\n\
\n\
        uint v = zero_extend(pr[1] + diff, bits);\n\
        imageStore(dec[p], sp + LADDR(ivec2(x, y)), uvec4(v));\n\
    }\n\
}\n\
#endif\n\
\n\
#ifdef RGB\n\
ivec4 transform_sample(ivec4 pix, ivec2 rct_coef)\n\
{\n\
    pix.b -= rct_offset;\n\
    pix.r -= rct_offset;\n\
    pix.g -= (pix.b*rct_coef.y + pix.r*rct_coef.x) >> 2;\n\
    pix.b += pix.g;\n\
    pix.r += pix.g;\n\
    return ivec4(pix[fmt_lut[0]], pix[fmt_lut[1]],\n\
                 pix[fmt_lut[2]], pix[fmt_lut[3]]);\n\
}\n\
\n\
void writeout_rgb(in SliceContext sc, ivec2 sp, int w, int y, bool apply_rct)\n\
{\n\
    for (uint x = gl_LocalInvocationID.x; x < w; x += gl_WorkGroupSize.x) {\n\
        ivec2 lpos = sp + LADDR(ivec2(x, y));\n\
        ivec2 pos = sc.slice_pos + ivec2(x, y);\n\
\n\
        ivec4 pix;\n\
        pix.r = int(imageLoad(dec[2], lpos)[0]);\n\
        pix.g = int(imageLoad(dec[0], lpos)[0]);\n\
        pix.b = int(imageLoad(dec[1], lpos)[0]);\n\
        if (transparency != 0)\n\
            pix.a = int(imageLoad(dec[3], lpos)[0]);\n\
\n\
        if (expectEXT(apply_rct, true))\n\
            pix = transform_sample(pix, sc.slice_rct_coef);\n\
\n\
        imageStore(dst[0], pos, pix);\n\
        if (planar_rgb != 0) {\n\
            for (int i = 1; i < color_planes; i++)\n\
                imageStore(dst[i], pos, ivec4(pix[i]));\n\
        }\n\
    }\n\
}\n\
#endif\n\
\n\
void decode_slice(inout SliceContext sc, const uint slice_idx)\n\
{\n\
    int w = sc.slice_dim.x;\n\
    ivec2 sp = sc.slice_pos;\n\
\n\
#ifndef RGB\n\
    int bits = bits_per_raw_sample;\n\
#else\n\
    int bits = 9;\n\
    if (bits != 8 || sc.slice_coding_mode != 0)\n\
        bits = bits_per_raw_sample + int(sc.slice_coding_mode != 1);\n\
\n\
    sp.y = int(gl_WorkGroupID.y)*RGB_LINECACHE;\n\
#endif\n\
\n\
    /* PCM coding */\n\
#ifndef GOLOMB\n\
    if (sc.slice_coding_mode == 1) {\n\
#ifndef RGB\n\
        for (int p = 0; p < planes; p++) {\n\
            int h = sc.slice_dim.y;\n\
            if (p > 0 && p < 3)\n\
                h >>= chroma_shift.y;\n\
\n\
            for (int y = 0; y < h; y++)\n\
                decode_line_pcm(sc, sp, w, y, p, bits);\n\
        }\n\
#else\n\
        for (int y = 0; y < sc.slice_dim.y; y++) {\n\
            for (int p = 0; p < color_planes; p++)\n\
                decode_line_pcm(sc, sp, w, y, p, bits);\n\
\n\
            writeout_rgb(sc, sp, w, y, false);\n\
        }\n\
#endif\n\
    } else\n\
\n\
    /* Arithmetic coding */\n\
#endif\n\
    {\n\
        u8vec4 quant_table_idx = sc.quant_table_idx.xyyz;\n\
        u32vec4 slice_state_off = (slice_idx*codec_planes + uvec4(0, 1, 1, 2))*plane_state_size;\n\
\n\
#ifndef RGB\n\
        for (int p = 0; p < planes; p++) {\n\
            int h = sc.slice_dim.y;\n\
            if (p > 0 && p < 3)\n\
                h >>= chroma_shift.y;\n\
\n\
            int run_index = 0;\n\
            for (int y = 0; y < h; y++)\n\
                decode_line(sc, sp, w, y, p, bits,\n\
                            slice_state_off[p], quant_table_idx[p], run_index);\n\
        }\n\
#else\n\
        int run_index = 0;\n\
        for (int y = 0; y < sc.slice_dim.y; y++) {\n\
            for (int p = 0; p < color_planes; p++)\n\
                decode_line(sc, sp, w, y, p, bits,\n\
                            slice_state_off[p], quant_table_idx[p], run_index);\n\
\n\
            writeout_rgb(sc, sp, w, y, true);\n\
        }\n\
#endif\n\
    }\n\
}\n\
\n\
void main(void)\n\
{\n\
    const uint slice_idx = gl_WorkGroupID.y*gl_NumWorkGroups.x + gl_WorkGroupID.x;\n\
    decode_slice(slice_ctx[slice_idx], slice_idx);\n\
\n\
    uint32_t status = corrupt ? uint32_t(corrupt) : overread;\n\
    if (status != 0)\n\
        slice_status[2*slice_idx + 1] = status;\n\
}";
