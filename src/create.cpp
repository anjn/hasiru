#include <iostream>
#include <fstream>
#include <codecvt>
#include <string>
#include <regex>
#include <vector>
#include <map>
#include <cassert>

#include "cdbpp.h"

#include "util.hpp"

using namespace std;

const bool compress = !false;

int main(int argc, char** argv)
{
  std::ofstream ofs("dat/index.cdb", std::ios_base::binary);
  if (ofs.fail()) {
    std::cerr << "ERROR: Failed to open a database file." << std::endl;
    return 1;
  }

  std::ofstream ofs_table("dat/eiwa.table", std::ios_base::binary);
  std::ofstream ofs_dat("dat/eiwa.dat", std::ios_base::binary);

  wstring_convert<codecvt_utf8<wchar_t>,wchar_t> cv;

  std::map<std::wstring, std::vector<std::pair<uint32_t, uint32_t>>> key2id;
  uint32_t dat_pos = 0;
  std::vector<uint32_t> dat_pos_table;

  wregex re_sep(LR"( : )");
  wregex re_part(LR"((\{.+?\}))");
  wregex re_ruby(LR"(｛.+?｝)");
  wregex re_space(LR"(\s+)");
  wregex re_head(LR"(■)");

  uint32_t id = 0;
  string line;
  while (getline(cin, line)) {
    //cout << "input: " << line << endl;

    auto wline = cv.from_bytes(line);
    wline = regex_replace(wline, re_space, L" ");
    //wline = regex_replace(wline, re_head, L"");

    auto title_body = wregex_split(wline, re_sep, 2);
    assert(title_body.size() == 2);

    auto title_part = wregex_split(title_body[0], re_part);
    assert(title_part.size() <= 2);
    auto title = title_part[0];
    title = wstrip(title);

    wstring part = L"";
    if (title_part.size() > 1) part = title_part[1];

    auto body = title_body[1];
    body = regex_replace(body, re_ruby, L"");
    body = wstrip(body);

    auto title_body_dat = title + L"〓" + body;

    //cout << "  ^" << cv.to_bytes(title) << "$" << endl;
    //cout << "  ^" << cv.to_bytes(body)  << "$" << endl;

    uint32_t pos = 0;
    for (auto key: make_bigram(title)) {
      if (key[0] == L' ' || key[1] == L' ') continue;
      key2id[key].emplace_back(id, pos);
      pos++;
    }

    auto title_body_bytes = cv.to_bytes(title_body_dat);
    ofs_dat.write(&title_body_bytes[0], title_body_bytes.size());

    dat_pos_table.push_back(dat_pos);
    dat_pos += title_body_bytes.size();

    id++;

    if (id%10000==0) cout << id << endl;
  }

  dat_pos_table.push_back(dat_pos);
  ofs_table.write((char*)&dat_pos_table[0], sizeof(uint32_t)*dat_pos_table.size());

  wstring max_bigram;
  size_t max_size = 0;
  for (auto i: key2id) {
    if (max_size < i.second.size()) {
      max_size = i.second.size();
      max_bigram = i.first;
    }
  }
  cout << "max_size: " << max_size << endl;
  cout << "max_bigram: " << cv.to_bytes(max_bigram) << endl;

  vector<char> buf(vpacker32::CompressBound(max_size*2));

  cdbpp::builder db(ofs);
  for (auto i: key2id) {
    auto key = cv.to_bytes(i.first);
    size_t comp_size;
    encode_ids((uint32_t*)&i.second[0], i.second.size(), &buf[0], &comp_size);
    db.put(key.c_str(), key.size(), &buf[0], comp_size);
  }

  return 0;
}
