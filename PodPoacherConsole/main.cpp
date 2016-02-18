// main.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "HTTPFileDownload.h"
#include "XMLFileParser.h"
#include "HTTPFileStreamer.h"
#include "RSSContentHandler.h"
#include <string>
#include <iostream>

using namespace std;

void downloadRSSFile(string url, string rssFilePath)
{
  cout << "Getting RSS file..." << endl;
  HTTPFileDownload::downloadTextFile(url, rssFilePath);
  cout << "RSS file downloaded." << endl;
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

//long fileSize;
long tickSize;

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

long tickCount = 0;

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
  cout << "Streaming MP3 file..." << endl;
  HTTPFileDownload::downloadBinaryFile(url, filePath, &FileProgress, 4096);
  cout << endl;
  cout << "MP3 file downloaded." << endl;
}

int main()
{
  //string rssURL = "http://www.giantbomb.com/podcast-xml/giant-bombcast";
  //string rssFilePath = "C:\\Projects\\PodPoacher_Test\\giant-bomb.rss";

  string rssURL = "http://www.bbc.co.uk/programmes/b00lvdrj/episodes/downloads.rss";
  string rssFilePath = "C:\\Projects\\PodPoacher_Test\\podcasts.rss";

  //string podcastURL = "http://www.giantbomb.com/podcasts/download/1501/Giant_Bombcast_02_09_2016-02-09-2016-8270465901.mp3";
  string podcastFilePath = "C:\\Projects\\PodPoacher_Test\\podcast.mp3";

  try
  {
    //downloadRSSFile(rssURL, rssFilePath);
    
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

