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
  std::string podcastDirectory;
  std::string feedURL;
  std::string publishedCount;
  std::string publishedDate;

public:
  PodcastChannel(const std::string& url, const std::string& directory);
  PodcastChannel(std::string url, std::string channelTitle, std::string channelDescription, std::string channelDirectory, std::string channelWebsite, std::string date);
  ~PodcastChannel();   

  std::string getTitle() { return title; }
  std::string getDescription() { return description; }
  std::string getWebsite() { return website; }
  std::string getFeedURL() { return feedURL; }
  std::string getPublishedDate() { return publishedDate; }
  std::string getDirectory() { return podcastDirectory; }
  int getPodcastCount() { return podcasts->size(); }
  PodcastDetails* getPodcast(int index);

  void setTitle(std::string channelTitle);
  void setDescription(std::string channelDescription);
  void setWebsite(std::string channelWebsite);
  void setPublishDate(std::string date);

  void addPodcastDetails(std::string title, std::string description, std::string pubDate, std::string url, long fileSize);
  void addPodcastDetails(std::string title, std::string description, std::string pubDate, std::string url, long fileSize, std::string downloadDate);
};


