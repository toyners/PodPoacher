#pragma once

#include "PodcastStorage.h"
#include <string>
#include <vector>

class FileBasedStorage : public PodcastStorage
{
public:
  FileBasedStorage(std::string storagePath);
  ~FileBasedStorage();

  void addChannel(PodcastChannel& channel);
  std::vector<PodcastChannel*>& getChannels();
  void loadChannel(PodcastChannel& channel);
  void updateChannel(PodcastChannel& channel);

private:
  std::string getChannelFileName(PodcastChannel& channel);
  void getTokensFromLine(const std::string& line, std::vector<std::string>& tokens);
  void loadChannelHeaders();
};

