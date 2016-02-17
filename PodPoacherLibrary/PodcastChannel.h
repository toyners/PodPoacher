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
  PodcastChannel(std::string feedURL);
  ~PodcastChannel();   

  std::string getTitle() { return title; }
  std::string getDescription() { return description; }
  std::string getWebsite() { return website; }
  std::string getFeedURL() { return feedURL; }
  int getPodcastCount() { return podcasts->size(); }
  PodcastDetails* getPodcast(int index);

  void setTitle(std::string channelTitle);
  void setDescription(std::string channelDescription);
  void setWebsite(std::string channelWebsite);

  void addPodcastDetails(std::string title, std::string description, std::string pubDate, std::string url, long fileSize);
};


