#pragma once

#include <string>
#include "PodcastChannel.h"

void downloadRSSFile(std::string url, std::string rssFilePath);
void parseRSSFile(std::string url, std::string rssFilePath, PodcastChannel* channel);
void fileProgress(long position);
void setupProgress(long fileSize);
bool tryGetFileNameFromURL(std::string& url, std::string& fileName);
std::string getSchemeFromURL(std::string& url);
std::string getWorkingDirectory();
long getFileSize(std::string filePath);
std::string removeIllegalFilePathCharactersFromText(std::string text);
std::string getReadableFileSize(long size);

static long tickSize;
static long tickCount;
