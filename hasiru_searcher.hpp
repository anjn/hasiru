#pragma once
#include <sys/mman.h>
#include <fcntl.h>
#include <iostream>
#include <fstream>
#include <codecvt>
#include <string>
#include <regex>
#include <vector>

#include "cdbpp.h"

#include "util.hpp"

class hasiru_searcher {
public:
  hasiru_searcher():
    fd(-1), dat(nullptr)
  {}

  ~hasiru_searcher()
  {
    if (dat != nullptr) munmap(dat, dat_size);
    if (fd >= 0) close(fd);
    dat = nullptr;
    fd = -1;
  }

  bool db_open()
  {
    // index
    ifs_index.open("index.cdb", std::ios_base::binary);
    if (ifs_index.fail()) {
      std::cerr << "ERROR: Failed to open a database file." << std::endl;
      return false;
    }

    db_index.open(ifs_index);
    if (!db_index.is_open()) {
      std::cerr << "ERROR: Failed to read a database file." << std::endl;
      return false;
    }

    // table
    {
      std::ifstream ifs_table("eiwa.table", std::ios_base::binary);
      if (!ifs_table.is_open()) {
        return false;
      }

      ifs_table.seekg(0, ifs_table.end);
      int table_size = ifs_table.tellg();
      ifs_table.seekg(0, ifs_table.beg);

      dat_pos_table.resize(table_size/sizeof(uint32_t));
      ifs_table.read(reinterpret_cast<char*>(&dat_pos_table[0]), table_size);
    }

    // data
    dat_size = dat_pos_table.back();
    fd = open("eiwa.dat", O_RDWR, 0666);
    if (fd < 0) {
      return false;
    }
    dat = (char*) mmap(NULL, dat_size, PROT_READ, MAP_SHARED, fd, 0);
    if (dat == MAP_FAILED) {
      dat = nullptr;
      return false;
    }

    return true;
  }

  void search_bigram(std::string const& key,
      std::vector<std::pair<uint32_t, uint32_t>>& id_pos) const
  {
    size_t buf_size;
    char const* buf = reinterpret_cast<char const*>(db_index.get(key.c_str(), key.size(), &buf_size));
    if (buf != nullptr) decode_ids(buf, buf_size, id_pos);
  }

  std::vector<std::pair<uint32_t, uint32_t>>
  search_word(std::wstring const& word)
  {
    std::vector<std::pair<uint32_t, uint32_t>> result, tmp;
    auto bigram = make_bigram(word);
    for (unsigned i=0; i<bigram.size(); i++) {
      auto key = cv.to_bytes(bigram[i]);
      if (i==0) {
        search_bigram(key, result);
      } else if (!result.empty()) {
        search_bigram(key, tmp);

        auto it_tmp = tmp.begin();
        auto it_rm = std::remove_if(result.begin(), result.end(),
          [&](std::pair<uint32_t, uint32_t> const& id_pos) {
            auto jv = id2v(id_pos);
            bool match = false;
            while (it_tmp != tmp.end()) {
              auto ov = id2v(*it_tmp);
              if (jv > ov) {
                it_tmp++;
                continue;
              } else if (jv+i == ov) {
                it_tmp++;
                match = true;
              }
              break;
            }
            return !match;
          });
        result.erase(it_rm, result.end());
      }
    }
    return result;
  }

  std::vector<std::string>
  search(std::string const& query, int from, int count, int* total) {
    std::vector<std::string> result;

    auto wquery = cv.from_bytes(query);
    wquery = wstrip(wquery);

    if (wquery.empty()) {
      *total = dat_pos_table.size()-1;
      for (unsigned i=from; i<from+count; i++) {
        result.push_back(get(i));
      }
      return result;
    }

    if (wquery.size() == 1) wquery = L"■" + wquery;
    auto found = search_word(wquery);

    *total = found.size();
    for (unsigned i=from; i<from+count && i<found.size(); i++) {
      int id = found[i].first;
      result.push_back(get(id));
    }
    return result;
  }

  std::string get(int id) {
    if (0 <= id && id < dat_pos_table.size()-1) {
      return std::string(dat + dat_pos_table[id], dat + dat_pos_table[id+1]);
    } else {
      return "Invalid ID!!";
    }
  }

private:
  uint64_t id2v(std::pair<uint32_t, uint32_t> const& id) {
    return (((uint64_t)id.first)<<32) | id.second;
  };

  std::wstring_convert<std::codecvt_utf8<wchar_t>,wchar_t> cv;

  // index
  std::ifstream ifs_index;
  cdbpp::cdbpp db_index;

  // table
  std::vector<uint32_t> dat_pos_table;

  // data
  int fd;
  int dat_size;
  char* dat;
};
