#pragma once

#include "PodcastChannel.h"
#include "PodcastDetails.h"
#include <string>
#include <vector>

class PodcastStorage
{
public:
  virtual void AddChannel(PodcastChannel& channel) = 0;
  virtual std::vector<PodcastChannel*>& GetChannels() = 0;
  virtual void LoadChannel(PodcastChannel& channel) = 0;
  virtual void UpdateChannel(PodcastChannel& channel) = 0;
};