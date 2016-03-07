#include "stdafx.h"
#include "PodcastChannel.h"
#include <string>
#include <sstream>

PodcastChannel::PodcastChannel(const std::string& url, const std::string& directory)
{
  feedURL = url;
  podcastDirectory = directory;
  
  // Ensure that the directory has a trailing back slash.
  if (podcastDirectory.size() > 0 && podcastDirectory[podcastDirectory.size() - 1] != '\\')
  {
    podcastDirectory += '\\';
  }

  podcasts = new std::list<PodcastDetails*>;
}

PodcastChannel::PodcastChannel(std::string url, std::string channelTitle, std::string channelDescription, std::string channelDirectory, std::string channelWebsite, std::string date)
{
  feedURL = url;
  title = channelTitle;
  description = channelDescription;
  podcastDirectory = channelDirectory;
  website = channelWebsite;
  publishedDate = date;
  podcasts = new std::list<PodcastDetails*>;
}

PodcastChannel::~PodcastChannel()
{
  podcasts->clear();
  delete podcasts;
}

PodcastDetails* PodcastChannel::getPodcast(int index)
{
  if (podcasts->size() == 0)
  {
    throw new std::exception("No podcasts loaded.");
  }

  if (index >= podcasts->size())
  {
    std::stringstream messageBuilder;
    messageBuilder << "Index " << index << " is outside range 0.." << podcasts->size() - 1;

    throw new std::exception(messageBuilder.str().c_str());
  }

  int count = index;
  std::list<PodcastDetails*>::iterator it;
  for (it = podcasts->begin(); count > 0; it++)
  {
    count--;
  }

  return it._Ptr->_Myval;
}

void PodcastChannel::setTitle(std::string channelTitle)
{
  if (title == "" && channelTitle != "")
  {
    title = channelTitle;
  }
}

void PodcastChannel::setDescription(std::string channelDescription)
{
  if (description == "" && channelDescription != "")
  {
    description = channelDescription;
  }
}

void PodcastChannel::setWebsite(std::string channelWebsite)
{
  if (website == "" && channelWebsite != "")
  {
    website = channelWebsite;
  }
}

void PodcastChannel::setPublishDate(std::string date)
{
  if (publishedDate == "" && date != "")
  {
    publishedDate = date;
  }
}

void PodcastChannel::addPodcastDetails(std::string title, std::string description, std::string url, std::string pubDate, long fileSize)
{
  addPodcastDetails(title, description, url, pubDate, fileSize, "");
}

void PodcastChannel::addPodcastDetails(std::string title, std::string description, std::string url, std::string pubDate, long fileSize, std::string downloadDate)
{
  PodcastDetails* podcast = new PodcastDetails(
    title,
    description,
    url,
    pubDate,
    fileSize);

  podcast->setDownloadDate(downloadDate);

  podcasts->push_back(podcast);
}
