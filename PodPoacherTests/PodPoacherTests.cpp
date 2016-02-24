// PodPoacherTests.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "gtest/gtest.h"

using namespace std;

int main(int argc, char **argv)
{
  ::testing::InitGoogleTest(&argc, argv);
  int result = RUN_ALL_TESTS();

  cout << endl << "Result is " << result << "." << endl << "Press any key to Exit.";
  cin.ignore();

  return result;
}

