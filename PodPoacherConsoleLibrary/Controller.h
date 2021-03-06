#pragma once

#include "PodcastChannel.h"
#include "PodcastStorage.h"
#include "BaseController.h"
#include "UI.h"
#include <string>
#include <vector>

class Controller : public BaseController
{
private:
  std::string workingPath;
  PodcastStorage* storage;
  UI* ui;

public:
  Controller(const std::string& path);
  ~Controller();

  void addChannel(std::string url, std::string directory);
  void downloadPodcast(PodcastChannel* channel, int podcastIndex);
  void execute();
  std::vector<PodcastChannel*>& getChannels() { return storage->getChannels(); }
  int getChannelCount() { return storage->getChannels().size(); }
  PodcastChannel* getChannel(int channelIndex);
  void removeChannel(int channelIndex);
  int scanChannel(int channelIndex);

private:
  PodcastChannel* createChannelFromFeed(std::string feedURL, std::string directory);
  long downloadPodcastFile(std::string url, std::string filePath, long fileSize);
  std::string getDate();
  std::string getTime();
  int indexOfChannelInList(const std::string& feedURL, std::vector<PodcastChannel*>& channels);
  void verifyChannelIsNotInList(const std::string& url);
};

