#include "stdafx.h"
#include "Utility.h"
#include "HTTPFileDownload.h"
#include "RSSContentHandler.h"
#include "XMLFileParser.h"
#include <fstream>
#include <iostream>

void downloadRSSFile(std::string url, std::string rssFilePath)
{
  std::cout << "Getting RSS file...";
  HTTPFileDownload::downloadTextFile(url, rssFilePath);
  std::cout << "DONE" << std::endl;
}

void parseRSSFile(std::string url, std::string rssFilePath, PodcastChannel* channel)
{
  std::cout << "Parsing RSS file...";
  RSSContentHandler contentHandler(channel);
  XMLFileParser parser(&contentHandler);
  parser.ParseFile(rssFilePath);
  std::cout << "DONE" << std::endl;
}

void setupProgress(long fileSize)
{
  tickSize = fileSize / 20;
  tickCount = 0;
}

void fileProgress(long position)
{
  int ticks = position / tickSize;
  while (ticks > tickCount)
  {
    tickCount++;
    std::cout << ".";
  }
}

std::string getWorkingDirectory()
{
  char* pathPtr;
  _get_pgmptr(&pathPtr);
  std::string path(pathPtr);
  path = path.substr(0, path.find_last_of('\\') + 1);

  std::ifstream configFile;
  configFile.open(path + "config.txt");
  if (!configFile.good())
  {
    throw new std::ios::failure("Config file not opened.");
  }

  std::string line;
  std::getline(configFile, line);
  configFile.close();

  return line;
}

long getFileSize(std::string filePath)
{
  std::ifstream in(filePath, std::ifstream::ate | std::ifstream::binary);
  return in.tellg();
}

std::string removeIllegalFilePathCharactersFromText(std::string text)
{
  size_t position = -1;
  while ((position = text.find_first_of("\\/:*?\"<>|", position + 1)) != std::string::npos)
  {
    text[position] = '-';
  }

  return text;
}
