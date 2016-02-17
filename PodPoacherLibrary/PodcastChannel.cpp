#include "stdafx.h"
#include "PodcastChannel.h"
#include <string>
#include <sstream>

using namespace std;

PodcastChannel::PodcastChannel()
{
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

void PodcastChannel::addPodcastDetails(std::string title, std::string description, std::string pubDate, std::string url, long fileSize)
{
  PodcastDetails* podcast = new PodcastDetails(
    title,
    description,
    pubDate,
    url,
    fileSize);

  podcasts->push_back(podcast);
}
