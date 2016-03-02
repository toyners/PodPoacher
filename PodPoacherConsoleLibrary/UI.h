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

  void outputMessage(std::string message, bool appendEndOfLine);

private:
  void displayChannelsUI();
  void scanChannelsUI();
  void addChannelUI();
  bool tryConvertInputToNumber(std::string input, int& number, int total);
  void displayChannel(int number, PodcastChannel& channel);
  void displayChannelDetails(int number, PodcastChannel& channel);
  void displayChannels();
  void displayPodcasts(PodcastChannel& channel);
  void downloadPodcast(PodcastChannel* channel, int number);
  void downloadAllPodcasts(PodcastChannel* channel);
  std::string getInputStringContainingWhiteSpace();
  bool haltRollingDisplayOfPodcasts(int total, int remaining);
  bool haltRollingDisplayOfChannels(int remaining);
};

