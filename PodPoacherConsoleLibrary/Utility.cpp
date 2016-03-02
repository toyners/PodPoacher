#include "stdafx.h"
#include "Utility.h"
#include "HTTPFileDownload.h"
#include "RSSContentHandler.h"
#include "XMLFileParser.h"
#include <fstream>

using namespace std;

void downloadRSSFile(string url, string rssFilePath)
{
  cout << "Getting RSS file...";
  HTTPFileDownload::downloadTextFile(url, rssFilePath);
  cout << "DONE" << endl;
}

void parseRSSFile(string url, string rssFilePath, PodcastChannel* channel)
{
  cout << "Parsing RSS file...";
  RSSContentHandler contentHandler(channel);
  XMLFileParser parser(&contentHandler);
  parser.ParseFile(rssFilePath);
  cout << "DONE" << endl;
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
    cout << ".";
  }
}

string getWorkingDirectory()
{
  char* pathPtr;
  _get_pgmptr(&pathPtr);
  string path(pathPtr);
  path = path.substr(0, path.find_last_of('\\') + 1);

  ifstream configFile;
  configFile.open(path + "config.txt");
  if (!configFile.good())
  {
    throw new ios::failure("Config file not opened.");
  }

  string line;
  getline(configFile, line);
  configFile.close();

  return line;
}

long getFileSize(string filePath)
{
  ifstream in(filePath, std::ifstream::ate | std::ifstream::binary);
  return in.tellg();
}