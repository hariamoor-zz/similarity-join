#include "simJoin.h"
#include <algorithm>
#include <fstream>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>

class PassJoin {
  static inline int edit_distance(const std::string &s1, const std::string &s2,
                           int THRESHOLD, int xpos = 0, int ypos = 0,
                           int xlen = -1, int ylen = -1) {
    if (xlen == -1)
      xlen = s1.length() - xpos;
    if (ylen == -1)
      ylen = s2.length() - ypos;
    if (xlen > ylen + THRESHOLD || ylen > xlen + THRESHOLD)
      return THRESHOLD + 1;
    if (xlen == 0)
      return ylen;

    int matrix[xlen + 1][2 * THRESHOLD + 1];
    for (int k = 0; k <= THRESHOLD; k++)
      matrix[0][THRESHOLD + k] = k;

    int right = (THRESHOLD + (ylen - xlen)) / 2;
    int left = (THRESHOLD - (ylen - xlen)) / 2;
    for (int i = 1; i <= xlen; i++) {
      bool valid = false;
      if (i <= left) {
        matrix[i][THRESHOLD - i] = i;
        valid = true;
      }
      for (int j = (i - left >= 1 ? i - left : 1);
           j <= (i + right <= ylen ? i + right : ylen); j++) {
        if (s1[xpos + i - 1] == s2[ypos + j - 1])
          matrix[i][j - i + THRESHOLD] = matrix[i - 1][j - i + THRESHOLD];
        else
          matrix[i][j - i + THRESHOLD] =
              std::min({matrix[i - 1][j - i + THRESHOLD],
                        j - 1 >= i - left ? matrix[i][j - i + THRESHOLD - 1]
                                          : THRESHOLD,
                        j + 1 <= i + right
                            ? matrix[i - 1][j - i + THRESHOLD + 1]
                            : THRESHOLD}) +
              1;
        if (abs(xlen - ylen - i + j) + matrix[i][j - i + THRESHOLD] <=
            THRESHOLD)
          valid = true;
      }
      if (!valid)
        return THRESHOLD + 1;
    }
    return matrix[xlen][ylen - xlen + THRESHOLD];
  }

  struct PIndex {
    int stPos;   // start position of substring
    int Lo;      // start position of segment
    int partLen; // substring/segment length
    int len;     // length of indexed string
    PIndex(int _s, int _o, int _p, int _l)
        : stPos(_s), Lo(_o), partLen(_p), len(_l) {}
  };

  int D, N, PN;
  vector<string> dictionary;
  int maxDictLength = 0;
  int minDictLength = INT32_MAX;

  std::vector<std::vector<std::vector<PIndex>>> partIndex;
  std::vector<std::vector<std::unordered_map<int, std::vector<int>>>> invertedIndex;
  std::vector<std::vector<int>> partPosition, partLength;
  std::vector<int> distance;

public:
  PassJoin(unsigned threshold, simJoin* joiner) : D(threshold), N(joiner->getDataNum()), PN(threshold + 1) {
    for (int i = 0; i < N; i++) {
      string out;
      joiner->getString(i, out);

      auto l = out.length();
      if (l > maxDictLength) {
        maxDictLength = l;
      }

      if (l < minDictLength) {
        minDictLength = l;
      }

	  dictionary.insert(std::upper_bound(dictionary.begin(), dictionary.end(), out, [](const std::string& r, const std::string& s){ return r.length() < s.length(); }), out);
    }

    partLength =
        std::vector<std::vector<int>>(PN, std::vector<int>(maxDictLength + 1));
    partPosition = std::vector<std::vector<int>>(
        PN + 1, std::vector<int>(maxDictLength + 1));

    partIndex = std::vector<std::vector<std::vector<PIndex>>>(
        PN, std::vector<std::vector<PIndex>>(maxDictLength + 1));

    invertedIndex =
        std::vector<std::vector<std::unordered_map<int, std::vector<int>>>>(
            PN, std::vector<std::unordered_map<int, std::vector<int>>>(
                    maxDictLength + 1));
    distance = std::vector<int>(maxDictLength + 2);

    for (int lp = 0; lp <= maxDictLength + 1; lp++) {
      distance[lp] = N;
    }

    for (int len = minDictLength; len <= maxDictLength; len++) {
      partPosition[0][len] = 0;
      partLength[0][len] = len / PN;
      partPosition[PN][len] = len;
    }

    for (int pid = 1; pid < PN; pid++) {
      for (int len = minDictLength; len <= maxDictLength; len++) {
        partPosition[pid][len] = partPosition[pid - 1][len] + partLength[pid - 1][len];
        if (pid == (PN - len % PN)) {
          partLength[pid][len] = partLength[pid - 1][len] + 1;
        } else {
          partLength[pid][len] = partLength[pid - 1][len];
        }
      }
    }
  }

  bool prepare() {
    int clen = 0;
    for (int id = 0; id < N; id++) {
      if (clen == (int)dictionary[id].length())
        continue;
      for (int lp = clen + 1; lp <= (int)dictionary[id].length(); lp++)
        distance[lp] = id;
      clen = dictionary[id].length();
    }

    clen = 0;
    for (int id = 0; id < N; id++) {
      if (clen == (int)dictionary[id].length())
        continue;
      clen = dictionary[id].length();

      for (int pid = 0; pid < PN; pid++) {
        for (int len = max(clen - D, minDictLength); len <= clen; len++) {
          if (distance[len] == distance[len + 1])
            continue;

          for (int stPos =
                   std::max({0, partPosition[pid][len] - pid,
                             partPosition[pid][len] + (clen - len) - (D - pid)});
               stPos <=
               std::min({clen - partLength[pid][len], partPosition[pid][len] + pid,
                         partPosition[pid][len] + (clen - len) + (D - pid)});
               stPos++) {
            partIndex[pid][clen].emplace_back(stPos, partPosition[pid][len],
                                              partLength[pid][len], len);
          }
        }
      }
    }

	return true;
  }

  bool perform_join(vector<triple<unsigned, unsigned, unsigned>> &results) {
    for (int id = 0; id < N; id++) {
      std::unordered_set<int> checked_ids;
      int clen = dictionary[id].length();
      for (int partId = 0; partId < PN; partId++) {
        for (int lp = 0; lp < (int)partIndex[partId][clen].size(); lp++) {
          const int stPos = partIndex[partId][clen][lp].stPos;
          const int Lo = partIndex[partId][clen][lp].Lo;
          const int pLen = partIndex[partId][clen][lp].partLen;
          const int len = partIndex[partId][clen][lp].len;

          int hash_value = std::hash<std::string>{}(dictionary[id].substr(stPos, pLen));
          if (invertedIndex[partId][len].count(hash_value) == 0)
            continue;
          for (int cand : invertedIndex[partId][len][hash_value]) {
            if (checked_ids.find(cand) == checked_ids.end()) {
              if (partId == D)
                checked_ids.insert(cand);
              if (partId == 0 || PassJoin::edit_distance(dictionary[cand], dictionary[id], partId, 0,
                                               0, Lo, stPos) <= partId) {
                if (partId == 0)
                  checked_ids.insert(cand);
                if (partId == D ||
                    PassJoin::edit_distance(dictionary[cand], dictionary[id], D - partId, Lo + pLen,
                                  stPos + pLen) <= D - partId) {
                  auto ed = PassJoin::edit_distance(dictionary[cand], dictionary[id], D);

                  if (ed <= D) {
                    checked_ids.insert(cand);
                    results.push_back({.id1 = (unsigned)id,
                                       .id2 = (unsigned)cand,
                                       .ed = (unsigned)ed});
                  }
                }
              }
            }
          }
        }
      }

      for (int partId = 0; partId < PN; partId++) {
        int pLen = partLength[partId][clen];
        int stPos = partPosition[partId][clen];
        invertedIndex[partId][clen][std::hash<std::string>{}(dictionary[id].substr(stPos, pLen))]
            .push_back(id);
      }
    }

	return true;
  }
};

int simJoin::getDataNum() const {
  return data.size();
}

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

bool simJoin::SimilarityJoin(
    unsigned int threshold,
    vector<triple<unsigned int, unsigned int, unsigned int>> &results) {
  auto passJoin = PassJoin(threshold, this);
  if(!passJoin.prepare())
	return false;
  if(!passJoin.perform_join(results))
	return false;

  return true;
}
