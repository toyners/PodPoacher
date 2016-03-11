#include "stdafx.h"
#include "Utility.h"
#include "HTTPFileDownload.h"
#include "RSSContentHandler.h"
#include "XMLFileParser.h"
#include <fstream>
#include <iostream>
#include <algorithm>
#include <cctype>

void downloadRSSFile(std::string url, std::string rssFilePath)
{
  std::cout << "Getting RSS file...";
  std::string scheme = getSchemeFromURL(url);
  if (scheme == "http")
  {
    HTTPFileDownload::downloadTextFile(url, rssFilePath);
  }
  else if (scheme == "https")
  {
    HTTPFileDownload::downloadSecureTextFile(url, rssFilePath);
  }
  else
  {
    std::string message = "Scheme '" + scheme + "' not handled.";
    throw std::exception(message.data());
  }

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

bool tryGetFileNameFromURL(std::string& url, std::string& fileName)
{
  int lastBackslashPos = url.find_last_of('/');
  if (lastBackslashPos == std::string::npos)
  {
    return false;
  }

  fileName = url.substr(lastBackslashPos + 1);
  return true;
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

std::string getSchemeFromURL(std::string& url)
{
  size_t colonPos = url.find_first_of(':');
  if (colonPos == std::string::npos)
  { 
    std::string message = "No colon found in URL '" + url + "'.";
    throw std::exception(message.data());
  }

  std::string scheme = url.substr(0, colonPos);
  std::transform(scheme.begin(), scheme.end(), scheme.begin(), std::tolower);

  return scheme;
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

std::string getReadableFileSize(long size)
{
  char buffer[25];
  float sizeInKB = (float)size / 1024;
  if (sizeInKB < 0)
  {
    return std::to_string(size) + " b";
  }

  float sizeInMB = sizeInKB / 1024;
  if (sizeInMB < 0)
  {
    sprintf(buffer, "%.1f", sizeInKB);
    return std::string(buffer) + " KB";
  }

  sprintf(buffer, "%.1f", sizeInMB);
  return std::string(buffer) + " MB";
}
