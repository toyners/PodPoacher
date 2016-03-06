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

  deserialiseChannelsComplete();
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
    << channel.getDirectory() << "|"
    << channel.getWebsite() << "|"
    << channel.getPublishedDate() << "|"
    << endl;

  outputFile.close();

  string fileName = storagePath + to_string(channelList.size() + 1) + ".txt";

  if (channel.getPodcastCount() > 0)
  {
    serialisePodcasts(fileName, channel);
  }

  channelList.push_back(&channel);
}

vector<PodcastChannel*>& FileBasedStorage::getChannels()
{
  return channelList;
}

void FileBasedStorage::loadChannel(PodcastChannel& channel)
{
  // Do nothing.
}

void FileBasedStorage::updateChannel(PodcastChannel& channel)
{
  string channelFileName = getChannelFileName(channel);
  
  serialisePodcasts(channelFileName, channel);
}

void FileBasedStorage::updateChannel(PodcastChannel& oldChannel, PodcastChannel& newChannel)
{
  for (int i = 0; i < channelList.size(); i++)
  { 
    if (channelList[i] == &oldChannel)
    {
      channelList[i] = &newChannel;
      delete &oldChannel;

      serialisePodcasts(storagePath + to_string(i + 1) + ".txt", newChannel);
      serialiseChannelsOnly();
      return;
    }
  }

  throw range_error("Old channel not found when updating channel.");
}

string FileBasedStorage::getChannelFileName(PodcastChannel& channel)
{
  for (int i = 0; i < channelList.size(); i++)
  {
    if (channelList[i] == &channel)
    {
      return to_string(i + 1) + ".txt";
    }
  }

  string message = "Channel '" + channel.getTitle() + "' not found when getting file name.";
  throw range_error(message.data());
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

void FileBasedStorage::deserialiseChannelsComplete()
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

    if (line.empty())
    {
      continue;
    }

    getTokensFromLine(line, tokens);

    PodcastChannel* channel = new PodcastChannel
      (
      tokens[0],
      tokens[1],
      tokens[2],
      tokens[3],
      tokens[4],
      tokens[5]
      );

    string fileName = storagePath + to_string(channelList.size() + 1) + ".txt";
    deserialisePodcasts(fileName, *channel);

    channelList.push_back(channel);
  }

  file.close();
}

void FileBasedStorage::serialiseChannelsOnly()
{
  string backupPath = createBackup(channelPath);

  ofstream outputFile;
  outputFile.open(channelPath, ios::trunc);

  for (int index = 0; index < channelList.size(); index++)
  {
    PodcastChannel& channel = *channelList[index];

    outputFile << channel.getFeedURL() << "|"
      << channel.getTitle() << "|"
      << channel.getDescription() << "|"
      << channel.getDirectory() << "|"
      << channel.getWebsite() << "|"
      << channel.getPublishedDate() << "|"
      << endl;
  }

  outputFile.close();

  remove(backupPath.data());
}

void FileBasedStorage::serialisePodcasts(const string& filePath, PodcastChannel& channel)
{
  string backupPath = createBackup(filePath);

  ofstream outputFile;
  outputFile.open(filePath);

  for (int i = 0; i < channel.getPodcastCount(); i++)
  {
    PodcastDetails& podcast = *channel.getPodcast(i);

    outputFile << podcast.getTitle() << "|"
      << podcast.getDescription() << "|"
      << podcast.getURL() << "|"
      << podcast.getPublishedDate() << "|"
      << podcast.getFileSize() << "|"
      << podcast.getDownloadDate() << "|"
      << endl;
  }

  outputFile.close();

  remove(backupPath.data());
}

void FileBasedStorage::deserialisePodcasts(const string& fileName, PodcastChannel& channel)
{
  ifstream inputFile;
  inputFile.open(fileName);

  vector<string> tokens;
  while (!inputFile.eof())
  {
    string line;
    getline(inputFile, line);

    if (line.empty())
    {
      continue;
    }

    getTokensFromLine(line, tokens);

    channel.addPodcastDetails(
      tokens[0],
      tokens[1],
      tokens[2],
      tokens[3],
      stol(tokens[4]),
      tokens[5]);
  }
}

string FileBasedStorage::createBackup(const string& filePath)
{
  string backupPath = filePath + "_bk";

  const char* backupPathPtr = backupPath.data();
  const char* filePathPtr = filePath.data();

  int result = rename(filePathPtr, backupPathPtr);

  return backupPath;
}
