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
  std::cout << "PodPoacher v0.93" << std::endl << std::endl;

  while (true)
  {
    char input;
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
      scanChannels();
      continue;
    }

    int number;
    if (tryConvertInputToNumber(input, number, channelCount))
    {
      int channelIndex = number - 1;
      int downloadCount = scanChannel(channelIndex);
      if (downloadCount > 0)
      { 
        downloadPodcasts(controller->getChannel(channelIndex), downloadCount);
      }
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
  int channelIndex = controller->getChannelCount() - 1;
  PodcastChannel* channel = controller->getChannel(channelIndex);
  
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
      downloadPodcasts(channel, channel->getPodcastCount());
      continue;
    }

    int number;
    if (tryConvertInputToNumber(input, number, podcastCount))
    {
      downloadPodcast(channel, number);
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

bool UI::haltRollingDisplayOfPodcasts(PodcastChannel* channel, int remaining)
{
  int total = channel->getPodcastCount();
  while (true)
  {
    string input;
    cout << remaining << " Podcasts to go. [D]own, Download [A]ll podcasts, Download individual podcasts [1 - " << total << "] or [B]reak" << endl;
    cin >> input;

    char c = tolower(input[0]);
    if (c == 'b')
    {
      return true;
    }

    if (c == 'd')
    {
      return false;
    }

    if (c == 'a')
    {
      downloadPodcasts(channel);
      continue;
    }

    int number;
    if (tryConvertInputToNumber(input, number, total))
    {
      downloadPodcast(channel, number);
      continue;
    }
  }
}

bool UI::haltRollingDisplayOfChannels(int remaining)
{
  string input;
  cout << remaining << " Channels to go. [D]own or [B]reak" << endl;
  cin >> input;

  char c = tolower(input[0]);
  if (c == 'b')
  {
    return true;
  }

  if (c == 'd')
  {
    return false;
  }
}

void UI::displayPodcasts(PodcastChannel& channel)
{
  int total = channel.getPodcastCount();
  for (int index = 0; index < total; index++)
  {
    int number = index + 1;
    string label = "<" + to_string(number) + "> ";
    string indent(label.size(), ' ');
    PodcastDetails* podcast = channel.getPodcast(index);
    cout << label << "TITLE: " << podcast->getTitle() << endl
      << indent << "PUB DATE: " << podcast->getPublishedDate() 
      << "  SIZE: " << podcast->getFileSize();

    if (podcast->isDownloaded())
    {
      cout << endl << label << "DOWNLOAD DATE: " << podcast->getDownloadDate();
    }

    cout << endl << endl;

    int remaining = total - number;
    if (number > 0 && number % 5 == 0 && remaining > 0 && haltRollingDisplayOfPodcasts(&channel, remaining))
    {
      break;
    }
  }
}

void UI::displayChannel(int number, PodcastChannel& channel)
{
  displayChannelDetails(number, channel);

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
      downloadPodcast(&channel, number);
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

void UI::displayChannels()
{
  vector<PodcastChannel*>& channels = controller->getChannels();
  int total = channels.size();
  for (int index = 0; index < total; index++)
  {
    int number = index + 1;
    displayChannelDetails(number, *channels[index]);

    int remaining = total - number;
    if (number > 0 && number % 5 == 0 && remaining > 0 && haltRollingDisplayOfChannels(remaining))
    {
      break;
    }
  }
}

void UI::downloadPodcast(PodcastChannel* channel, int podcastIndex)
{
  while (true)
  {
    try
    {
      cout << "Getting MP3 file";
      controller->downloadPodcast(channel, podcastIndex);
      return;
    }
    catch (exception& e)
    {
      if (retryDownloadAfterException(e))
      {
        continue;
      }

      return;
    }
  }
}

bool UI::retryDownloadAfterException(exception& e)
{
  char input;
  cout << endl << "EXCEPTION OCCURRED DURING DOWNLOAD: " << e.what() << endl;
  cout << "[R]etry or [C]ancel" << endl;
  cin >> input;

  return (tolower(input) == 'r');
}

void UI::downloadPodcasts(PodcastChannel* channel)
{
  downloadPodcasts(channel, channel->getPodcastCount());
}

void UI::downloadPodcasts(PodcastChannel* channel, int total)
{
  for (int podcastIndex = 0; podcastIndex < total; podcastIndex++)
  {
    cout << "Getting MP3 file [" << (podcastIndex + 1) << " of " << total << "]";
    controller->downloadPodcast(channel, podcastIndex);
  }
}

string UI::getInputStringContainingWhiteSpace()
{
  string line;
  cin.ignore(); // ignore '\n' character that is in the buffer from previous cin operation.
  getline(cin, line);
  return line;
}

int UI::scanChannel(int channelIndex)
{
  string title = controller->getChannel(channelIndex)->getTitle();

  cout << "Scanning \"" + title << "\"" << endl;
  int podcastCount = controller->scanChannel(channelIndex);

  if (podcastCount == 0)
  {
    cout << "Scan completed. No change to \"" + title << "\"" << endl << endl;
    return 0;
  }

  cout << "Scan completed. " << podcastCount << " podcast(s) added to \"" << title + "\"" << endl << endl;
  return podcastCount;
}

void UI::scanChannels()
{
  vector<PodcastChannel*>& channels = controller->getChannels();
  int total = channels.size();
  for (int channelIndex = 0; channelIndex < total; channelIndex++)
  {
    int downloadCount = scanChannel(channelIndex);
    if (downloadCount > 0)
    {
      downloadPodcasts(controller->getChannel(channelIndex), downloadCount);
    }
  }
}