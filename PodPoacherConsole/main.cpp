// main.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "HTTPFileDownload.h"
#include "XMLFileParser.h"
#include "HTTPFileStreamer.h"
#include "RSSContentHandler.h"
#include "FileBasedStorage.h"
#include "UI.h"
#include <string>
#include <iostream>
#include <vector>
#include <ctime>

using namespace std;

string workingPath;
PodcastStorage* storage;
long tickSize;
long tickCount = 0;
PodcastChannel* currentChannel;
long filePosition;

void downloadRSSFile(string url, string rssFilePath)
{
  cout << "Getting RSS file...";
  HTTPFileDownload::downloadTextFile(url, rssFilePath);
  cout << "DONE." << endl;
}

void parseRSSFile(string url, string rssFilePath, PodcastChannel* channel)
{
  cout << "Parsing RSS file...";
  RSSContentHandler contentHandler(channel);
  XMLFileParser parser(&contentHandler);
  parser.ParseFile(rssFilePath);
  cout << "DONE." << endl;
}

void FileProgress(long position)
{
  int ticks = position / tickSize;
  while (ticks > tickCount)
  {
    tickCount++;
    cout << ".";
  }

  filePosition = position;
}

int indexOfChannelInList(const string& feedURL, vector<PodcastChannel*>& channels)
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

void displayChannelDetails(int number, PodcastChannel& channel)
{
  string label = "[" + to_string(number) + "] ";
  string indent(label.size(), ' ');
  cout << label << channel.getTitle() << endl;
  cout << indent << channel.getDirectory() << endl;
  cout << indent << "PODCASTS: " << channel.getPodcastCount() << "  PUBLISHED DATE: " << channel.getPublishedDate() << endl;
  cout << endl;
}

void displayChannels(vector<PodcastChannel*>& channels)
{
  for (int i = 0; i < channels.size(); i++)
  {
    displayChannelDetails(i + 1, *channels[i]);
  }
}

void displayChannel(int number)
{
  vector<PodcastChannel*>& channels = storage->getChannels();
  if (number == -1)
  {
    displayChannels(channels);
    return;
  }

  displayChannelDetails(number, *channels[number - 1]);
}

string getDate()
{
  time_t t = time(0);   // get time now
  struct tm * now = localtime(&t);
  return to_string(now->tm_mday) + "-" + to_string(now->tm_mon) + "-" + to_string(now->tm_year + 1900);
}

string getTime()
{
  time_t t = time(0);   // get time now
  struct tm * now = localtime(&t);
  return to_string(now->tm_hour) + "-" + to_string(now->tm_min) + "-" + to_string(now->tm_sec);
}

string getReadableFileSize(long size)
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

long downloadPodcastFile(string url, string filePath, long fileSize)
{
  tickSize = fileSize / 20;
  tickCount = 0;
  filePosition = 0;

  HTTPFileDownload::downloadBinaryFile(url, filePath, &FileProgress, 4096);
  cout << "DONE. " << getReadableFileSize(filePosition) << endl;
  return filePosition;
}

void downloadPodcast(PodcastChannel* channel, int number)
{
  PodcastDetails* podcast = channel->getPodcast(number - 1);
  string date = getDate();
  string time = getTime();
  string filePath = channel->getDirectory() + channel->getTitle() + "_" + podcast->getTitle() + "_" + date + "_" + time + ".mp3";
  long fileSize = downloadPodcastFile(podcast->getURL(), filePath, podcast->getFileSize());
  podcast->setFileSize(fileSize);
  podcast->setDownloadDate(date + " " + time);
}

void downloadPodcasts(int number)
{
  if (number == -1)
  {
    int count = currentChannel->getPodcastCount();
    for (int i = 1; i <= count; i++)
    {
      cout << "Getting MP3 file [" << i << " of " << count << "]";
      downloadPodcast(currentChannel, i);
    }
  }
  else
  {
    cout << "Getting MP3 file";
    downloadPodcast(currentChannel, number);
  }

  storage->updateChannel(*currentChannel);
}

void verifyChannelIsNotInList(string url)
{
  vector<PodcastChannel*>& channels = storage->getChannels();

  int index;
  if ((index = indexOfChannelInList(url, channels)) != -1)
  {
    string message = "Channel already in storage!" + channels[index]->getTitle();
    throw new invalid_argument(message.data());
  }
}

PodcastChannel* addChannel(string url, string directory)
{
  verifyChannelIsNotInList(url);

  string rssFilePath = workingPath + "podcast.rss";
  downloadRSSFile(url, rssFilePath);

  currentChannel = new PodcastChannel(url, directory);
  parseRSSFile(url, rssFilePath, currentChannel);

  storage->addChannel(*currentChannel);

  return currentChannel;
}

PodcastChannel* createChannelFromFeed(string feedURL, string directory)
{
  string rssFilePath = workingPath + "podcast.rss";

  downloadRSSFile(feedURL, rssFilePath);
  
  PodcastChannel* channel = new PodcastChannel(feedURL, directory);

  parseRSSFile(feedURL, rssFilePath, channel);

  cout << "Parsing RSS file...";
  
  return channel;
}

void scanChannel(int number)
{
  vector<PodcastChannel*> channels = storage->getChannels();
  PodcastChannel* originalChannel = channels[number - 1];
  PodcastChannel* newChannel = createChannelFromFeed(originalChannel->getFeedURL(), originalChannel->getDirectory());

  if (newChannel->getPublishedDate() == originalChannel->getPublishedDate())
  {
    return;
  }

  // Get the number of podcasts to download.
  string latestPublishDate = originalChannel->getPodcast(0)->getPublishedDate();
  for (int i = 0; i < newChannel->getPodcastCount(); i++)
  {
    PodcastDetails* podcast = newChannel->getPodcast(i);
    if (latestPublishDate == podcast->getPublishedDate())
    {
      break;
    }

    cout << "Getting MP3 file";
    downloadPodcast(newChannel, i);
  }

  delete originalChannel;
  channels[number - 1] = newChannel;
  storage->updateChannel(*newChannel);
}

void scanChannels(int number)
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

int getChannelCount()
{
  return storage->getChannels().size();
}

int main()
{
  workingPath = "C:\\Projects\\PodPoacher_Test\\Working\\";
  storage = new FileBasedStorage("C:\\Projects\\PodPoacher_Test\\Data\\");
  UI ui(addChannel, displayChannel, getChannelCount, scanChannels, downloadPodcasts);

  ui.topLevel();

  return 0;
}

