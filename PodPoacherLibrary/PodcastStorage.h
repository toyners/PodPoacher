#pragma once

#include "PodcastChannel.h"
#include "PodcastDetails.h"
#include <string>
#include <vector>

class PodcastStorage
{
public:
  virtual void addChannel(PodcastChannel& channel) = 0;
  virtual std::vector<PodcastChannel*>& getChannels() = 0;
  virtual void loadChannel(PodcastChannel& channel) = 0;
  virtual void updateChannel(PodcastChannel& channel) = 0;
};