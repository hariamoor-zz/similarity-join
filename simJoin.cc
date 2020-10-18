#include "simJoin.h"
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

int min(int x, int y, int z) 
{ 
    return min(min(x, y), z); 
} 
  
int distance(string str1, string str2, int m, int n) 
{ 
    // If first string is empty, the only option is to 
    // insert all characters of second string into first 
    if (m == 0) 
        return n; 
  
    // If second string is empty, the only option is to 
    // remove all characters of first string 
    if (n == 0) 
        return m; 
  
    // If last characters of two strings are same, nothing 
    // much to do. Ignore last characters and get count for 
    // remaining strings. 
    if (str1[m - 1] == str2[n - 1]) 
        return distance(str1, str2, m - 1, n - 1); 
  
    // If last characters are not same, consider all three 
    // operations on last character of first string, recursively 
    // compute minimum cost for all three operations and take 
    // minimum of three values. 
    return 1 + min(distance(str1, str2, m, n - 1), // Insert 
                   distance(str1, str2, m - 1, n), // Remove 
                   distance(str1, str2, m - 1, n - 1) // Replace 
                   ); 
}

void test_distance() {
  
}

bool simJoin::SimilarityJoin(
    unsigned threshold, vector<triple<unsigned, unsigned, unsigned>> &results) {
  for (unsigned i = 0; i < getDataNum(); i++) {
    for (unsigned j = i + 1; j < getDataNum(); j++) {
      string a, b;
      // cout << "(a, b) = " << i << ", " << j << endl;
      this->getString(i, a);
      this->getString(j, b);

      unsigned d = distance(a, b, a.length(), b.length());

      if (d < threshold) {
        triple<unsigned, unsigned, unsigned> t = {.id1 = i, .id2 = j, .ed = d};
        results.push_back(t);
      }
    }
  }
  return true;
}
