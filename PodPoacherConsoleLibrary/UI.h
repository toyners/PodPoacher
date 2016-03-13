#pragma once

#include "PodcastStorage.h"
#include "BaseController.h"
#include "IndexParser.h"

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
  void downloadPodcasts(PodcastChannel* channel, int total);
  void downloadPodcasts(PodcastChannel* channel, std::vector<int>& indexes);
  std::string getInputStringContainingWhiteSpace();
  bool haltRollingDisplayOfPodcasts(PodcastChannel* channel, int remaining);
  bool haltRollingDisplayOfChannels(int remaining);
  void handleIndexInput(std::string& input, PodcastChannel& channel);
  bool retryDownloadAfterException(std::string& message);
  int scanChannel(int number);
  void scanChannels();
  void tryDownloadPodcast(PodcastChannel* channel, int podcastIndex);
};

