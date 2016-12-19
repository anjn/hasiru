#include <iostream>
#include <codecvt>
#include <string>
#include <regex>
#include <vector>

#include "util.hpp"

using namespace std;

int main(int argc, char** argv)
{
  wstring_convert<codecvt_utf8<wchar_t>,wchar_t> cv;

  string line;
  while (getline(cin, line)) {
    cout << "input: " << line << endl;

    wstring wline = cv.from_bytes(line);
    for (auto i : wregex_split(wline, wregex(LR"( : )"), 1)) {
      cout << cv.to_bytes(i) << endl;
    }
  }

  return 0;
}

