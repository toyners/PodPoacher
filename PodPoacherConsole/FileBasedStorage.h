#pragma once

#include "PodcastStorage.h"
#include <string>
#include <vector>

class FileBasedStorage : public PodcastStorage
{
public:
  FileBasedStorage(std::string storagePath);
  ~FileBasedStorage();

  void AddChannel(PodcastChannel& channel);
  std::vector<PodcastChannel*>& GetChannels();
  void LoadChannel(PodcastChannel& channel);
  void UpdateChannel(PodcastChannel& channel);

private:
  std::string getChannelFileName(PodcastChannel& channel);
  void getTokensFromLine(const std::string& line, std::vector<std::string>& tokens);
};

