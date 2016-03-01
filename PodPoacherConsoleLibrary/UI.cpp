#include "stdafx.h"
#include "UI.h"
#include "PodcastStorage.h"
#include "PodcastChannel.h"
#include "Util.h"

#include <iostream>
#include <vector>

using namespace std;

UI::UI(BaseController& baseController)
{
  controller = &baseController;
}

UI::~UI() {}

void UI::topLevelUI()
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
      scanChannelsUI();
    }
  }
}

void UI::outputMessage(string message, bool appendEndOfLine)
{
  cout << message;

  if (appendEndOfLine)
  {
    cout << endl;
  }
}

void UI::displayChannelsUI()
{
  string input;
  int channelCount = controller->getChannelCount();

  if (channelCount == 0)
  {
    cout << "No channels to display." << endl;
    return;
  }

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
      displayChannels(-1);
      continue;
    }

    int number;
    if (tryConvertInputToNumber(input, number, channelCount))
    {
      displayChannels(number);
    }
  }
}

void UI::scanChannelsUI()
{
  string input;
  int channelCount = controller->getChannelCount();

  if (channelCount == 0)
  {
    cout << "No channels to scan." << endl;
    return;
  }

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
      controller->scanChannels(-1);
      continue;
    }

    int number;
    if (tryConvertInputToNumber(input, number, channelCount))
    {
      controller->scanChannels(number);
    }
  }
}

void UI::addChannelUI()
{
  string url;
  cout << "Enter feed URL: ";
  cin >> url;

  string directory;
  cout << "Enter directory for podcasts: ";
  directory = getInputContainingWhiteSpace();

  controller->addChannel(url, directory);
  PodcastChannel* channel = controller->getCurrentChannel();
  
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
      controller->downloadPodcasts(-1);
      continue;
    }

    int number;
    if (tryConvertInputToNumber(input, number, podcastCount))
    {
      controller->downloadPodcasts(number);
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
      controller->downloadPodcasts(-1);
      continue;
    }

    int number;
    if (tryConvertInputToNumber(input, number, total))
    {
      controller->downloadPodcasts(number);
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

void UI::displayChannelDetails(int number, PodcastChannel& channel)
{
  string label = "[" + to_string(number) + "] ";
  string indent(label.size(), ' ');
  cout << label << channel.getTitle() << endl;
  cout << indent << channel.getDirectory() << endl;
  cout << indent << "PODCASTS: " << channel.getPodcastCount() << "  PUBLISHED DATE: " << channel.getPublishedDate() << endl;
  cout << endl;
}

void UI::displayChannels(vector<PodcastChannel*>& channels)
{
  for (int i = 0; i < channels.size(); i++)
  {
    displayChannelDetails(i + 1, *channels[i]);
  }
}

void UI::displayChannels(int number)
{
  vector<PodcastChannel*>& channels = controller->getChannels();
  if (number == -1)
  {
    displayChannels(channels);
    return;
  }

  displayChannelDetails(number, *channels[number - 1]);
}