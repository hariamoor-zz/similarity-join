#include "simJoin.h"
#include <algorithm>
#include <fstream>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>

template <typename T>
void alloc2d(std::vector<std::vector<T>> &p, int m, int n) {
  p.reserve(m);
  for (int k = 0; k < m; k++) {
    p[k].reserve(n);
  }
}

template <typename T>
typename std::enable_if<std::is_arithmetic<T>::value, T>::type min3(T a, T b,
                                                                    T c) {
  return std::min(a, std::min(b, c));
}

template <typename T>
typename std::enable_if<std::is_arithmetic<T>::value, T>::type max3(T a, T b,
                                                                    T c) {
  return std::max(a, std::max(b, c));
}

inline int DJB_hash(const char *str, int len) {
  unsigned hash = 5381;

  for (int k = 0; k < len; k++) {
    hash += (hash << 5) + str[k];
  }

  return (hash & 0x7FFFFFFF);
}

inline int edit_distance(const std::string &s1, const std::string &s2,
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
            min3(matrix[i - 1][j - i + THRESHOLD],
                 j - 1 >= i - left ? matrix[i][j - i + THRESHOLD - 1]
                                   : THRESHOLD,
                 j + 1 <= i + right ? matrix[i - 1][j - i + THRESHOLD + 1]
                                    : THRESHOLD) +
            1;
      if (abs(xlen - ylen - i + j) + matrix[i][j - i + THRESHOLD] <= THRESHOLD)
        valid = true;
    }
    if (!valid)
      return THRESHOLD + 1;
  }
  return matrix[xlen][ylen - xlen + THRESHOLD];
}

template <typename T, typename H = std::hash<T>>
using HashSet = std::unordered_set<T, H>;

template <typename K, typename V, typename H = std::hash<K>>
using HashMap = std::unordered_map<K, V, H>;

struct PIndex {
  int stPos;   // start position of substring
  int Lo;      // start position of segment
  int partLen; // substring/segment length
  int len;     // length of indexed string
  PIndex(int _s, int _o, int _p, int _l)
      : stPos(_s), Lo(_o), partLen(_p), len(_l) {}
};

int D, N, PN;
vector<string> dict;
int maxDictLength = 0;
int minDictLength = 0x7FFFFFFF;

std::vector<std::vector<std::vector<PIndex>>> partIndex;
// HashMap<int, vector<int>> **invLists;
// int **partPos;
// int **partLen;
// int *dist;
std::vector<std::vector<HashMap<int, std::vector<int>>>> invLists;
std::vector<std::vector<int>> partPos, partLen;
std::vector<int> dist;

long long candNum = 0;
long long realNum = 0;

template <typename T> void alloc1d(std::vector<T> &p, int n) { p.reserve(n); }

void init() {
  // alloc2d(partLen, PN, maxDictLength + 1);
  partLen =
      std::vector<std::vector<int>>(PN, std::vector<int>(maxDictLength + 1));
  // alloc2d(partPos, PN + 1, maxDictLength + 1);
  partPos = std::vector<std::vector<int>>(PN + 1,
                                          std::vector<int>(maxDictLength + 1));

  // alloc2d(partIndex, PN, maxDictLength + 1);
  partIndex = std::vector<std::vector<std::vector<PIndex>>>(
      PN, std::vector<std::vector<PIndex>>(maxDictLength + 1));

  // alloc2d(invLists, PN, maxDictLength + 1);
  invLists = std::vector<std::vector<HashMap<int, std::vector<int>>>>(
      PN, std::vector<HashMap<int, std::vector<int>>>(maxDictLength + 1));
  // alloc1d(dist, maxDictLength + 2);
  dist = std::vector<int>(maxDictLength + 2);

  for (int lp = 0; lp <= maxDictLength + 1; lp++) {
    dist[lp] = N;
  }

  for (int len = minDictLength; len <= maxDictLength; len++) {
    partPos[0][len] = 0;
    partLen[0][len] = len / PN;
    partPos[PN][len] = len;
  }

  for (int pid = 1; pid < PN; pid++) {
    for (int len = minDictLength; len <= maxDictLength; len++) {
      partPos[pid][len] = partPos[pid - 1][len] + partLen[pid - 1][len];
      if (pid == (PN - len % PN)) {
        partLen[pid][len] = partLen[pid - 1][len] + 1;
      } else {
        partLen[pid][len] = partLen[pid - 1][len];
      }
    }
  }
}

void prepare() {
  int clen = 0;
  for (int id = 0; id < N; id++) {
    if (clen == (int)dict[id].length())
      continue;
    for (int lp = clen + 1; lp <= (int)dict[id].length(); lp++)
      dist[lp] = id;
    clen = dict[id].length();
  }

  clen = 0;
  for (int id = 0; id < N; id++) {
    if (clen == (int)dict[id].length())
      continue;
    clen = dict[id].length();

    for (int pid = 0; pid < PN; pid++) {
      for (int len = max(clen - D, minDictLength); len <= clen; len++) {
        if (dist[len] == dist[len + 1])
          continue;

        for (int stPos = max3(0, partPos[pid][len] - pid,
                              partPos[pid][len] + (clen - len) - (D - pid));
             stPos <= min3(clen - partLen[pid][len], partPos[pid][len] + pid,
                           partPos[pid][len] + (clen - len) + (D - pid));
             stPos++) {
          partIndex[pid][clen].emplace_back(stPos, partPos[pid][len],
                                            partLen[pid][len], len);
        }
      }
    }
  }
}

void perform_join(vector<triple<unsigned, unsigned, unsigned>> &results) {
  for (int id = 0; id < N; id++) {
    HashSet<int> checked_ids;
    int clen = dict[id].length();
    for (int partId = 0; partId < PN; partId++) {
      for (int lp = 0; lp < (int)partIndex[partId][clen].size(); lp++) {
        int stPos = partIndex[partId][clen][lp].stPos;
        int Lo = partIndex[partId][clen][lp].Lo;
        int pLen = partIndex[partId][clen][lp].partLen;
        int len = partIndex[partId][clen][lp].len;

        int hash_value = DJB_hash(dict[id].c_str() + stPos, pLen);
        if (invLists[partId][len].count(hash_value) == 0)
          continue;
        for (int cand : invLists[partId][len][hash_value]) {
          if (checked_ids.find(cand) == checked_ids.end()) {
            ++candNum;
            if (partId == D)
              checked_ids.insert(cand);
            if (partId == 0 || edit_distance(dict[cand], dict[id], partId, 0, 0,
                                             Lo, stPos) <= partId) {
              if (partId == 0)
                checked_ids.insert(cand);
              if (partId == D ||
                  edit_distance(dict[cand], dict[id], D - partId, Lo + pLen,
                                stPos + pLen) <= D - partId) {
                auto ed = edit_distance(dict[cand], dict[id], D);

                if (ed <= D) {
                  checked_ids.insert(cand);
                  realNum++;
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
      int pLen = partLen[partId][clen];
      int stPos = partPos[partId][clen];
      invLists[partId][clen][DJB_hash(dict[id].c_str() + stPos, pLen)]
          .push_back(id);
    }
  }
}

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

bool simJoin::SimilarityJoin(
    unsigned int threshold,
    vector<triple<unsigned int, unsigned int, unsigned int>> &results) {
  // cout << "Constructing" << endl;
  // auto impl = PassJoin(threshold, this, results);

  // cout << "Starting prepare" << endl;
  // if (!impl.prepare()) {
  //   return false;
  // }

  // cout << "Starting to join" << endl;
  // if (!impl.join()) {
  //   return false;
  // }

  // cout << "Returning" << endl;
  D = threshold;
  N = this->getDataNum();
  PN = D + 1;

  for (int i = 0; i < N; i++) {
    string out;
    this->getString(i, out);

    auto l = out.length();
    if (l > maxDictLength) {
      maxDictLength = l;
    }

    if (l < minDictLength) {
      minDictLength = l;
    }

    dict.push_back(out);
  }

  sort(dict.begin(), dict.end(), [](const string &r, const string &s) {
    return r.length() < s.length();
  });

  cout << "Perform init" << endl;
  init();

  cout << "Perform prepare" << endl;
  prepare();

  cout << "Perform join" << endl;
  perform_join(results);

  cout << "Number of elements in results: " << results.size() << endl;

  cout << "Returning" << endl;
  return true;
}
