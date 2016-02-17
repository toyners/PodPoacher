#pragma once

#include "PodcastChannel.h"
#include "PodcastDetails.h"
#include <string>

class PodcastStorage
{
public:
  virtual void AddChannel(PodcastChannel* channel) = 0;
  virtual std::list<std::string> GetChannelList() = 0;
  virtual PodcastChannel* GetChannel(std::string feedURL) = 0;
  virtual void LoadChannel(PodcastChannel* channel) = 0;
  virtual void UpdateChannel(PodcastChannel* channel) = 0;
};