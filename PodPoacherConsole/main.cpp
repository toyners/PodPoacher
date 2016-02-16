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
using namespace Jabberwocky::BasicHTTPLibrary;

void getRSSFile(string url, string rssFilePath)
{
  cout << "Getting RSS file..." << endl;
  HTTPFileDownload fileDownload;
  fileDownload.DownloadFile(url, rssFilePath);
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

void parseRSSFile(string rssFilePath)
{
  cout << "Parsing RSS file..." << endl;
  RSSContentHandler contentHandler;
  cout << "RSS content handler created." << endl;
  XMLFileParser parser(&contentHandler);
  cout << "Parsing." << endl;
  parser.ParseFile(rssFilePath);
  cout << "RSS file parsed." << endl;

  displayParsingResults(&contentHandler);
}

void streamPodcastFile(string url, string filePath)
{
  cout << "Streaming MP3 file..." << endl;
  HTTPFileStreamer podcastStreamer;
  podcastStreamer.StreamFile(url, filePath);
  cout << "MP3 file downloaded." << endl;
}

int main()
{
  //string rssURL = "http://www.giantbomb.com/podcast-xml/giant-bombcast";
  //string rssFilePath = "C:\\Projects\\PodPoacher\\Test\\giant-bomb.rss";

  string rssURL = "http://www.bbc.co.uk/programmes/b00lvdrj/episodes/downloads.rss";
  string rssFilePath = "C:\\Projects\\PodPoacher\\Test\\bbc.rss";

  string podcastURL = "http://www.giantbomb.com/podcasts/download/1501/Giant_Bombcast_02_09_2016-02-09-2016-8270465901.mp3";
  string podcastFilePath = "C:\\Projects\\PodPoacher\\Test\\giantbomb.mp3";

  try
  {
    //getRSSFile(rssURL, rssFilePath);

    parseRSSFile(rssFilePath);

    //streamPodcastFile(url, podcastFilePath);
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

