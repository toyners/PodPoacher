#include "stdafx.h"
#include "UI.h"
#include "PodcastStorage.h"
#include "PodcastChannel.h"
#include "Utility.h"
#include "Poco/Net/NetException.h"

#include <iostream>
#include <vector>

UI::UI(BaseController& baseController)
{
  controller = &baseController;
}

UI::~UI() {}

void UI::topLevelUI()
{
  std::cout << "PodPoacher v0.94" << std::endl << std::endl;

  while (true)
  {
    char input;
    std::cout << "[A]dd channel, [D]isplay channels, [S]can channels or e[X]it" << std::endl;
    std::cin >> input;

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
  std::string input;
  int channelCount = controller->getChannelCount();

  if (channelCount == 0)
  {
    std::cout << "No channels to display." << std::endl;
    return;
  }

  while (true)
  {
    std::cout << "Display [A]ll channels, numbers [1 - " << channelCount << "] or [B]ack" << std::endl;
    std::cin >> input;

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
      std::vector<PodcastChannel*>& channels = controller->getChannels();
      displayChannel(number, *channels[number - 1]);
    }
  }
}

void UI::scanChannelsUI()
{
  std::string input;
  int channelCount = controller->getChannelCount();

  if (channelCount == 0)
  {
    std::cout << "No channels to scan." << std::endl;
    return;
  }

  while (true)
  {
    std::cout << "Scan [A]ll channels, numbers [1 - " << channelCount << "] or [B]ack" << std::endl;
    std::cin >> input;

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
  std::string url;
  std::cout << "Enter feed URL: ";
  std::cin >> url;

  std::string directory;
  std::cout << "Enter directory for podcasts: ";
  directory = getInputStringContainingWhiteSpace();

  controller->addChannel(url, directory);
  int channelIndex = controller->getChannelCount() - 1;
  PodcastChannel* channel = controller->getChannel(channelIndex);
  
  int podcastCount = channel->getPodcastCount();
  std::cout << podcastCount << " Podcasts loaded." << std::endl << std::endl;

  // Display podcasts, download all or download some
  while (true)
  {
    std::string input;
    std::cout << "[D]isplay podcasts, Download [A]ll podcasts, Download individual podcasts [1 - " << podcastCount << "] or [B]ack" << std::endl;
    std::cin >> input;
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
      downloadPodcast(channel, number - 1);
      continue;
    }
  }
}

bool UI::tryConvertInputToNumber(std::string input, int& number, int podcastCount)
{
  try
  {
    number = stoi(input);

    if (number <= 0)
    {
      std::cout << "'" << input << "' is negative! Range is [1 - " << podcastCount << "]" << std::endl;
      return false;
    }

    if (number > podcastCount)
    {
      std::cout << "'" << input << "' is out of range! Range is [1 - " << podcastCount << "]" << std::endl;
      return false;
    }

    return true;
  }
  catch (std::invalid_argument& i)
  {
    std::cout << "'" << input << "' is not a valid option! Options are [A], [B] or [1 - " << podcastCount << "]" << std::endl;
  }
  catch (std::out_of_range& o)
  {
    std::cout << "'" << input << "' is out of range! Range is [1 - " << podcastCount << "]" << std::endl;
  }

  return false;
}

bool UI::haltRollingDisplayOfPodcasts(PodcastChannel* channel, int remaining)
{
  int total = channel->getPodcastCount();
  while (true)
  {
    std::string input;
    std::cout << remaining << " Podcasts to go. [D]own, Download [A]ll podcasts, Download individual podcasts [1 - " << total << "] or [B]reak" << std::endl;
    std::cin >> input;

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
      downloadPodcast(channel, number - 1);
      continue;
    }
  }
}

bool UI::haltRollingDisplayOfChannels(int remaining)
{
  std::string input;
  std::cout << remaining << " Channels to go. [D]own or [B]reak" << std::endl;
  std::cin >> input;

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
    std::string label = "<" + std::to_string(number) + "> ";
    std::string indent(label.size(), ' ');
    PodcastDetails* podcast = channel.getPodcast(index);
    std::cout << label << "TITLE: " << podcast->getTitle() << std::endl
      << indent << "PUB DATE: " << podcast->getPublishedDate() 
      << "  SIZE: " << podcast->getFileSize();

    if (podcast->isDownloaded())
    {
      std::cout << std::endl << label << "DOWNLOAD DATE: " << podcast->getDownloadDate();
    }

    std::cout << std::endl << std::endl;

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
    std::string input;
    std::cout <<  "Display [A]ll podcasts. Download individual podcast [1 - " << podcastCount << "] or [B]ack" << std::endl;
    std::cin >> input;

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
      downloadPodcast(&channel, number - 1);
    }
  }
}

void UI::displayChannelDetails(int number, PodcastChannel& channel)
{
  std::string label = "[" + std::to_string(number) + "] ";
  std::string indent(label.size(), ' ');
  std::cout << label << channel.getTitle() << std::endl;
  std::cout << indent << channel.getDirectory() << std::endl;
  std::cout << indent << "PODCASTS: " << channel.getPodcastCount() << "  PUBLISHED DATE: " << channel.getPublishedDate() << std::endl;
  std::cout << std::endl;
}

void UI::displayChannels()
{
  std::vector<PodcastChannel*>& channels = controller->getChannels();
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
      std::cout << "Getting MP3 file";
      controller->downloadPodcast(channel, podcastIndex);
      return;
    }
    catch (Poco::Net::HTTPException& h)
    {
      if (retryDownloadAfterException(h.displayText()))
      {
        continue;
      }

      return;
    }
    catch (std::exception& e)
    {
      if (retryDownloadAfterException(std::string(e.what())))
      {
        continue;
      }

      return;
    }
  }
}

bool UI::retryDownloadAfterException(std::string& message)
{
  char input;
  std::cout << std::endl << "EXCEPTION OCCURRED DURING DOWNLOAD: " << message << std::endl;
  std::cout << "[R]etry or [C]ancel" << std::endl;
  std::cin >> input;

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
    std::cout << "Getting MP3 file [" << (podcastIndex + 1) << " of " << total << "]";
    controller->downloadPodcast(channel, podcastIndex);
  }
}

std::string UI::getInputStringContainingWhiteSpace()
{
  std::string line;
  std::cin.ignore(); // ignore '\n' character that is in the buffer from previous cin operation.
  std::getline(std::cin, line);
  return line;
}

int UI::scanChannel(int channelIndex)
{
  std::string title = controller->getChannel(channelIndex)->getTitle();

  std::cout << "Scanning \"" + title << "\"" << std::endl;
  int podcastCount = controller->scanChannel(channelIndex);

  if (podcastCount == 0)
  {
    std::cout << "Scan completed. No change to \"" + title << "\"" << std::endl << std::endl;
    return 0;
  }

  std::cout << "Scan completed. " << podcastCount << " podcast(s) added to \"" << title + "\"" << std::endl << std::endl;
  return podcastCount;
}

void UI::scanChannels()
{
  std::vector<PodcastChannel*>& channels = controller->getChannels();
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