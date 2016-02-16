#pragma once

#include "stdafx.h"
#include <string>

#define DLL __declspec(dllexport)
typedef void(__stdcall * FileProgressCallback)(long filePosition);

class HTTPFileDownload
{
public:
  static DLL void downloadTextFile(std::string url, std::string filePath);

  static DLL void downloadBinaryFile(std::string url, std::string filePath, FileProgressCallback fileProgressCallback, int bufferSize = 512);
};
