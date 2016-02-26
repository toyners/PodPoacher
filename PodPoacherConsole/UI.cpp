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
DownloadCallback download;

UI::UI(
  AddChannelCallback addChannelCallBack,
  DisplayChannelsCallback displayChannelsCallBack,
  GetChannelCountCallback getChannelCountCallBack,
  ScanChannelCallback scanChannelCallBack,
  DownloadCallback downloadCallBack
  )
{
  addChannel = addChannelCallBack;
  displayChannel = displayChannelsCallBack;
  getChannelCount = getChannelCountCallBack;
  download = downloadCallBack;
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
    if (tryConvertInputToNumber(input, number, channelCount))
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
    if (tryConvertInputToNumber(input, number, channelCount))
    {
      scanChannel(number);
    }
  }
}

string getInputContainingWhiteSpace()
{
  string line;
  cin.ignore(); // ignore '\n' character that is in the buffer from previous cin operation.
  getline(cin, line);
  return line;
}

void UI::addChannelUI()
{
  string url;
  cout << "Enter feed URL: ";
  cin >> url;

  string directory;
  cout << "Enter directory for podcasts: ";
  directory = getInputContainingWhiteSpace();

  PodcastChannel* channel = addChannel(url, directory);

  int podcastCount = channel->getPodcastCount();
  cout << podcastCount << " Podcasts loaded." << endl << endl;

  // Display podcasts, download all or download some
  while (true)
  {
    string input;
    cout << "[D]isplay podcasts, Download [A]ll podcasts, Download individual podcasts [1 - " << podcastCount << "] or [B]ack" << endl;
    cin >> input;
    char c = tolower(input[0]);

    if (c == 'b')
    {
      break;
    }

    if (c == 'd')
    {
      displayPodcasts(*channel);
      continue;
    }

    if (c == 'a')
    {
      download(-1);
      continue;
    }

    int number;
    if (tryConvertInputToNumber(input, number, podcastCount))
    {
      download(number);
      continue;
    }
  }
}

bool UI::tryConvertInputToNumber(string input, int& number, int podcastCount)
{
  try
  {
    number = stoi(input);

    if (number <= 0)
    {
      cout << "'" << input << "' is negative! Range is [1 - " << podcastCount << "]" << endl;
      return false;
    }

    if (number > podcastCount)
    {
      cout << "'" << input << "' is out of range! Range is [1 - " << podcastCount << "]" << endl;
      return false;
    }

    return true;
  }
  catch (invalid_argument& i)
  {
    cout << "'" << input << "' is not a valid option! Options are [A], [B] or [1 - " << podcastCount << "]" << endl;
  }
  catch (out_of_range& o)
  {
    cout << "'" << input << "' is out of range! Range is [1 - " << podcastCount << "]" << endl;
  }

  return false;
}

bool UI::haltRollingPodcastDisplay(int total, int remaining)
{
  while (true)
  {
    string input;
    cout << remaining << " Podcasts to go. [D]own, Download [A]ll podcasts, Download individual podcasts [1 - " << total << "] or [B]reak" << endl;
    cin >> input;

    char c = tolower(input[0]);
    if (c == 'b')
    {
      return false;
    }

    if (c == 'd')
    {
      return true;
    }

    if (c == 'a')
    {
      download(-1);
      continue;
    }

    int number;
    if (tryConvertInputToNumber(input, number, total))
    {
      download(number);
      continue;
    }
  }
}

void UI::displayPodcasts(PodcastChannel& channel)
{
  int count = channel.getPodcastCount();
  for (int i = 1; i < count + 1; i++)
  {
    string number = "<" + to_string(i) + "> ";
    string indent(number.size(), ' ');
    PodcastDetails* podcast = channel.getPodcast(i - 1);
    cout << number << "TITLE: " << podcast->getTitle() << endl
      << indent << "PUB DATE: " << podcast->getPublishedDate() 
      << "  SIZE: " << podcast->getFileSize();

    if (podcast->isDownloaded())
    {
      cout << "DOWNLOAD DATE: " << podcast->getDownloadDate();
    }

    cout << endl;

    if (i > 0 && i % 5 == 0)
    {
      if (!haltRollingPodcastDisplay(count, count - i))
      {
        break;
      }
    }
  }
}
