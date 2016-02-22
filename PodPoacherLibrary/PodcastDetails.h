#pragma once

#include "stdafx.h"
#include <string>

class PodcastDetails
{
private:
  std::string title;
  std::string description;
  std::string pubDate;
  std::string url;
  long fileSize;
  bool ignored;
  std::string downloadDate;

public:
  PodcastDetails(std::string podcastTitle, std::string podcastDescription, std::string podcastPubDate, std::string podcastURL, long podcastSize);
  ~PodcastDetails();

  std::string getTitle();
  std::string getDescription();
  std::string getPublishedDate();
  std::string getURL();
  std::string getDownloadDate() { return downloadDate; }
  long getFileSize();
  bool isIgnored() { return ignored; }
  bool isDownloaded() { return !downloadDate.empty(); }
  void setIgnored(bool value);
  void setDownloadDate(std::string date);
};

