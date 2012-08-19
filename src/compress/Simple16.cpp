/*-----------------------------------------------------------------------------
 *  Simple16.hpp - A encoder/decoder for Simple16
 *
 *  Coding-Style: google-styleguide
 *      https://code.google.com/p/google-styleguide/
 *
 *  Authors:
 *      Takeshi Yamamuro <linguin.m.s_at_gmail.com>
 *      Fabrizio Silvestri <fabrizio.silvestri_at_isti.cnr.it>
 *      Rossano Venturini <rossano.venturini_at_isti.cnr.it>
 *
 *  Copyright 2012 Integer Encoding Library <integerencoding_at_isti.cnr.it>
 *      http://integerencoding.ist.cnr.it/
 *-----------------------------------------------------------------------------
 */

#include <compress/policy/Simple16.hpp>

namespace integer_encoding {
namespace internals {

Simple16::Simple16()
    : Simple9(E_SIMPLE16) {
  unpack_[0] = unpack28_1;
  unpack_[1] = unpack7_2_14_1;
  unpack_[2] = unpack7_1_7_2_7_1;
  unpack_[3] = unpack14_1_7_2;
  unpack_[4] = unpack14_2;
  unpack_[5] = unpack1_4_8_3;
  unpack_[6] = unpack1_3_4_4_3_3;
  unpack_[7] = unpack7_4;
  unpack_[8] = unpack4_5_2_4;
  unpack_[9] = unpack2_4_4_5;
  unpack_[10] = unpack3_6_2_5;
  unpack_[11] = unpack2_5_3_6;
  unpack_[12] = unpack4_7;
  unpack_[13] = unpack1_10_2_9;
  unpack_[14] = unpack2_14;
  unpack_[15] = unpack1_28;
}

Simple16::~Simple16() throw() {}

void Simple16::encodeArray(const uint32_t *in,
                           uint64_t len,
                           uint32_t *out,
                           uint64_t *nvalue) const {
  if (in == NULL)
    THROW_ENCODING_EXCEPTION("Invalid input: in");
  if (len == 0)
    THROW_ENCODING_EXCEPTION("Invalid input: len");
  if (out == NULL)
    THROW_ENCODING_EXCEPTION("Invalid input: out");
  if (*nvalue == 0)
    THROW_ENCODING_EXCEPTION("Invalid input: nvalue");

  ASSERT_ADDR(in , len);
  ASSERT_ADDR(out, *nvalue);

  BitsWriter  wt(out, *nvalue);

#define WRITE_SIMPLE16_ENTRY(__num__, __bit__)  \
    ({  \
      uint32_t num = (len < __num__)? len : __num__;  \
      for (uint32_t i = 0; i < num; i++)  \
        wt.write_bits(*in++, __bit__);    \
      len -= num;   \
    })

  while (len > 0) {
    if (Simple9::try_pack(in, len, 28, 1)) {
      wt.write_bits(0, 4);
      WRITE_SIMPLE16_ENTRY(28, 1);
    } else if (try_pack(in, len,
                        7, 2, 14, 1)) {
      wt.write_bits(1, 4);
      WRITE_SIMPLE16_ENTRY(7, 2);
      WRITE_SIMPLE16_ENTRY(14, 1);
    } else if (try_pack(in, len,
                        7, 1, 7, 2, 7, 1)) {
      wt.write_bits(2, 4);
      WRITE_SIMPLE16_ENTRY(7, 1);
      WRITE_SIMPLE16_ENTRY(7, 2);
      WRITE_SIMPLE16_ENTRY(7, 1);
    } else if (try_pack(in, len,
                        14, 1, 7, 2)) {
      wt.write_bits(3, 4);
      WRITE_SIMPLE16_ENTRY(14, 1);
      WRITE_SIMPLE16_ENTRY(7, 2);
    } else if (Simple9::try_pack(in, len, 14, 2)) {
      wt.write_bits(4, 4);
      WRITE_SIMPLE16_ENTRY(14, 2);
    } else if (try_pack(in, len,
                        1, 4, 8, 3)) {
      wt.write_bits(5, 4);
      WRITE_SIMPLE16_ENTRY(1, 4);
      WRITE_SIMPLE16_ENTRY(8, 3);
    } else if (try_pack(in, len,
                        1, 3, 4, 4, 3, 3)) {
      wt.write_bits(6, 4);
      WRITE_SIMPLE16_ENTRY(1, 3);
      WRITE_SIMPLE16_ENTRY(4, 4);
      WRITE_SIMPLE16_ENTRY(3, 3);
    } else if (Simple9::try_pack(in, len, 7, 4)) {
      wt.write_bits(7, 4);
      WRITE_SIMPLE16_ENTRY(7, 4);
    } else if (try_pack(in, len,
                        4, 5, 2, 4)) {
      wt.write_bits(8, 4);
      WRITE_SIMPLE16_ENTRY(4, 5);
      WRITE_SIMPLE16_ENTRY(2, 4);
    } else if (try_pack(in, len,
                        2, 4, 4, 5)) {
      wt.write_bits(9, 4);
      WRITE_SIMPLE16_ENTRY(2, 4);
      WRITE_SIMPLE16_ENTRY(4, 5);
    } else if (try_pack(in, len,
                        3, 6, 2, 5)) {
      wt.write_bits(10, 4);
      WRITE_SIMPLE16_ENTRY(3, 6);
      WRITE_SIMPLE16_ENTRY(2, 5);
    } else if (try_pack(in, len,
                        2, 5, 3, 6)) {
      wt.write_bits(11, 4);
      WRITE_SIMPLE16_ENTRY(2, 5);
      WRITE_SIMPLE16_ENTRY(3, 6);
    } else if (Simple9::try_pack(in, len, 4, 7)) {
      wt.write_bits(12, 4);
      WRITE_SIMPLE16_ENTRY(4, 7);
    } else if (try_pack(in, len,
                        1, 10, 2, 9)) {
      wt.write_bits(13, 4);
      WRITE_SIMPLE16_ENTRY(1, 10);
      WRITE_SIMPLE16_ENTRY(2, 9);
    } else if (Simple9::try_pack(in, len, 2, 14)) {
      wt.write_bits(14, 4);
      WRITE_SIMPLE16_ENTRY(2, 14);
    } else {
      if ((*in >> 28) > 0)
        THROW_ENCODING_EXCEPTION("Out of value range: *in");

      /* Write a value as it is */
      wt.write_bits(15, 4);
      wt.write_bits(*in++, 28);
      len--;
    }

    wt.flush_bits();
  }

  *nvalue = wt.size();
}

void Simple16::decodeArray(const uint32_t *in,
                           uint64_t len,
                           uint32_t *out,
                           uint64_t nvalue) const {
  if (in == NULL)
    THROW_ENCODING_EXCEPTION("Invalid input: in");
  if (len == 0)
    THROW_ENCODING_EXCEPTION("Invalid input: len");
  if (out == NULL)
    THROW_ENCODING_EXCEPTION("Invalid input: out");
  if (nvalue == 0)
    THROW_ENCODING_EXCEPTION("Invalid input: nvalue");

  ASSERT_ADDR(in, len);
  ASSERT_ADDR(out, nvalue);

  const uint32_t *iterm = in + len;
  uint32_t *oterm = out + nvalue;

  while (1) {
    if (UNLIKELY(out >= oterm || in >= iterm))
      break;
    (unpack_[*in >> (32 - SIMPLE9_LOGDESC)])(&out, &in);
  }
}

bool Simple16::try_pack(const uint32_t *in,
                        uint32_t len,
                        uint32_t num1, uint32_t log1,
                        uint32_t num2, uint32_t log2) {
  if (!Simple9::try_pack(in, len, num1, log1))
    return false;
  uint32_t base = (len < num1)? len : num1;
  if (!Simple9::try_pack(in + base, len - base,
                         num2, log2))
    return false;
  return true;
}

bool Simple16::try_pack(const uint32_t *in,
                        uint32_t len,
                        uint32_t num1, uint32_t log1,
                        uint32_t num2, uint32_t log2,
                        uint32_t num3, uint32_t log3) {
  if (!try_pack(in, len, num1, log1, num2, log2))
    return false;
  uint32_t base = (len < num1 + num2)? len : num1 + num2;
  if (!Simple9::try_pack(in + base, len - base,
                         num3, log3))
    return false;
  return true;
}

void Simple16::unpack7_2_14_1(uint32_t **out,
                              const uint32_t **in) {
  uint32_t *pout = *out;
  const uint32_t *pin = *in;

  pout[0] = (pin[0] >> 26) & 0x03;
  pout[1] = (pin[0] >> 24) & 0x03;
  pout[2] = (pin[0] >> 22) & 0x03;
  pout[3] = (pin[0] >> 20) & 0x03;
  pout[4] = (pin[0] >> 18) & 0x03;
  pout[5] = (pin[0] >> 16) & 0x03;
  pout[6] = (pin[0] >> 14) & 0x03;

  pout[7] = (pin[0] >> 13) & 0x01;
  pout[8] = (pin[0] >> 12) & 0x01;
  pout[9] = (pin[0] >> 11) & 0x01;
  pout[10] = (pin[0] >> 10) & 0x01;
  pout[11] = (pin[0] >> 9) & 0x01;
  pout[12] = (pin[0] >> 8) & 0x01;
  pout[13] = (pin[0] >> 7) & 0x01;
  pout[14] = (pin[0] >> 6) & 0x01;
  pout[15] = (pin[0] >> 5) & 0x01;
  pout[16] = (pin[0] >> 4) & 0x01;
  pout[17] = (pin[0] >> 3) & 0x01;
  pout[18] = (pin[0] >> 2) & 0x01;
  pout[19] = (pin[0] >> 1) & 0x01;
  pout[20] = pin[0] & 0x01;

  *in = pin + 1;
  *out = pout + 21;
}

void Simple16::unpack7_1_7_2_7_1(uint32_t **out,
                                 const uint32_t **in) {
  uint32_t *pout = *out;
  const uint32_t *pin = *in;

  pout[0] = (pin[0] >> 27) & 0x01;
  pout[1] = (pin[0] >> 26) & 0x01;
  pout[2] = (pin[0] >> 25) & 0x01;
  pout[3] = (pin[0] >> 24) & 0x01;
  pout[4] = (pin[0] >> 23) & 0x01;
  pout[5] = (pin[0] >> 22) & 0x01;
  pout[6] = (pin[0] >> 21) & 0x01;

  pout[7] = (pin[0] >> 19) & 0x03;
  pout[8] = (pin[0] >> 17) & 0x03;
  pout[9] = (pin[0] >> 15) & 0x03;
  pout[10] = (pin[0] >> 13) & 0x03;
  pout[11] = (pin[0] >> 11) & 0x03;
  pout[12] = (pin[0] >> 9) & 0x03;
  pout[13] = (pin[0] >> 7) & 0x03;

  pout[14] = (pin[0] >> 6) & 0x01;
  pout[15] = (pin[0] >> 5) & 0x01;
  pout[16] = (pin[0] >> 4) & 0x01;
  pout[17] = (pin[0] >> 3) & 0x01;
  pout[18] = (pin[0] >> 2) & 0x01;
  pout[19] = (pin[0] >> 1) & 0x01;
  pout[20] = pin[0] & 0x01;

  *in = pin + 1;
  *out = pout + 21;
}

void Simple16::unpack14_1_7_2(uint32_t **out,
                              const uint32_t **in) {
  uint32_t *pout = *out;
  const uint32_t *pin = *in;

  pout[0] = (pin[0] >> 27) & 0x01;
  pout[1] = (pin[0] >> 26) & 0x01;
  pout[2] = (pin[0] >> 25) & 0x01;
  pout[3] = (pin[0] >> 24) & 0x01;
  pout[4] = (pin[0] >> 23) & 0x01;
  pout[5] = (pin[0] >> 22) & 0x01;
  pout[6] = (pin[0] >> 21) & 0x01;
  pout[7] = (pin[0] >> 20) & 0x01;
  pout[8] = (pin[0] >> 19) & 0x01;
  pout[9] = (pin[0] >> 18) & 0x01;
  pout[10] = (pin[0] >> 17) & 0x01;
  pout[11] = (pin[0] >> 16) & 0x01;
  pout[12] = (pin[0] >> 15) & 0x01;
  pout[13] = (pin[0] >> 14) & 0x01;

  pout[14] = (pin[0] >> 12) & 0x03;
  pout[15] = (pin[0] >> 10) & 0x03;
  pout[16] = (pin[0] >> 8) & 0x03;
  pout[17] = (pin[0] >> 6) & 0x03;
  pout[18] = (pin[0] >> 4) & 0x03;
  pout[19] = (pin[0] >> 2) & 0x03;
  pout[20] = pin[0] & 0x03;

  *in = pin + 1;
  *out = pout + 21;
}

void Simple16::unpack1_4_8_3(uint32_t **out,
                             const uint32_t **in) {
  uint32_t *pout = *out;
  const uint32_t *pin = *in;

  pout[0] = (pin[0] >> 24) & 0x0f;

  pout[1] = (pin[0] >> 21) & 0x07;
  pout[2] = (pin[0] >> 18) & 0x07;
  pout[3] = (pin[0] >> 15) & 0x07;
  pout[4] = (pin[0] >> 12) & 0x07;
  pout[5] = (pin[0] >> 9) & 0x07;
  pout[6] = (pin[0] >> 6) & 0x07;
  pout[7] = (pin[0] >> 3) & 0x07;
  pout[8] = pin[0] & 0x07;

  *in = pin + 1;
  *out = pout + 9;
}

void Simple16::unpack1_3_4_4_3_3(uint32_t **out,
                                 const uint32_t **in) {
  uint32_t *pout = *out;
  const uint32_t *pin = *in;

  pout[0] = (pin[0] >> 25) & 0x07;

  pout[1] = (pin[0] >> 21) & 0x0f;
  pout[2] = (pin[0] >> 17) & 0x0f;
  pout[3] = (pin[0] >> 13) & 0x0f;
  pout[4] = (pin[0] >> 9) & 0x0f;

  pout[5] = (pin[0] >> 6) & 0x07;
  pout[6] = (pin[0] >> 3) & 0x07;
  pout[7] = pin[0] & 0x07;

  *in = pin + 1;
  *out = pout + 8;
}

void Simple16::unpack4_5_2_4(uint32_t **out,
                             const uint32_t **in) {
  uint32_t *pout = *out;
  const uint32_t *pin = *in;

  pout[0] = (pin[0] >> 23) & 0x1f;
  pout[1] = (pin[0] >> 18) & 0x1f;
  pout[2] = (pin[0] >> 13) & 0x1f;
  pout[3] = (pin[0] >> 8) & 0x1f;

  pout[4] = (pin[0] >> 4) & 0x0f;
  pout[5] = pin[0] & 0x0f;

  *in = pin + 1;
  *out = pout + 6;
}

void Simple16::unpack2_4_4_5(uint32_t **out,
                             const uint32_t **in) {
  uint32_t *pout = *out;
  const uint32_t *pin = *in;

  pout[0] = (pin[0] >> 24) & 0x0f;
  pout[1] = (pin[0] >> 20) & 0x0f;

  pout[2] = (pin[0] >> 15) & 0x1f;
  pout[3] = (pin[0] >> 10) & 0x1f;
  pout[4] = (pin[0] >> 5) & 0x1f;
  pout[5] = pin[0] & 0x1f;

  *in = pin + 1;
  *out = pout + 6;
}

void Simple16::unpack3_6_2_5(uint32_t **out,
                             const uint32_t **in) {
  uint32_t *pout = *out;
  const uint32_t *pin = *in;

  pout[0] = (pin[0] >> 22) & 0x3f;
  pout[1] = (pin[0] >> 16) & 0x3f;
  pout[2] = (pin[0] >> 10) & 0x3f;

  pout[3] = (pin[0] >> 5) & 0x1f;
  pout[4] = pin[0] & 0x1f;

  *in = pin + 1;
  *out = pout + 5;
}

void Simple16::unpack2_5_3_6(uint32_t **out,
                             const uint32_t **in) {
  uint32_t *pout = *out;
  const uint32_t *pin = *in;

  pout[0] = (pin[0] >> 23) & 0x1f;
  pout[1] = (pin[0] >> 18) & 0x1f;

  pout[2] = (pin[0] >> 12) & 0x3f;
  pout[3] = (pin[0] >> 6) & 0x3f;
  pout[4] = pin[0] & 0x3f;

  *in = pin + 1;
  *out = pout + 5;
}

void Simple16::unpack1_10_2_9(uint32_t **out,
                              const uint32_t **in) {
  uint32_t *pout = *out;
  const uint32_t *pin = *in;

  pout[0] = (pin[0] >> 18) & 0x03ff;

  pout[1] = (pin[0] >> 9) & 0x01ff;
  pout[2] = pin[0] & 0x01ff;

  *in = pin + 1;
  *out = pout + 3;
}

} /* namespace: internals */
} /* namespace: integer_encoding */