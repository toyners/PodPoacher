#pragma once

#include "PodcastStorage.h"

typedef PodcastChannel*(*AddChannelCallback)(std::string, std::string);
typedef void(*DisplayChannelsCallback)(int);
typedef void(*DisplayPodcastsCallback)(int);
typedef int(*GetChannelCountCallback)();
typedef void(*ScanChannelCallback)(int);
typedef void(*DownloadCallback)(int);

class UI
{
public:
  UI(
    AddChannelCallback addChannelCallBack,
    DisplayChannelsCallback displayChannelsCallBack, 
    GetChannelCountCallback getChannelCountCallBack,
    ScanChannelCallback scanChannelCallBack,
    DownloadCallback downloadCallBack
    );

  ~UI();

  void topLevel();

private:
  void displayChannelsUI();
  void scanChannelsUI();
  void addChannelUI();
  bool tryConvertInputToNumber(std::string input, int& number);
  void displayPodcasts(PodcastChannel& channel);
  bool haltRollingPodcastDisplay(int total, int remaining);
};

