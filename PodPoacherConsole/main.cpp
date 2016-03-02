// main.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "Controller.h"
#include "Utility.h"
#include <string>

using namespace std;

int main(int argc, char* argv[])
{
  string workingPath = getWorkingDirectory();

  Controller controller(workingPath);

  controller.execute();

  return 0;
}

