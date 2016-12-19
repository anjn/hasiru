#include <iostream>

#include "hasiru_searcher.hpp"

using namespace std;

int main(int argc, char** argv)
{
  hasiru_searcher hasiru;
  hasiru.db_open();

  int total;
  auto result = hasiru.search(argv[1], 0, 10, &total);
  for (auto a: result) {
    cout << a << endl;
  }

  return 0;
}
