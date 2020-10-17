
#include "simJoin.h"

void formating(vector< triple<unsigned, unsigned, unsigned> > &results)
{
  sort(results.begin(), results.end(), [](const triple<unsigned, unsigned, unsigned> &t1, const triple<unsigned, unsigned, unsigned> &t2) {

    if (t1.id1 != t2.id1)
      return t1.id1 < t2.id1;

    if (t1.id2 != t2.id2)
      return t1.id2 < t2.id2;

    return t1.ed < t2.ed;
  });
}

void logTimeMessage(timeval &t1, timeval &t2, string message)
{
  cout << message << t2.tv_sec - t1.tv_sec + (t2.tv_usec - t1.tv_usec) / 1e6 << endl;
}

int main (int argc, char ** argv)
{
  timeval t1, t2, t3;
  vector< triple<unsigned, unsigned, unsigned > > results;


  cout << "Reading data begin " << endl;

  string filename = "data.txt";

  gettimeofday(&t1, NULL);
  simJoin joiner(filename);
  gettimeofday(&t2, NULL);

  cout << "Reading data done. " << joiner.getDataNum() << " strings in total." << endl;

  string message1 = "IO Time: ";
  logTimeMessage(t1, t2, message1);

  unsigned edtau = 3;
  joiner.SimilarityJoin(edtau, results);
  gettimeofday(&t3, NULL);

  string message2 = "Join Time: ";
  logTimeMessage(t2, t3, message2);
  cout << "Number of results: " << results.size() << endl;

  formating(results);

  for (unsigned i = 0; i < results.size(); i++) 
  {
    unsigned firstId = results[i].id1;
    string firstStr;
    joiner.getString(firstId, firstStr);

    unsigned secondId = results[i].id2;
    string secondStr;
    joiner.getString(secondId, secondStr);

    unsigned ed = results[i].ed;

    cerr << firstId << " " << secondId << " " << ed << endl;
    cerr << "# " << firstStr << endl;
    cerr << "# " << secondStr << endl;
  }

  return 0;
}
