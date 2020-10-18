#include <unordered_map>
#include "simJoin.h"

bool simJoinTest() {
  typedef struct {
    std::tuple<std::string, std::string> values;
    int expected;
  } TestCase;

  const TestCase cases[] = {
    {
      std::make_tuple("kitten", "sitting"),
      3,
    },
    {
      std::make_tuple("sitting", "kitten"), // Check symmetry with cases 1 + 2
      3,
    },
    {
      std::make_tuple("something", "something"), // d(str, str) = 0
      0,
    },
    {
      std::make_tuple("horse", "ros"),
      3,
    },
    {
      std::make_tuple("intention", "execution"),
      5,
    },
  };

  auto ret_val = true;
  std::cout << "Starting cases in simJoinTest" << std::endl << std::endl;
  
  for (auto testCase : cases) {
    std::string first = std::get<0>(testCase.values);
    std::string second = std::get<1>(testCase.values);
    int received = distance(first, second, first.length(), second.length());

    if (received == testCase.expected) {
      std::cout << "Passed test case with (" <<
        first <<
        ", " <<
        second <<
        ") and expecting " <<
        received <<
        std::endl;
    } else {
      std::cout << "Error (simJoinTest): Received " <<
        received <<
        " while expecting " <<
        testCase.expected <<
        " with parameters (" <<
        first <<
        ", " <<
        second <<
        ")" <<
        std::endl;

      ret_val = false;
    }
  }

  if(ret_val) {
    std::cout << std::endl;
    std::cout << "Passed all test cases for simJoin!" << std::endl << std::endl;
  }
  return ret_val;
}

void unitTests() {
  simJoinTest();
}

void integrationTests() {
  std::cout << "Need to write integration tests" << std::endl;
}

int main(int argc, char **argv) {
  unitTests();
  integrationTests();
}
