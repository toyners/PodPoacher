#pragma once

#include "PodcastStorage.h"
#include "BaseController.h"

class UI
{
private:
  BaseController* controller;

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
  void downloadPodcast(PodcastChannel* channel, int number);
  void downloadPodcasts(PodcastChannel* channel);
  void downloadPodcasts(PodcastChannel* channel, int total);
  std::string getInputStringContainingWhiteSpace();
  bool haltRollingDisplayOfPodcasts(PodcastChannel* channel, int remaining);
  bool haltRollingDisplayOfChannels(int remaining);
  bool retryDownloadAfterException(std::string& message);
  int scanChannel(int number);
  void scanChannels();
};

