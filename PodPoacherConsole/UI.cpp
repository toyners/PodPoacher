#include "stdafx.h"
#include "UI.h"
#include "PodcastStorage.h"
#include "PodcastChannel.h"
#include <iostream>
#include <vector>

using namespace std;

AddChannelCallback addChannel;
DisplayChannelsCallback displayChannel;
GetChannelCountCallback getChannelCount;
ScanChannelCallback scanChannel;

UI::UI(
  AddChannelCallback addChannelCallBack,
  DisplayChannelsCallback displayChannelsCallBack,
  GetChannelCountCallback getChannelCountCallBack,
  ScanChannelCallback scanChannelCallBack
  )
{
  addChannel = addChannelCallBack;
  displayChannel = displayChannelsCallBack;
  getChannelCount = getChannelCountCallBack;
}

UI::~UI() {}

void UI::topLevel()
{
  char input;
  while (true)
  {
    cout << "[A]dd channel, [D]isplay channels, [S]can channels or e[X]it" << endl;
    cin >> input;

    if (input == 'X' || input == 'x')
    {
      return;
    }

    if (input == 'A' || input == 'a')
    {
      addChannelUI();
    }

    if (input == 'D' || input == 'd')
    {
      displayChannelsUI();
    }

    if (input == 'S' || input == 's')
    {
      //scanChannels(channels);
    }
  }
}

void UI::displayChannelsUI()
{
  string input;
  int channelCount = getChannelCount();
  while (true)
  {
    cout << "Display [A]ll channels, numbers [1 - " << channelCount << "] or [B]ack" << endl;
    cin >> input;

    if (input == "B" || input == "b")
    {
      return;
    }

    if (input == "A" || input == "a")
    {
      displayChannel(-1);
      continue;
    }

    int number;
    if (tryConvertInputToNumber(input, number))
    {
      displayChannel(number);
    }
  }
}

void UI::scanChannelsUI()
{
  string input;
  int channelCount = getChannelCount();
  while (true)
  {
    cout << "Scan [A]ll channels, numbers [1 - " << channelCount << "] or [B]ack" << endl;
    cin >> input;

    if (input == "B" || input == "b")
    {
      return;
    }

    if (input == "A" || input == "a")
    {
      scanChannel(-1);
      continue;
    }

    int number;
    if (tryConvertInputToNumber(input, number))
    {
      scanChannel(number);
    }
  }
}

void UI::addChannelUI()
{
  string input;
  cout << "Enter feed URL: ";
  cin >> input;

  addChannel(input);
}

bool UI::tryConvertInputToNumber(string input, int& number)
{
  int channelCount = getChannelCount();
  try
  {
    number = stoi(input);

    if (number <= 0)
    {
      cout << "'" << input << "' is negative! Range is [1 - " << channelCount << "]" << endl;
      return false;
    }

    if (number > channelCount)
    {
      cout << "'" << input << "' is out of range! Range is [1 - " << channelCount << "]" << endl;
      return false;
    }

    return true;
  }
  catch (invalid_argument& i)
  {
    cout << "'" << input << "' is not a valid option! Options are [A], [B] or [1 - " << channelCount << "]" << endl;
  }
  catch (out_of_range& o)
  {
    cout << "'" << input << "' is out of range! Range is [1 - " << channelCount << "]" << endl;
  }

  return false;
}
