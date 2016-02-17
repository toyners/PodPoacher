#pragma once

#include <string>
#include <list>
#include "PodcastDetails.h"

class PodcastChannel
{
private:
  std::list<PodcastDetails*>* podcasts;
  std::string title;
  std::string description;
  std::string website;
  std::string feedURL;

public:
  PodcastChannel();
  ~PodcastChannel();   

  std::string getTitle() { return title; }
  std::string getDescription() { return description; }
  std::string getWebsite() { return website; }
  int getPodcastCount() { return podcasts->size(); }
  PodcastDetails* getPodcast(int index);

  void setTitle(std::string channelTitle);
  void setDescription(std::string channelDescription);
  void setWebsite(std::string channelWebsite);
  void addPodcastDetails(std::string title, std::string description, std::string pubDate, std::string url, long fileSize);
};


