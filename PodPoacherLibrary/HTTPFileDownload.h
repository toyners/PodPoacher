#pragma once

#include "stdafx.h"
#include <string>

typedef void(*FileProgressCallback)(long);

class HTTPFileDownload
{
public:
  static void downloadTextFile(std::string url, std::string filePath);

  static void downloadSecureTextFile(std::string url, std::string filePath);

  static void downloadBinaryFile(std::string url, std::string filePath, FileProgressCallback progressCallback, int bufferSize = 512);
};
