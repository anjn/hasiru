#include "util.hpp"

std::vector<std::wstring>
wregex_split(std::wstring const& str, std::wregex const& re, int const limit)
{
  std::vector<std::wstring> values;
  std::wsmatch m;
  auto it = str.cbegin();
  auto end = str.cend();
  while ((limit < 0 || int(values.size()) < limit-1) &&
      std::regex_search(it, end, m, re)){
    values.emplace_back(it, m[0].first);
    //for (unsigned i=1; i<m.size(); i++) values.push_back(m[i].str());
    it = m[0].second;
  }
  if (it != end) values.emplace_back(it, end);
  return values;
}

std::wstring
wstrip(std::wstring const& str)
{
  static std::wregex re(LR"((^\s+|\s+$))");
  return std::regex_replace(str, re, L"");
}

std::vector<std::wstring>
make_bigram(std::wstring const& str)
{
  std::vector<std::wstring> result;
  if (str.size() < 2) return result;
  
  result.reserve(str.size()-1);
  auto it = str.cbegin();
  auto end = str.cend()-1;
  while (it != end) {
    result.emplace_back(it, it+2);
    it++;
  }
  return result;
}

static const bool compress = !false;

void
encode_ids(uint32_t* const ids, unsigned size, char* dst, size_t* dst_size) {
  if (compress) {
    {
      uint32_t* p = ids;
      uint32_t last_id, last_pos;
      last_id  = *(p++);
      last_pos = *(p++);
      for (unsigned i=1; i<size; i++) {
        uint32_t id  = p[0];
        uint32_t pos = p[1];
        p[0] = id - last_id;
        if (id == last_id) p[1] = pos - last_pos;
        p += 2;
        last_id = id;
        last_pos = pos;
      }
    }
    *(uint32_t*)dst = size;
    *dst_size = 4 + vpacker32::Compress(ids, dst+4, size*2);
  } else {
    *dst_size = size*8;
    memcpy(dst, ids, *dst_size);
  }
}

void
decode_ids(char const* src, size_t src_size,
    std::vector<std::pair<uint32_t, uint32_t>>& ids)
{
  size_t size;
  if (compress) {
    size = *(uint32_t*)src;
    ids.resize(size);
    vpacker32::Uncompress(src+4, (uint32_t*)&ids[0], size*2);
    {
      uint32_t* p = (uint32_t*)&ids[0];
      uint32_t last_id, last_pos;
      last_id  = *(p++);
      last_pos = *(p++);
      for (unsigned i=1; i<size; i++) {
        uint32_t id  = p[0] + last_id;
        uint32_t pos = p[1];
        p[0] = id;
        if (id == last_id) p[1] = pos = pos + last_pos;
        p += 2;
        last_id = id;
        last_pos = pos;
      }
    }
  } else {
    size = src_size/8;
    ids.resize(size);
    memcpy(&ids[0], src, src_size);
  }
}

