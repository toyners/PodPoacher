#pragma once

#include "stdafx.h"
#include <string>

typedef void(*FileProgressCallback)(long);

class HTTPFileDownload
{
public:
  static void setProgressCallbacks(FileProgressCallback fileProgressCallback);

  static void downloadTextFile(std::string url, std::string filePath);

  static void downloadBinaryFile(std::string url, std::string filePath, int bufferSize = 512);
};
