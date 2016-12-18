#include <sys/mman.h>
#include <fcntl.h>
#include <unistd.h>
#include <iostream>
#include <fstream>
#include <codecvt>
#include <string>
#include <regex>
#include <vector>
#include <map>
#include <cassert>

#include "cdbpp.h"
#include "vpacker32.hpp"

#include "util.hpp"
#include "hasiru_searcher.hpp"

using namespace std;

int main(int argc, char** argv)
{
  hasiru_searcher hasiru;
  hasiru.db_open();

  wstring_convert<codecvt_utf8<wchar_t>,wchar_t> cv;

  wregex re_sep(LR"( : )");
  wregex re_part(LR"((\{.+?\}))");
  wregex re_ruby(LR"(｛.+?｝)");
  wregex re_space(LR"(\s+)");
  wregex re_head(LR"(■)");

  int total;
  auto result = hasiru.search(argv[1], 0, 10, &total);
  for (auto a: result) {
    cout << a << endl;
  }

  /*
  wstring query = cv.from_bytes(argv[1]);
  auto found = hasiru.search_word(query);
  query = L"■" + wstrip(query);
  auto bigram = make_bigram(query);

  vector<vector<pair<uint32_t, uint32_t>>> id_pos(bigram.size());
  for (unsigned i=0; i<bigram.size(); i++) {
    auto key = cv.to_bytes(bigram[i]);
    hasiru.search_bigram(key, id_pos[i]);
  }

  vector<size_t> index(id_pos.size(), 0);
  vector<pair<uint32_t, uint32_t>> found;

  // (id,pos) -> value:uint64_t
  auto id2v = [](pair<uint32_t, uint32_t>& id) -> uint64_t {
    return (((uint64_t)id.first)<<32) | id.second;
  };

  for (auto i: id_pos[1]) {
    auto iv = id2v(i);
    bool match = true;
    bool end = false;
    for (unsigned j=2; j<id_pos.size() && match && !end; j++) {
      while (match) {
        if (index[j] >= id_pos[j].size()) { end = true; break; }
        auto ov = id2v(id_pos[j][index[j]++]);
        if (iv > ov) {
          continue;
        } else if (iv+j-1 == ov) {
          break;
        } else {
          index[j]--;
          match = false;
        }
      }
    }
    if (end) break;
    if (match) found.push_back(i);
  }

  for (unsigned i=0; i<10 && i<found.size(); i++) {
    int id = found[i].first;
    cout << hasiru.get(id) << endl;
  }

  cout << found.size() <<endl;
  */

  return 0;
}
