#pragma once

#include <string>
#include <vector>

class IndexParser
{
private:
  std::string firstValue;
  std::string secondValue;

public:
  IndexParser();
  ~IndexParser();

  std::vector<int>* parseInput(std::string& url);

private:
  void addRange(std::string firstValue, std::string secondValue, std::vector<int>& list);
};

