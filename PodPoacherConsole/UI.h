#pragma once

#include "PodcastStorage.h"

typedef PodcastChannel*(*AddChannelCallback)(std::string, std::string);
typedef void(*DisplayChannelsCallback)(int);
typedef void(*DisplayPodcastsCallback)(int);
typedef int(*GetChannelCountCallback)();
typedef void(*ScanChannelsCallback)(int);
typedef void(*DownloadCallback)(int);

class UI
{
public:
  UI(
    AddChannelCallback addChannelCallBack,
    DisplayChannelsCallback displayChannelsCallBack, 
    GetChannelCountCallback getChannelCountCallBack,
    ScanChannelsCallback scanChannelsCallBack,
    DownloadCallback downloadCallBack
    );

  ~UI();

  void topLevel();

private:
  void displayChannelsUI();
  void scanChannelsUI();
  void addChannelUI();
  bool tryConvertInputToNumber(std::string input, int& number, int total);
  void displayPodcasts(PodcastChannel& channel);
  bool haltRollingPodcastDisplay(int total, int remaining);
};

