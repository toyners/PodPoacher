#include "stdafx.h"
#include "FilebasedStorage.h"
#include <fstream>
#include <sstream>
#include <vector>
#include <cstdio>

using namespace std;

string storagePath;
string channelPath;
vector<PodcastChannel*> channelList;

FileBasedStorage::FileBasedStorage(string path)
{
  if (path.size() > 0 && path[path.size() - 1] != '\\')
  {
    path += '\\';
  }

  storagePath = path;
  channelPath = path + "channels.txt";

  loadChannelHeaders();
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
  outputFile.open(channelPath, ios::app);

  outputFile << channel.getFeedURL() << "|" 
    << channel.getTitle() << "|"
    << channel.getDescription() << "|"
    << channel.getWebsite() << "|"
    << channel.getPublishedCount() << "|"
    << endl;

  outputFile.close();

  channelList.push_back(&channel);

  if (channel.getPodcastCount() > 0)
  {
    // create and load channel file. 
  }
}

vector<PodcastChannel*>& FileBasedStorage::getChannels()
{
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
  const char* outputName = getChannelFileName(channel).data();
  string newName(outputName);
  newName = storagePath + newName + "_old";

  const char* backupName = newName.data();
  rename(outputName, backupName);

  ofstream file;
  file.open(outputName, ios::trunc);

  for (int i = 0; i < channel.getPodcastCount(); i++)
  {
    PodcastDetails* podcastDetails = channel.getPodcast(0);
    file << podcastDetails->getTitle() << "|"
      << podcastDetails->getDescription() << "|"
      << podcastDetails->getPublishedDate() << "|"
      << podcastDetails->getURL() << "|"
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
  while (getline(ss, item, '|'))
  {
    tokens.push_back(item);
  }
}

void FileBasedStorage::loadChannelHeaders()
{
  ifstream file;
  file.open(channelPath);

  if (!file.good())
  {
    // File does not exist so nothing to load.
    return;
  }

  vector<string> tokens;
  while (!file.eof())
  {
    string line;
    getline(file, line);

    if (line.size() == 0)
    {
      continue;
    }

    getTokensFromLine(line, tokens);

    PodcastChannel* podcastChannel = new PodcastChannel
      (
      tokens[0],
      tokens[1],
      tokens[2],
      tokens[3],
      tokens[4]
      );

    channelList.push_back(podcastChannel);
  }

  file.close();
}