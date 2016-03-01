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
  void displayChannels(int number);
  void displayChannelDetails(int number, PodcastChannel& channel);
  void displayChannels(std::vector<PodcastChannel*>& channels);
  void displayPodcasts(PodcastChannel& channel);
  std::string getInputStringContainingWhiteSpace();
  bool haltRollingDisplay(int total, int remaining);
};

