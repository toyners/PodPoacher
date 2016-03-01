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
  PodcastChannel* currentChannel;
  UI* ui;

public:
  Controller(const std::string& path);
  ~Controller();

  void addChannel(std::string url, std::string directory);
  void displayChannel(int number);
  void downloadPodcasts(int number);
  void execute();
  std::vector<PodcastChannel*>& getChannels() { return storage->getChannels(); }
  int getChannelCount() { return storage->getChannels().size(); }
  PodcastChannel* getCurrentChannel() { return currentChannel; }
  void scanChannels(int number);

private:
  PodcastChannel* createChannelFromFeed(std::string feedURL, std::string directory);
  void displayChannelDetails(int number, PodcastChannel& channel);
  void displayChannels(std::vector<PodcastChannel*>& channels);
  void downloadOneOfMultiplePodcasts(PodcastChannel* channel, int number, int total);
  long downloadPodcastFile(std::string url, std::string filePath, long fileSize);
  void downloadPodcast(PodcastChannel* channel, int number);
  void downloadSinglePodcast(PodcastChannel* channel, int number);
  std::string getDate();
  std::string getReadableFileSize(long size);
  std::string getTime();
  int indexOfChannelInList(const std::string& feedURL, std::vector<PodcastChannel*>& channels);
  void scanChannel(int number);
  void verifyChannelIsNotInList(const std::string& url);
};

