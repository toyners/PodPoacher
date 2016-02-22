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

void downloadRSSFile(string url, string rssFilePath)
{
  cout << "Getting RSS file...";
  HTTPFileDownload::downloadTextFile(url, rssFilePath);
  cout << "DONE." << endl;
}

void displayParsingResults(RSSContentHandler* contentHandler)
{
  PodcastChannel* channel = contentHandler->getChannel();
  cout << "Displaying parsing results" << endl;
  cout << "CHANNEL TITLE: " << channel->getTitle() << endl;
  cout << "CHANNEL DESCRIPTION: " << channel->getDescription() << endl;
  cout << "CHANNEL WEBSITE: " << channel->getWebsite() << endl;
  cout << "PODCAST COUNT: " << channel->getPodcastCount() << endl;

  PodcastDetails* podcast = channel->getPodcast(0);
  cout << "TITLE: " << podcast->getTitle() << endl;
  cout << "DESCRIPTION: " << podcast->getDescription() << endl;
  cout << "PUB DATE: " << podcast->getPublishedDate() << endl;
  cout << "URL: " << podcast->getURL() << endl;
  cout << "FILE SIZE: " << podcast->getFileSize() << endl;

  cout << "Parsing results displayed." << endl;
}

string parseRSSFile(string rssURL, string rssFilePath)
{
  cout << "Parsing RSS file..." << endl;
  PodcastChannel podcastChannel(rssURL);
  RSSContentHandler contentHandler(&podcastChannel);
  cout << "RSS content handler created." << endl;
  XMLFileParser parser(&contentHandler);
  cout << "Parsing." << endl;
  parser.ParseFile(rssFilePath);
  cout << "RSS file parsed." << endl;

  displayParsingResults(&contentHandler);

  PodcastDetails* podcast = contentHandler.getChannel()->getPodcast(0);
  //fileSize = podcast->getFileSize();
  tickSize = podcast->getFileSize() / 20;

  return podcast->getURL();
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

int SimpleTest()
{
  //string rssURL = "http://www.giantbomb.com/podcast-xml/giant-bombcast";
  //string rssFilePath = "C:\\Projects\\PodPoacher_Test\\giant-bomb.rss";

  string rssURL = "http://www.bbc.co.uk/programmes/b00lvdrj/episodes/downloads.rss";
  string rssFilePath = "C:\\Projects\\PodPoacher_Test\\podcasts.rss";

  //string podcastURL = "http://www.giantbomb.com/podcasts/download/1501/Giant_Bombcast_02_09_2016-02-09-2016-8270465901.mp3";
  string podcastFilePath = "C:\\Projects\\PodPoacher_Test\\podcast.mp3";

  try
  {
    downloadRSSFile(rssURL, rssFilePath);

    string url = parseRSSFile(rssURL, rssFilePath);

    downloadPodcastFile(url, podcastFilePath);
  }
  catch (exception& e)
  {
    cout << e.what() << endl;
  }

  cout << "Finished.";
  string value;
  cin >> value;
  return 0;
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
  cout << "    " << channel.getDescription() << endl;
  cout << "    Podcasts: " << channel.getPodcastCount() << endl;
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

void addChannel()
{
  string input;
  cout << "Enter feed URL: ";
  cin >> input;

  vector<PodcastChannel*>& channels = storage->getChannels();

  int index;
  if ((index = indexOfChannelInList(input, channels)) != -1)
  {
    cout << "Channel already in storage:" << endl;
    displayChannelDetails(index + 1, *channels[index]);
    return;
  }

  string rssFilePath = workingPath + "podcast.rss";

  downloadRSSFile(input, rssFilePath);

  cout << "Parsing RSS file...";
  PodcastChannel* podcastChannel = new PodcastChannel(input);
  RSSContentHandler contentHandler(podcastChannel);
  XMLFileParser parser(&contentHandler);
  parser.ParseFile(rssFilePath);
  cout << "DONE." << endl;

  storage->addChannel(*podcastChannel);
}

PodcastChannel* createChannelFromFeed(string feedURL)
{
  string rssFilePath = workingPath + "podcast.rss";

  downloadRSSFile(feedURL, rssFilePath);

  cout << "Parsing RSS file...";
  PodcastChannel* podcastChannel = new PodcastChannel(feedURL);
  RSSContentHandler contentHandler(podcastChannel);
  XMLFileParser parser(&contentHandler);
  parser.ParseFile(rssFilePath);
  cout << "DONE." << endl;

  return podcastChannel;
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
  PodcastChannel* newChannel = createChannelFromFeed(originalChannel->getFeedURL());

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
  UI ui(addChannel, displayChannel, getChannelCount, scanChannel);

  ui.topLevel();

  return 0;
}

