#pragma once

#include "PodcastStorage.h"
#include <string>
#include <vector>

class FileBasedStorage : public PodcastStorage
{
private:
  std::string storagePath;
  std::string channelPath;
  std::vector<PodcastChannel*> channelList;

public:
  FileBasedStorage(std::string storagePath);
  ~FileBasedStorage();

  void addChannel(PodcastChannel& channel);
  std::vector<PodcastChannel*>& getChannels();
  void loadChannel(PodcastChannel& channel);
  void removeChannel(PodcastChannel& channel);
  void updateChannel(PodcastChannel& oldChannel, PodcastChannel& newChannel);

private:
  std::string getChannelFileName(PodcastChannel& channel);
  void getTokensFromLine(const std::string& line, std::vector<std::string>& tokens);
  void deserialiseChannelsComplete();
  void serialiseChannelsOnly();
  void serialisePodcasts(const std::string& fileName, PodcastChannel& channel);
  void deserialisePodcasts(const std::string& fileName, PodcastChannel& channel);
  std::string createBackup(const std::string& filePath);
};

