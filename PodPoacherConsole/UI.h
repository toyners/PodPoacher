#pragma once

#include "PodcastStorage.h"

typedef void(*AddChannelCallback)();
typedef void(*DisplayChannelsCallback)(int);
typedef int(*GetChannelCountCallback)();
typedef void(*ScanChannelCallback)(int);

class UI
{
public:
  UI(
    AddChannelCallback addChannelCallBack,
    DisplayChannelsCallback displayChannelsCallBack, 
    GetChannelCountCallback getChannelCountCallBack,
    ScanChannelCallback scanChannelCallBack
    );

  ~UI();

  void topLevel();

private:
  void displayChannelsUI();
  void scanChannelsUI();
  bool tryConvertInputToNumber(std::string input, int& number);
};

