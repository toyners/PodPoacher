#include "stdafx.h"
#include "PodcastDetails.h"

PodcastDetails::PodcastDetails(std::string podcastTitle, std::string podcastDescription, std::string podcastURL, std::string podcastPubDate, long podcastSize)
{
  title = podcastTitle;
  description = podcastDescription;
  pubDate = podcastPubDate;
  url = podcastURL;
  fileSize = podcastSize;
}

PodcastDetails::~PodcastDetails() {}

std::string PodcastDetails::getTitle()
{
  return title;
}

std::string PodcastDetails::getDescription()
{
  return description;
}

std::string PodcastDetails::getPublishedDate()
{
  return pubDate;
}

std::string PodcastDetails::getURL()
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
