#pragma once

#include <string>
#include <list>
#include "PodcastDetails.h"

class PodcastChannel
{
private:
  std::list<PodcastDetails*>* podcasts;
  std::string channelTitle;
  std::string channelDescription;
  std::string channelWebsite;

public:
  PodcastChannel();
  ~PodcastChannel();

  std::string getTitle();
  std::string getDescription();
  std::string getWebsite();
  int getPodcastCount();
  PodcastDetails* getPodcast(int index);

  void setTitle(std::string title);
  void setDescription(std::string description);
  void setWebsite(std::string website);
  void addPodcastDetails(std::string title, std::string description, std::string pubDate, std::string url, long fileSize);
};


