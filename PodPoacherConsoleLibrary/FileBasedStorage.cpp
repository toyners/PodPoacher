#include "stdafx.h"
#include "FilebasedStorage.h"
#include <fstream>
#include <sstream>
#include <vector>
#include <cstdio>

FileBasedStorage::FileBasedStorage(std::string path)
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
  std::ofstream outputFile;
  outputFile.open(channelPath, std::ios::app);

  outputFile << channel.getFeedURL() << "|"
    << channel.getTitle() << "|"
    << channel.getDescription() << "|"
    << channel.getDirectory() << "|"
    << channel.getWebsite() << "|"
    << channel.getPublishedDate() << "|"
    << std::endl;

  outputFile.close();

  std::string fileName = storagePath + std::to_string(channelList.size() + 1) + ".txt";

  if (channel.getPodcastCount() > 0)
  {
    serialisePodcasts(fileName, channel);
  }

  channelList.push_back(&channel);
}

std::vector<PodcastChannel*>& FileBasedStorage::getChannels()
{
  return channelList;
}

void FileBasedStorage::loadChannel(PodcastChannel& channel)
{
  // Do nothing.
}

void FileBasedStorage::updateChannel(PodcastChannel& channel)
{
  std::string channelFileName = getChannelFileName(channel);
  
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

      serialisePodcasts(storagePath + std::to_string(i + 1) + ".txt", newChannel);
      serialiseChannelsOnly();
      return;
    }
  }

  throw std::range_error("Old channel not found when updating channel.");
}

std::string FileBasedStorage::getChannelFileName(PodcastChannel& channel)
{
  for (int i = 0; i < channelList.size(); i++)
  {
    if (channelList[i] == &channel)
    {
      return std::to_string(i + 1) + ".txt";
    }
  }

  std::string message = "Channel '" + channel.getTitle() + "' not found when getting file name.";
  throw std::range_error(message.data());
}

void FileBasedStorage::getTokensFromLine(const std::string& line, std::vector<std::string>& tokens)
{
  std::stringstream ss(line);
  std::string item;
  tokens.clear();
  while (std::getline(ss, item, '|'))
  {
    tokens.push_back(item);
  }
}

void FileBasedStorage::deserialiseChannelsComplete()
{
  std::ifstream file;
  file.open(channelPath);

  if (!file.good())
  {
    // File does not exist so nothing to load.
    return;
  }

  std::vector<std::string> tokens;
  while (!file.eof())
  {
    std::string line;
    std::getline(file, line);

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

    std::string fileName = storagePath + std::to_string(channelList.size() + 1) + ".txt";
    deserialisePodcasts(fileName, *channel);

    channelList.push_back(channel);
  }

  file.close();
}

void FileBasedStorage::serialiseChannelsOnly()
{
  std::string backupPath = createBackup(channelPath);

  std::ofstream outputFile;
  outputFile.open(channelPath, std::ios::trunc);

  for (int index = 0; index < channelList.size(); index++)
  {
    PodcastChannel& channel = *channelList[index];

    outputFile << channel.getFeedURL() << "|"
      << channel.getTitle() << "|"
      << channel.getDescription() << "|"
      << channel.getDirectory() << "|"
      << channel.getWebsite() << "|"
      << channel.getPublishedDate() << "|"
      << std::endl;
  }

  outputFile.close();

  remove(backupPath.data());
}

void FileBasedStorage::serialisePodcasts(const std::string& filePath, PodcastChannel& channel)
{
  std::string backupPath = createBackup(filePath);

  std::ofstream outputFile;
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
      << std::endl;
  }

  outputFile.close();

  remove(backupPath.data());
}

void FileBasedStorage::deserialisePodcasts(const std::string& fileName, PodcastChannel& channel)
{
  std::ifstream inputFile;
  inputFile.open(fileName);

  std::vector<std::string> tokens;
  while (!inputFile.eof())
  {
    std::string line;
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

std::string FileBasedStorage::createBackup(const std::string& filePath)
{
  std::string backupPath = filePath + "_bk";

  const char* backupPathPtr = backupPath.data();
  const char* filePathPtr = filePath.data();

  int result = rename(filePathPtr, backupPathPtr);

  return backupPath;
}
