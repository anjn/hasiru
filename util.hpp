#pragma once

#include <string>
#include <regex>
#include <vector>

#include "vpacker32.hpp"

#define DUMP(x) std::cout << #x << ": " << (x) << std::endl

std::vector<std::wstring>
wregex_split(std::wstring const& str, std::wregex const& re, int const limit = -1);

std::wstring
wstrip(std::wstring const& str);

std::vector<std::wstring>
make_bigram(std::wstring const& str);

void
encode_ids(uint32_t* const ids, unsigned size, char* dst, size_t* dst_size);

void
decode_ids(char const* src, size_t src_size,
    std::vector<std::pair<uint32_t, uint32_t>>& ids);
