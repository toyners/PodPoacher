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

using namespace std;

string workingPath;
PodcastStorage* storage;
long tickSize;
long tickCount = 0;
PodcastChannel* currentChannel;

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

void FileProgress(long filePosition)
{
  int ticks = filePosition / tickSize;
  while (ticks > tickCount)
  {
    tickCount++;
    //cout << ".";
    cout << filePosition << endl;
  }
}

void downloadPodcastFile(string url, string filePath)
{
  cout << "Streaming MP3 file...";
  HTTPFileDownload::downloadBinaryFile(url, filePath, &FileProgress, 4096);
  cout << endl;
  cout << "DONE." << endl;
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
  cout << "[" << to_string(number) << "] " << channel.getTitle() << endl;
  cout << "    " << channel.getDirectory() << endl;
  cout << "    Podcasts: " << channel.getPodcastCount() << " Published Date: " << channel.getPublishedDate() << endl;
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

void displayPodcasts(PodcastChannel& channel)
{
  int count = channel.getPodcastCount();
  for (int i = 1; i < count + 1; i++)
  {
    string number = "<" + to_string(i) + "> ";
    string indent(number.size(), ' ');
    PodcastDetails* podcast = channel.getPodcast(i - 1);
    cout << number << "TITLE: " << podcast->getTitle() << endl;
    cout << indent << "DESCRIPTION: " << podcast->getDescription() << endl;
    cout << indent << "PUB DATE: " << podcast->getPublishedDate() << " SIZE: " << podcast->getFileSize() << endl;

    if (podcast->isDownloaded() || podcast->isIgnored())
    {
      if (podcast->isIgnored())
      {
        cout << indent << "IGNORED ";
      }
      else
      {
        cout << indent << "        ";
      }

      if (podcast->isDownloaded())
      {
        cout << "DOWNLOAD DATE: " << podcast->getDownloadDate();
      }

      cout << endl;
    }

    cout << endl;

    if (i > 0 && i % 5 == 0)
    {
      string input;
      cout << (count - i) << " Podcasts to go. [C]ontinue or [S]top" << endl;
      cin >> input;

      char c = tolower(input[0]);
      if (c == 's')
      {
        break;
      }
    }
  }
}

void downloadPodcasts(int index)
{
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

void scanChannels(vector<PodcastChannel*>& channels)
{

}

void scanChannel(int number)
{
  vector<PodcastChannel*>& channels = storage->getChannels();
  if (number == -1)
  {
    scanChannels(channels);
    return;
  }

  PodcastChannel* originalChannel = channels[number - 1];
  PodcastChannel* newChannel = createChannelFromFeed(originalChannel->getFeedURL(), originalChannel->getDirectory());

  if (newChannel->getPublishedDate() == originalChannel->getPublishedDate())
  {

  }
}

int getChannelCount()
{
  return storage->getChannels().size();
}

int main()
{
  //SimpleTest();
  //return 0;

  workingPath = "C:\\Projects\\PodPoacher_Test\\Working\\";
  storage = new FileBasedStorage("C:\\Projects\\PodPoacher_Test\\Data\\");
  UI ui(addChannel, displayChannel, getChannelCount, scanChannel, downloadPodcasts);

  ui.topLevel();

  return 0;
}

