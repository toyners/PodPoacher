#include "stdafx.h"
#include "FilebasedStorage.h"
#include <fstream>
#include <sstream>
#include <vector>
#include <cstdio>

using namespace std;

string storagePath;
string channelPath;
int nextKey = -1;
vector<PodcastChannel*> channelList;
bool channelListIsLoaded;

FileBasedStorage::FileBasedStorage(string path)
{
  if (path[path.size() - 1] != '\\')
  {
    path += '\\';
  }

  storagePath = path;
  channelPath = path + "channels.txt";
}

FileBasedStorage::~FileBasedStorage()
{
  if (channelList.size() != 0)
  {
    channelList.clear();
  }
}

void FileBasedStorage::addChannel(PodcastChannel& channel)
{
  ofstream outputFile;
  string line = channel.getFeedURL();

  outputFile.open(storagePath, ios::app);
  outputFile.write(line.data(), line.size());
  outputFile.close();

  channelList.push_back(&channel);
  channelListIsLoaded = true;

  if (channel.getPodcastCount() > 0)
  {
    // create and load channel file. 
  }
}

vector<PodcastChannel*>& FileBasedStorage::getChannels()
{
  if (channelListIsLoaded)
  {
    return channelList;
  }

  ifstream inputFile;
  inputFile.open(storagePath);
  while (!inputFile.eof())
  {
    string line;
    getline(inputFile, line);
    
    PodcastChannel podcastChannel(line);
    channelList.push_back(&podcastChannel);
  }

  channelListIsLoaded = true;
  return channelList;
}

void FileBasedStorage::loadChannel(PodcastChannel& channel)
{
  ifstream file;
  file.open(getChannelFileName(channel).data());

  vector<string> tokens;
  while (!file.eof())
  {
    string line;
    getline(file, line);
    getTokensFromLine(line, tokens);

    channel.addPodcastDetails(
      tokens[0],
      tokens[1],
      tokens[2],
      tokens[3],
      stol(tokens[4]));
  }

  file.close();
}

void FileBasedStorage::updateChannel(PodcastChannel& channel)
{
  const char* outputpName = getChannelFileName(channel).data();
  string newName(outputpName);
  newName += "_old";

  const char* backupName = newName.data();
  rename(outputpName, backupName);

  ofstream file;
  file.open(outputpName, ios::trunc);

  for (int i = 0; i < channel.getPodcastCount(); i++)
  {
    PodcastDetails* podcastDetails = channel.getPodcast(0);
    file << podcastDetails->getTitle() << ","
      << podcastDetails->getDescription() << ","
      << podcastDetails->getPublishedDate() << ","
      << podcastDetails->getURL() << ","
      << to_string(podcastDetails->getFileSize()) << "\\n";
  }

  file.close();

  remove(backupName);
}

string FileBasedStorage::getChannelFileName(PodcastChannel& channel)
{
  int index = 1;
  for (int i = 0; i < channelList.size(); i++)
  {
    if (channelList[i] == &channel)
    {
      return to_string(i) + ".txt";
    }
  }

  throw new exception();
}

void FileBasedStorage::getTokensFromLine(const string& line, vector<string>& tokens)
{
  stringstream ss(line);
  string item;
  tokens.clear();
  while (getline(ss, item, ','))
  {
    tokens.push_back(item);
  }
}
