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
  std::string publishedCount;

public:
  PodcastChannel(const std::string& url);
  PodcastChannel(std::string url, std::string channelTitle, std::string channelDescription, std::string channelWebsite, std::string channelCount);
  ~PodcastChannel();   

  std::string getTitle() { return title; }
  std::string getDescription() { return description; }
  std::string getWebsite() { return website; }
  std::string getFeedURL() { return feedURL; }
  std::string getPublishedCount() { return publishedCount; }
  int getPodcastCount() { return podcasts->size(); }
  PodcastDetails* getPodcast(int index);

  void setTitle(std::string channelTitle);
  void setDescription(std::string channelDescription);
  void setWebsite(std::string channelWebsite);

  void addPodcastDetails(std::string title, std::string description, std::string pubDate, std::string url, long fileSize);
};


