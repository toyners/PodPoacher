#include "stdafx.h"
#include "UI.h"
#include "PodcastStorage.h"
#include "PodcastChannel.h"
#include "Utility.h"

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

    char c = tolower(input[0]);
    if (c == 'b')
    {
      return;
    }

    if (c == 'a')
    {
      displayChannels();
      continue;
    }

    int number;
    if (tryConvertInputToNumber(input, number, channelCount))
    {
      vector<PodcastChannel*>& channels = controller->getChannels();
      displayChannel(number, *channels[number - 1]);
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

    char c = tolower(input[0]);
    if (c == 'b')
    {
      return;
    }

    if (c == 'a')
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
  directory = getInputStringContainingWhiteSpace();

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
      downloadAllPodcasts(channel);
      continue;
    }

    int number;
    if (tryConvertInputToNumber(input, number, podcastCount))
    {
      cout << "Getting MP3 file";
      controller->downloadPodcast(channel, number);
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

bool UI::haltRollingDisplay(int total, int remaining)
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
      downloadAllPodcasts(controller->getCurrentChannel());
      continue;
    }

    int number;
    if (tryConvertInputToNumber(input, number, total))
    {
      controller->downloadPodcast(controller->getCurrentChannel(), number);
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
      if (!haltRollingDisplay(count, count - i))
      {
        break;
      }
    }
  }
}

void UI::displayChannel(int number, PodcastChannel& channel)
{
  string label = "[" + to_string(number) + "] ";
  string indent(label.size(), ' ');
  cout << label << channel.getTitle() << endl;
  cout << indent << channel.getDirectory() << endl;
  cout << indent << "PODCASTS: " << channel.getPodcastCount() << "  PUBLISHED DATE: " << channel.getPublishedDate() << endl;
  cout << endl;

  int podcastCount = channel.getPodcastCount();
  while (true)
  {
    string input;
    cout <<  "Display [A]ll podcasts. Download individual podcast [1 - " << podcastCount << "] or [B]ack" << endl;
    cin >> input;

    char c = tolower(input[0]);
    if (c == 'b')
    {
      return;
    }

    if (c == 'a')
    {
      displayPodcasts(channel);
      continue;
    }

    int number;
    if (tryConvertInputToNumber(input, number, podcastCount))
    {
      controller->downloadPodcast(&channel, number);
    }
  }

}

void UI::displayChannels()
{
  vector<PodcastChannel*>& channels = controller->getChannels();
  for (int i = 0; i < channels.size(); i++)
  {
    displayChannel(i + 1, *channels[i]);
  }
}

void UI::downloadAllPodcasts(PodcastChannel* channel)
{
  int total = channel->getPodcastCount();
  for (int number = 1; number <= total; number++)
  {
    cout << "Getting MP3 file [" << number << " of " << total << "]";
    controller->downloadPodcast(channel, number);
  }
}

std::string UI::getInputStringContainingWhiteSpace()
{
  string line;
  cin.ignore(); // ignore '\n' character that is in the buffer from previous cin operation.
  getline(cin, line);
  return line;
}