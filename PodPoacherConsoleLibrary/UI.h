#pragma once

#include "PodcastStorage.h"
#include "BaseController.h"
#include "IndexParser.h"
#include "PodcastChannel.h"
#include <map>

class UI
{
private:
  BaseController* controller;
  IndexParser* indexParser;

public:
  UI(BaseController& baseController);

  ~UI();

  void topLevelUI();

private:
  void displayChannelsUI();
  void scanChannelsUI();
  bool addChannel();
  void addChannelUI();
  bool tryConvertInputToNumber(std::string input, int& number, int total);
  void displayChannel(int number, PodcastChannel& channel);
  void displayChannelDetails(int number, PodcastChannel& channel);
  void displayChannels();
  void displayPodcasts(PodcastChannel& channel);
  void downloadPodcasts(PodcastChannel* channel);
  void downloadPodcasts(PodcastChannel* channel, int total, std::map<PodcastChannel*, int>* downloadCounter = nullptr);
  void downloadPodcasts(PodcastChannel* channel, std::vector<int>& indexes);
  std::string getInputStringContainingWhiteSpace();
  bool haltRollingDisplayOfPodcasts(PodcastChannel* channel, int remaining);
  bool haltRollingDisplayOfChannels(int remaining, int total, std::vector<PodcastChannel*>& channels);
  void handleIndexInput(std::string& input, PodcastChannel& channel);
  void removeChannelsUI();
  bool retryDownloadAfterException(std::string& message);
  int scanChannel(int number);
  void scanChannels();
  void tryDownloadPodcast(PodcastChannel* channel, int podcastIndex, std::map<PodcastChannel*, int>* downloadCounter = nullptr);
};

