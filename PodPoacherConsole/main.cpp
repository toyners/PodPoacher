// main.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "Controller.h"
#include "Utility.h"
#include <string>

int main(int argc, char* argv[])
{
  std::string workingPath = getWorkingDirectory();

  Controller controller(workingPath);

  controller.execute();

  return 0;
}

