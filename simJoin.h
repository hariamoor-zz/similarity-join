#pragma once
#include <algorithm>
#include <fstream>
#include <iostream>
#include <string>
#include <sys/time.h>
#include <vector>

using namespace std;

template <typename T1, typename T2, typename T3> struct triple {
  T1 id1;
  T2 id2;
  T3 ed;
};

class simJoin {
public:
  simJoin(const string &filename) { readData(filename); };

  ~simJoin(){};

  bool SimilarityJoin(unsigned ed_threshold,
                      vector<triple<unsigned, unsigned, unsigned>> &results);
  bool getString(int id, string &out) const;
  int getDataNum() const;

private:
  vector<string> data;
  bool readData(const string &filename);
};

unsigned int distance(const std::string &, const std::string &);
