#include "simJoin.h"

int simJoin::getDataNum() const
{
  return data.size();
}

bool simJoin::getString(int id, string &out) const
{
  if (id < 0 || id >= data.size())
    return false;
  out = data[id];
  return true;
}

bool simJoin::readData(const string &filename)
{
  string str;
  ifstream datafile(filename, ios::in);
  while (getline(datafile, str))
    data.emplace_back(str);

  return true;
}

 /*
 * It should do a similarity join operation betweent the set of strings from a data file
 * such that the edit distance between two string is not larger than the given threshold. The 
 * format of result is a triple of numbers which respectively stand for the two IDs of the pair of strings
 * from the data file and the edit distance between the two strings. All results are stored in a vector, sorted based on the IDs of the string from
 * the first file and then the IDs of the string from the second file in an ascending order. Return 
 * an error if the similarity join operation is failed.
 */


bool simJoin::SimilarityJoin(unsigned threshold, vector< triple<unsigned, unsigned, unsigned> > &results) 
{
	return true;
}
