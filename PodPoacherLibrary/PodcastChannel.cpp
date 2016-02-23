#include "stdafx.h"
#include "PodcastChannel.h"
#include <string>
#include <sstream>

using namespace std;

PodcastChannel::PodcastChannel(const string& url, const string& directory)
{
  feedURL = url;
  podcastDirectory = directory;
  
  // Ensure that the directory has a trailing back slash.
  if (podcastDirectory.size() > 0 && podcastDirectory[podcastDirectory.size() - 1] != '\\')
  {
    podcastDirectory += '\\';
  }

  podcasts = new list<PodcastDetails*>;
}

PodcastChannel::PodcastChannel(string url, string channelTitle, string channelDescription, string channelDirectory, string channelWebsite, string date)
{
  feedURL = url;
  title = channelTitle;
  description = channelDescription;
  podcastDirectory = channelDirectory;
  website = channelWebsite;
  publishedDate = date;
  podcasts = new list<PodcastDetails*>;
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
    throw new exception("No podcasts loaded.");
  }

  if (index >= podcasts->size())
  {
    stringstream messageBuilder;
    messageBuilder << "Index " << index << " is outside range 0.." << podcasts->size() - 1;

    throw new exception(messageBuilder.str().c_str());
  }

  int count = index;
  list<PodcastDetails*>::iterator it;
  for (it = podcasts->begin(); count > 0; it++)
  {
    count--;
  }

  return it._Ptr->_Myval;
}

void PodcastChannel::setTitle(string channelTitle)
{
  if (title == "" && channelTitle != "")
  {
    title = channelTitle;
  }
}

void PodcastChannel::setDescription(string channelDescription)
{
  if (description == "" && channelDescription != "")
  {
    description = channelDescription;
  }
}

void PodcastChannel::setWebsite(string channelWebsite)
{
  if (website == "" && channelWebsite != "")
  {
    website = channelWebsite;
  }
}

void PodcastChannel::setPublishDate(string date)
{
  if (publishedDate == "" && date != "")
  {
    publishedDate = date;
  }
}

void PodcastChannel::addPodcastDetails(string title, string description, string pubDate, string url, long fileSize)
{
  addPodcastDetails(title, description, pubDate, url, fileSize, false, "");
}

void PodcastChannel::addPodcastDetails(string title, string description, string pubDate, string url, long fileSize, bool ignored, string downloadDate)
{
  PodcastDetails* podcast = new PodcastDetails(
    title,
    description,
    pubDate,
    url,
    fileSize);

  podcast->setIgnored(ignored);
  podcast->setDownloadDate(downloadDate);

  podcasts->push_back(podcast);
}
