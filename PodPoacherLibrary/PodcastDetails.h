#pragma once

#include "stdafx.h"
#include <string>

class PodcastDetails
{
public:
  PodcastDetails(std::string podcastTitle, std::string podcastDescription, std::string podcastPubDate, std::string podcastURL, long podcastSize);
  ~PodcastDetails();

  std::string getTitle();
  std::string getDescription();
  std::string getPublishedDate();
  std::string getURL();
  long getFileSize();

private:
  std::string title;
  std::string description;
  std::string pubDate;
  std::string url;
  long fileSize;
};

