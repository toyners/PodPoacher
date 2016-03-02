#pragma once

#include <string>
#include "PodcastChannel.h"

void downloadRSSFile(std::string url, std::string rssFilePath);
void parseRSSFile(std::string url, std::string rssFilePath, PodcastChannel* channel);
void fileProgress(long position);
void setupProgress(long fileSize);
std::string getWorkingDirectory();
long getFileSize(std::string filePath);

static long tickSize;
static long tickCount;
