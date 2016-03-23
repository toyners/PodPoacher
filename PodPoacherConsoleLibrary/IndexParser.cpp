#include "stdafx.h"

#include <string>
#include <algorithm>
#include "IndexParser.h"

IndexParser::IndexParser() {}

IndexParser::~IndexParser() {}

std::vector<int>* IndexParser::parseInput(std::string& url)
{
  std::vector<int>* list = new std::vector<int>();
  std::string firstValue;
  std::string secondValue;
  bool inRange = false;

  for (int index = 0; index < url.size(); index++)
  {
    char c = url[index];
    if (isdigit(c))
    {
      firstValue += c;
    }
    else if (c == '-')
    {
      if (inRange)
      {
        throw std::exception("'-' is illegal - already in range.");
      }

      if (firstValue.size() == 0)
      {
        throw std::exception("No first value found in range.");
      }

      inRange = true;
      secondValue = firstValue;
      firstValue.clear();
    }
    else if (c == ',')
    {
      if (inRange)
      {

        addRange(firstValue, secondValue, *list);
        inRange = false;
        firstValue.clear();
        secondValue.clear();
        continue;
      }

      if (firstValue.size() > 0)
      {
        list->push_back(stoi(firstValue));
        firstValue.clear();
      }
    }
  }

  if (inRange)
  {
    addRange(firstValue, secondValue, *list);
  }
  else if (firstValue.size() > 0)
  {
    list->push_back(stoi(firstValue));
  }

  return list;
}

void IndexParser::addRange(std::string firstValue, std::string secondValue, std::vector<int>& list)
{
  // Swap the two values.
  int first = stoi(secondValue);
  int second = stoi(firstValue);

  int stepDirection = 1;
  if (first > second)
  {
    while (first >= second)
    {
      list.push_back(first--);
    }
  }
  else
  {
    while (first <= second)
    {
      list.push_back(first++);
    }
  }
}
