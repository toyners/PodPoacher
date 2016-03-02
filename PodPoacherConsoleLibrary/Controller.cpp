#include "stdafx.h"
#include "Controller.h"
#include "HTTPFileDownload.h"
#include "RSSContentHandler.h"
#include "XMLFileParser.h"
#include "FileBasedStorage.h"
#include "Utility.h"
#include <vector>
#include <ctime>

using namespace std;

Controller::Controller(const string& path)
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

void Controller::addChannel(string url, string directory)
{
  verifyChannelIsNotInList(url);

  currentChannel = createChannelFromFeed(url, directory);

  storage->addChannel(*currentChannel);
}

string Controller::getDate()
{
  time_t t = time(0);   // get time now
  struct tm * now = localtime(&t);
  return to_string(now->tm_mday) + "-" + to_string(now->tm_mon) + "-" + to_string(now->tm_year + 1900);
}

string Controller::getReadableFileSize(long size)
{
  char buffer[25];
  float sizeInKB = (float)size / 1024;
  if (sizeInKB < 0)
  {
    return to_string(size) + " b";
  }

  float sizeInMB = sizeInKB / 1024;
  if (sizeInMB < 0)
  {
    sprintf(buffer, "%.1f", sizeInKB);
    return string(buffer) + " KB";
  }

  sprintf(buffer, "%.1f", sizeInMB);
  return string(buffer) + " MB";
}

string Controller::getTime()
{
  time_t t = time(0);   // get time now
  struct tm * now = localtime(&t);
  return to_string(now->tm_hour) + "-" + to_string(now->tm_min) + "-" + to_string(now->tm_sec);
}

long Controller::downloadPodcastFile(string url, string filePath, long fileSize)
{
  setupProgress(fileSize);

  HTTPFileDownload::downloadBinaryFile(url, filePath, &fileProgress, 4096);

  long actualFileSize = getFileSize(filePath);

  cout << "DONE " << getReadableFileSize(actualFileSize) << endl;
  return actualFileSize;
}

void Controller::downloadPodcast(PodcastChannel* channel, int number)
{
  PodcastDetails* podcast = channel->getPodcast(number - 1);
  string date = getDate();
  string time = getTime();
  string filePath = channel->getDirectory() + channel->getTitle() + "_" + podcast->getTitle() + "_" + date + "_" + time + ".mp3";
  long fileSize = downloadPodcastFile(podcast->getURL(), filePath, podcast->getFileSize());
  podcast->setFileSize(fileSize);
  podcast->setDownloadDate(date + " " + time);
}

void Controller::execute()
{
  ui->topLevelUI();
}

void Controller::scanChannels(int number)
{
  if (number == -1)
  {
    for (int i = 1; i <= storage->getChannels().size(); i++)
    {
      scanChannel(i);
    }
  }
  else
  {
    scanChannel(number);
  }
}

PodcastChannel* Controller::createChannelFromFeed(string feedURL, string directory)
{
  string rssFilePath = workingPath + "podcast.rss";

  downloadRSSFile(feedURL, rssFilePath);

  PodcastChannel* channel = new PodcastChannel(feedURL, directory);

  parseRSSFile(feedURL, rssFilePath, channel);

  return channel;
}

int Controller::indexOfChannelInList(const string& feedURL, vector<PodcastChannel*>& channels)
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

void Controller::scanChannel(int number)
{
  vector<PodcastChannel*> channels = storage->getChannels();
  PodcastChannel* originalChannel = channels[number - 1];

  cout << "Scanning \"" + originalChannel->getTitle() << "\"" << endl;
  PodcastChannel* newChannel = createChannelFromFeed(originalChannel->getFeedURL(), originalChannel->getDirectory());

  if (newChannel->getPublishedDate() == originalChannel->getPublishedDate())
  {
    cout << "Scan completed. No change to \"" + originalChannel->getTitle() << "\"" << endl << endl;
    return;
  }

  // Get the number of podcasts to download.
  string latestPublishDate = originalChannel->getPodcast(0)->getPublishedDate();
  int newPodcastCount = newChannel->getPodcastCount() - originalChannel->getPodcastCount();
  for (int i = 1; i <= newChannel->getPodcastCount(); i++)
  {
    PodcastDetails* podcast = newChannel->getPodcast(i - 1);
    if (latestPublishDate == podcast->getPublishedDate())
    {
      break;
    }

    downloadPodcast(newChannel, i);
  }

  cout << "Scan completed. " << newPodcastCount << " podcast(s) added to \"" << originalChannel->getTitle() + "\"" << endl << endl;

  storage->updateChannel(*originalChannel, *newChannel);
}

void Controller::verifyChannelIsNotInList(const string& url)
{
  vector<PodcastChannel*>& channels = storage->getChannels();

  int index;
  if ((index = indexOfChannelInList(url, channels)) != -1)
  {
    string message = "Channel already in storage!" + channels[index]->getTitle();
    throw new invalid_argument(message.data());
  }
}
