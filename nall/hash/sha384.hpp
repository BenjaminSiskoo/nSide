#pragma once

#include <nall/hash/hash.hpp>

namespace nall { namespace Hash {

struct SHA384 : Hash {
  nallHash(SHA384)

  auto reset() -> void override {
    for(auto& n : queue) n = 0;
    for(auto& n : w) n = 0;
    for(auto  n : range(8)) h[n] = square(n);
    queued = length = 0;
  }

  auto input(uint8_t data) -> void override {
    byte(data);
    length++;
  }

  auto output() const -> vector<uint8_t> override {
    SHA384 self(*this);
    self.finish();
    vector<uint8_t> result;
    for(auto h : range(6)) {
      for(auto n : rrange(8)) result.append(self.h[h] >> n * 8);
    }
    return result;
  }

  auto value() const -> uint512_t {
    uint512_t value = 0;
    for(auto byte : output()) value = value << 8 | byte;
    return value;
  }

private:
  auto byte(uint8_t data) -> void {
    uint64_t shift = (7 - (queued & 7)) * 8;
    queue[queued >> 3] &=~((uint64_t)0xff << shift);
    queue[queued >> 3] |= ((uint64_t)data << shift);
    if(++queued == 128) block(), queued = 0;
  }

  auto block() -> void {
    for(auto n : range(16)) w[n] = queue[n];
    for(auto n : range(16, 80)) {
      uint64_t a = ror(w[n - 15],  1) ^ ror(w[n - 15],  8) ^ (w[n - 15] >> 7);
      uint64_t b = ror(w[n -  2], 19) ^ ror(w[n -  2], 61) ^ (w[n -  2] >> 6);
      w[n] = w[n - 16] + w[n - 7] + a + b;
    }
    uint64_t t[8];
    for(auto n : range(8)) t[n] = h[n];
    for(auto n : range(80)) {
      uint64_t a = ror(t[0], 28) ^ ror(t[0], 34) ^ ror(t[0], 39);
      uint64_t b = ror(t[4], 14) ^ ror(t[4], 18) ^ ror(t[4], 41);
      uint64_t c = (t[0] & t[1]) ^ (t[0] & t[2]) ^ (t[1] & t[2]);
      uint64_t d = (t[4] & t[5]) ^ (~t[4] & t[6]);
      uint64_t e = t[7] + w[n] + cube(n) + b + d;
      t[7] = t[6]; t[6] = t[5]; t[5] = t[4]; t[4] = t[3] + e;
      t[3] = t[2]; t[2] = t[1]; t[1] = t[0]; t[0] = a + c + e;
    }
    for(auto n : range(8)) h[n] += t[n];
  }

  auto finish() -> void {
    byte(0x80);
    while(queued != 112) byte(0x00);
    for(auto n : range(16)) byte(length * 8 >> (15 - n) * 8);
  }

  auto square(uint n) -> uint64_t {
    static const uint64_t data[8] = {
      0xcbbb9d5dc1059ed8, 0x629a292a367cd507, 0x9159015a3070dd17, 0x152fecd8f70e5939,
      0x67332667ffc00b31, 0x8eb44a8768581511, 0xdb0c2e0d64f98fa7, 0x47b5481dbefa4fa4,
    };
    return data[n];
  }

  auto cube(uint n) -> uint64_t {
    static const uint64_t data[80] = {
      0x428a2f98d728ae22, 0x7137449123ef65cd, 0xb5c0fbcfec4d3b2f, 0xe9b5dba58189dbbc,
      0x3956c25bf348b538, 0x59f111f1b605d019, 0x923f82a4af194f9b, 0xab1c5ed5da6d8118,
      0xd807aa98a3030242, 0x12835b0145706fbe, 0x243185be4ee4b28c, 0x550c7dc3d5ffb4e2,
      0x72be5d74f27b896f, 0x80deb1fe3b1696b1, 0x9bdc06a725c71235, 0xc19bf174cf692694,
      0xe49b69c19ef14ad2, 0xefbe4786384f25e3, 0x0fc19dc68b8cd5b5, 0x240ca1cc77ac9c65,
      0x2de92c6f592b0275, 0x4a7484aa6ea6e483, 0x5cb0a9dcbd41fbd4, 0x76f988da831153b5,
      0x983e5152ee66dfab, 0xa831c66d2db43210, 0xb00327c898fb213f, 0xbf597fc7beef0ee4,
      0xc6e00bf33da88fc2, 0xd5a79147930aa725, 0x06ca6351e003826f, 0x142929670a0e6e70,
      0x27b70a8546d22ffc, 0x2e1b21385c26c926, 0x4d2c6dfc5ac42aed, 0x53380d139d95b3df,
      0x650a73548baf63de, 0x766a0abb3c77b2a8, 0x81c2c92e47edaee6, 0x92722c851482353b,
      0xa2bfe8a14cf10364, 0xa81a664bbc423001, 0xc24b8b70d0f89791, 0xc76c51a30654be30,
      0xd192e819d6ef5218, 0xd69906245565a910, 0xf40e35855771202a, 0x106aa07032bbd1b8,
      0x19a4c116b8d2d0c8, 0x1e376c085141ab53, 0x2748774cdf8eeb99, 0x34b0bcb5e19b48a8,
      0x391c0cb3c5c95a63, 0x4ed8aa4ae3418acb, 0x5b9cca4f7763e373, 0x682e6ff3d6b2b8a3,
      0x748f82ee5defb2fc, 0x78a5636f43172f60, 0x84c87814a1f0ab72, 0x8cc702081a6439ec,
      0x90befffa23631e28, 0xa4506cebde82bde9, 0xbef9a3f7b2c67915, 0xc67178f2e372532b,
      0xca273eceea26619c, 0xd186b8c721c0c207, 0xeada7dd6cde0eb1e, 0xf57d4f7fee6ed178,
      0x06f067aa72176fba, 0x0a637dc5a2c898a6, 0x113f9804bef90dae, 0x1b710b35131c471b,
      0x28db77f523047d84, 0x32caab7b40c72493, 0x3c9ebe0a15c9bebc, 0x431d67c49c100d4c,
      0x4cc5d4becb3e42b6, 0x597f299cfc657e2a, 0x5fcb6fab3ad6faec, 0x6c44198c4a475817,
    };
    return data[n];
  }

  uint64_t queue[16] = {0};
  uint64_t w[80] = {0};
  uint64_t h[8] = {0};
  uint64_t queued = 0;
  uint128_t length = 0;
};

}}
