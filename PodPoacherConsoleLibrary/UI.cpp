#include "stdafx.h"
#include "UI.h"
#include "PodcastStorage.h"
#include "PodcastChannel.h"
#include "Utility.h"
#include "Poco/Net/NetException.h"
#include "IndexParser.h"

#include <iostream>
#include <vector>

UI::UI(BaseController& baseController)
{
  controller = &baseController;
  indexParser = new IndexParser();
}

UI::~UI() 
{
  delete indexParser;
}

void UI::topLevelUI()
{
  std::cout << "PodPoacher v0.963" << std::endl << std::endl;

  while (true)
  {
    char input;
    std::cout << "[A]dd channel, [D]isplay channels, [R]emove Channel, [S]can channels or e[X]it" << std::endl;
    std::cin >> input;

    input = tolower(input);
    if (input == 'x')
    {
      return;
    }

    if (input == 'a')
    {
      addChannelUI();
    }

    if (input == 'd')
    {
      displayChannelsUI();
    }

    if (input == 'r')
    {
      removeChannelsUI();
    }

    if (input == 's')
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

bool UI::addChannel()
{
  std::string url;
  std::cout << "Enter feed URL: ";
  std::cin >> url;

  std::string directory;
  std::cout << "Enter directory for podcasts: ";
  directory = getInputStringContainingWhiteSpace();

  try
  {
    controller->addChannel(url, directory);
  }
  catch (std::exception& e)
  {
    std::cout << std::endl << "EXCEPTION OCCURRED DURING CHANNEL ADD: " << e.what() << std::endl << std::endl;
    return false;
  }

  return true;
}

void UI::addChannelUI()
{
  if (!addChannel())
  {
    return;
  }

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

    handleIndexInput(input, *channel);
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

    handleIndexInput(input, *channel);
  }
}

bool UI::haltRollingDisplayOfChannels(int remaining, int total, std::vector<PodcastChannel*>& channels)
{
  std::string input;
  std::cout << remaining << " Channels to go. [D]own, Select Channel [1 - " << total << "] or [B]reak" << std::endl;
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

  int number;
  if (tryConvertInputToNumber(input, number, total))
  {
    displayChannel(number, *channels[number - 1]);
  }
}

void UI::handleIndexInput(std::string & input, PodcastChannel& channel)
{
  std::vector<int>* indicies = indexParser->parseInput(input);
  downloadPodcasts(&channel, *indicies);
  delete indicies;
}

void UI::removeChannelsUI()
{
  std::string input;
  int channelCount = controller->getChannelCount();

  while (true)
  {
    if (channelCount == 0)
    {
      std::cout << "No channels to remove." << std::endl;
      return;
    }

    std::cout << "Remove channel [1 - " << channelCount << "] or [B]ack" << std::endl;
    std::cin >> input;
    char c = tolower(input[0]);
    if (c == 'b')
    {
      break;
    }

    int number;
    if (tryConvertInputToNumber(input, number, channelCount))
    {
      controller->removeChannel(number - 1);
      channelCount = controller->getChannelCount();
    }
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
      << "  SIZE: " << getReadableFileSize(podcast->getFileSize());

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

    handleIndexInput(input, channel);
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
    if (number > 0 && number % 5 == 0 && remaining > 0 && haltRollingDisplayOfChannels(remaining, total, channels))
    {
      break;
    }
  }
}

void UI::tryDownloadPodcast(PodcastChannel* channel, int podcastIndex)
{
  while (true)
  {
    try
    {
      controller->downloadPodcast(channel, podcastIndex);

      std::cout << "DONE " << getReadableFileSize(channel->getPodcast(podcastIndex)->getFileSize()) << std::endl;
      return;
    }
    catch (Poco::Exception& p)
    {
      if (retryDownloadAfterException(p.displayText()))
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
    std::cout << "Getting MP3 file [" << (podcastIndex + 1) << " of " << total << "] ";
    tryDownloadPodcast(channel, podcastIndex);
  }

  std::cout << std::endl;
}

void UI::downloadPodcasts(PodcastChannel* channel, std::vector<int>& indexes)
{
  int total = indexes.size();
  for (int index = 0; index < total; index++)
  {
    std::cout << "Getting MP3 file [" << (index + 1) << " of " << total << "] ";
    tryDownloadPodcast(channel, indexes[index] - 1);
  }

  std::cout << std::endl;
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
  try
  {
    std::string title = controller->getChannel(channelIndex)->getTitle();

    std::cout << "Scanning \"" + title << "\"" << std::endl;
    int podcastCount = controller->scanChannel(channelIndex);

    if (podcastCount == 0)
    {
      std::cout << "Scan completed. No change to \"" + title << "\"" << std::endl << std::endl;
      return 0;
    }

    std::cout << "Scan completed. " << podcastCount << " podcast(s) added to \"" << title + "\"" << std::endl;
    return podcastCount;
  }
  catch (Poco::Exception& p)
  {
    std::cout << std::endl << "EXCEPTION OCCURRED DURING SCAN: " << p.displayText() << std::endl << std::endl;
  }
  catch (std::exception& e)
  {
    std::cout << std::endl << "EXCEPTION OCCURRED DURING SCAN: " << e.what() << std::endl << std::endl;
  }

  return 0;
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