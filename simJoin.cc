#include "simJoin.h"
#include <algorithm>
#include <cmath>
#include <math.h>
#include <map>
#include <memory>
#include <set>
#include <string>

int simJoin::getDataNum() const { return data.size(); }

bool simJoin::getString(int id, string &out) const {
  if (id < 0 || id >= data.size())
    return false;
  out = data[id];
  return true;
}

bool simJoin::readData(const string &filename) {
  string str;
  ifstream datafile(filename, ios::in);
  while (getline(datafile, str))
    data.emplace_back(str);

  return true;
}

/*
 * It should do a similarity join operation betweent the set of strings from a
 * data file such that the edit distance between two string is not larger than
 * the given threshold. The format of result is a triple of numbers which
 * respectively stand for the two IDs of the pair of strings from the data file
 * and the edit distance between the two strings. All results are stored in a
 * vector, sorted based on the IDs of the string from the first file and then
 * the IDs of the string from the second file in an ascending order. Return an
 * error if the similarity join operation is failed.
 */

class PassJoin {
  typedef std::map<std::string, std::set<int>> InvertedIndex;
  unsigned threshold;
  simJoin& joiner;
  std::vector<triple<unsigned, unsigned, unsigned>>& results;
private:
  std::map<int, std::map<int, InvertedIndex>> indices;
public:
  PassJoin(unsigned threshold, simJoin& joiner, std::vector<triple<unsigned, unsigned, unsigned>>& results) : threshold(threshold), joiner(joiner), results(results) {}
  bool Initialize() {
    int partitions = threshold + 1;
    for(int id = 0; id < joiner.getDataNum(); id++) {
      std::string curr;
      joiner.getString(id, curr);

      int l = curr.length();
      int k = l / partitions;

      for(int i = 0, left = partitions; left; i += k, left--) {
        std::string sub = (left == 1) ? curr.substr(i * k) : curr.substr(i * k, k);
        indices[l][i][sub].insert(id);
      }
    }

    return true;
  };
  bool Join();
  unsigned distance(const std::string& s1, const std::string& s2) {
    const std::size_t len1 = s1.size(), len2 = s2.size();
    std::vector<std::vector<unsigned>> d(len1 + 1, std::vector<unsigned>(len2 + 1));

    d[0][0] = 0;
    for(unsigned i = 1; i <= len1; ++i) d[i][0] = i;
    for(unsigned i = 1; i <= len2; ++i) d[0][i] = i;

    for(unsigned i = 1; i <= len1; ++i)
      for(unsigned j = 1; j <= len2; ++j)
        d[i][j] = std::min({ d[i - 1][j] + 1, d[i][j - 1] + 1, d[i - 1][j - 1]
            + (s1[i - 1] == s2[j - 1] ? 0 : 1) });

    return d[len1][len2];
  }
};

bool simJoin::SimilarityJoin(
                             unsigned int threshold,
                             vector<triple<unsigned int, unsigned int, unsigned int>> &results) {
  return true;
}
