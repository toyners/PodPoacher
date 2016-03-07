// PodPoacherTests.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "gtest/gtest.h"

int main(int argc, char **argv)
{
  ::testing::InitGoogleTest(&argc, argv);
  int result = RUN_ALL_TESTS();

  std::cout << std::endl << "Result is " << result << "." << std::endl << "Press any key to Exit.";
  std::cin.ignore();

  return result;
}

