#include "stdafx.h"
#include "PodcastDetails.h"

using namespace std;

PodcastDetails::PodcastDetails(string podcastTitle, string podcastDescription, string podcastPubDate, string podcastURL, long podcastSize)
{
  title = podcastTitle;
  description = podcastDescription;
  pubDate = podcastPubDate;
  url = podcastURL;
  fileSize = podcastSize;
}

PodcastDetails::~PodcastDetails() {}

string PodcastDetails::getTitle()
{
  return title;
}

string PodcastDetails::getDescription()
{
  return description;
}

string PodcastDetails::getPublishedDate()
{
  return pubDate;
}

string PodcastDetails::getURL()
{
  return url;
}

long PodcastDetails::getFileSize()
{
  return fileSize;
}

void PodcastDetails::setDownloadDate(std::string date)
{
  if (downloadDate == "")
  {
    downloadDate = date;
  }
}
