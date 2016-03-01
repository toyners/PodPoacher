#pragma once

#include <string>

class BaseController
{
public:
  virtual void addChannel(std::string url, std::string directory) = 0;
  virtual void downloadPodcast(PodcastChannel* channel, int number) = 0;
  virtual void execute() = 0;
  virtual int getChannelCount() = 0;
  virtual std::vector<PodcastChannel*>& getChannels() = 0;
  virtual void scanChannels(int number) = 0;
  virtual PodcastChannel* getCurrentChannel() = 0;
};

