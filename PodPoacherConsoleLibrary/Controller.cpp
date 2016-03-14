#include "stdafx.h"
#include "Controller.h"
#include "HTTPFileDownload.h"
#include "RSSContentHandler.h"
#include "XMLFileParser.h"
#include "FileBasedStorage.h"
#include "Utility.h"
#include <vector>
#include <ctime>
#include <iostream>

Controller::Controller(const std::string& path)
{
  workingPath = path;
  storage = new FileBasedStorage(workingPath);
  ui = new UI(*this);
}

Controller::~Controller()
{
  delete storage;
  delete ui;
}

void Controller::addChannel(std::string url, std::string directory)
{
  verifyChannelIsNotInList(url);

  PodcastChannel* channel = createChannelFromFeed(url, directory);

  storage->addChannel(*channel);
}

std::string Controller::getDate()
{
  time_t t = time(0);   // get time now
  struct tm * now = localtime(&t);
  return std::to_string(now->tm_mday) + "-" + std::to_string(now->tm_mon) + "-" + std::to_string(now->tm_year + 1900);
}

std::string Controller::getTime()
{
  time_t t = time(0);   // get time now
  struct tm * now = localtime(&t);
  return std::to_string(now->tm_hour) + "-" + std::to_string(now->tm_min) + "-" + std::to_string(now->tm_sec);
}

long Controller::downloadPodcastFile(std::string url, std::string filePath, long fileSize)
{
  if (fileSize == 0)
  {
    HTTPFileDownload::downloadBinaryFile(url, filePath, nullptr, 4096);
  }
  else
  {
    setupProgress(fileSize);

    HTTPFileDownload::downloadBinaryFile(url, filePath, &fileProgress, 4096);
  }

  return getFileSize(filePath);
}

void Controller::downloadPodcast(PodcastChannel* channel, int podcastIndex)
{
  PodcastDetails* podcast = channel->getPodcast(podcastIndex);
  
  std::string date = getDate();
  std::string time = getTime();
  std::string url = podcast->getURL();
  std::string fileName;
  if (!tryGetFileNameFromURL(url, fileName))
  {
    fileName = removeIllegalFilePathCharactersFromText(channel->getTitle())
      + "_" + removeIllegalFilePathCharactersFromText(podcast->getTitle())
      + "_" + date + "_" + time + ".mp3";
  }

  std::string filePath = channel->getDirectory() + fileName;
  long fileSize = downloadPodcastFile(url, filePath, podcast->getFileSize());
  podcast->setFileSize(fileSize);
  podcast->setDownloadDate(date + " " + time);
}

void Controller::execute()
{
  ui->topLevelUI();
}

PodcastChannel* Controller::createChannelFromFeed(std::string feedURL, std::string directory)
{
  std::string rssFilePath = workingPath + "podcast.rss";

  downloadRSSFile(feedURL, rssFilePath);

  PodcastChannel* channel = new PodcastChannel(feedURL, directory);

  parseRSSFile(feedURL, rssFilePath, channel);

  return channel;
}

int Controller::indexOfChannelInList(const std::string& feedURL, std::vector<PodcastChannel*>& channels)
{
  for (int i = 0; i < channels.size(); i++)
  {
    PodcastChannel* channel = channels[i];
    if (channel->getFeedURL() == feedURL)
    {
      return i;
    }
  }

  return -1;
}

PodcastChannel* Controller::getChannel(int channelIndex)
{
  std::vector<PodcastChannel*> channels = storage->getChannels();
  if (channelIndex >= 0 && channelIndex < channels.size())
  {
    return channels[channelIndex];
  }

  std::string message = "Illegal value for parameter channelIndex. Is " +
    std::to_string(channelIndex) + " but should be 0.." + std::to_string(channels.size() - 1) + ".";
  throw std::range_error(message.data());
}

void Controller::removeChannel(int channelIndex)
{
  std::vector<PodcastChannel*> channels = storage->getChannels();
  PodcastChannel* channel = channels[channelIndex];
  storage->removeChannel(*channel);
}

int Controller::scanChannel(int channelIndex)
{
  std::vector<PodcastChannel*> channels = storage->getChannels();
  PodcastChannel* originalChannel = channels[channelIndex];

  PodcastChannel* newChannel = createChannelFromFeed(originalChannel->getFeedURL(), originalChannel->getDirectory());

  // Get the number of podcasts to download.
  int podcastCount = 0;
  std::string latestPublishDate = originalChannel->getPodcast(0)->getPublishedDate();
  for (int index = 0; index < newChannel->getPodcastCount(); index++)
  {
    PodcastDetails* podcast = newChannel->getPodcast(index);
    if (latestPublishDate == podcast->getPublishedDate())
    {
      break;
    }

    podcastCount++;
  }

  storage->updateChannel(*originalChannel, *newChannel);

  return podcastCount;
}

void Controller::verifyChannelIsNotInList(const std::string& url)
{
  std::vector<PodcastChannel*>& channels = storage->getChannels();

  int index;
  if ((index = indexOfChannelInList(url, channels)) != -1)
  {
    std::string message = "Channel already in storage!" + channels[index]->getTitle();
    throw new std::invalid_argument(message.data());
  }
}
