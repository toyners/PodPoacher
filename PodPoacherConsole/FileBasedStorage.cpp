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

  deserialiseChannels();
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
  for (int i = 0; i < channelList.size(); i++)
  {
    if (channelList[i] == &channel)
    {
      return to_string(i + 1) + ".txt";
    }
  }

  throw new exception(); // TODO
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

void FileBasedStorage::deserialiseChannels()
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
      tokens[4]
      );

    string fileName = storagePath + to_string(channelList.size() + 1) + ".txt";
    deserialisePodcasts(fileName, *channel);

    channelList.push_back(channel);
  }

  file.close();
}

void FileBasedStorage::serialisePodcasts(const string& fileName, PodcastChannel& channel)
{
  ofstream outputFile;
  outputFile.open(fileName);

  for (int i = 0; i < channel.getPodcastCount(); i++)
  {
    PodcastDetails& podcast = *channel.getPodcast(i);

    outputFile << podcast.getTitle() << "|"
      << podcast.getDescription() << "|"
      << podcast.getURL() << "|"
      << podcast.getPublishedDate() << "|"
      << podcast.getFileSize() << "|"
      << podcast.isIgnored() << "|"
      << podcast.getDownloadDate() << "|"
      << endl;
  }

  outputFile.close();
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
      tokens[5] == "true",
      tokens[6]);
  }
}