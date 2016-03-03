#pragma once

#include <string>
#include <vector>

class BaseController
{
public:
  virtual void addChannel(std::string url, std::string directory) = 0;
  virtual void downloadPodcast(PodcastChannel* channel, int podcastIndex) = 0;
  virtual void execute() = 0;
  virtual PodcastChannel* getChannel(int channelIndex) = 0;
  virtual int getChannelCount() = 0;
  virtual std::vector<PodcastChannel*>& getChannels() = 0;
  virtual PodcastChannel* getCurrentChannel() = 0;
  virtual int scanChannel(int channelIndex) = 0;
};

