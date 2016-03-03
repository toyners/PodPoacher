#include "stdafx.h"
#include "HTTPFileDownload.h"
#include "Poco/Net/HTTPClientSession.h"
#include "Poco/Net/HTTPRequest.h"
#include "Poco/Net/HTTPResponse.h"
#include "Poco/Net/HTTPStreamFactory.h"
#include "Poco/URI.h"
#include "Poco/URIStreamOpener.h"
#include "Poco/StreamCopier.h"
#include <iostream>
#include <fstream>

using Poco::Net::HTTPClientSession;
using Poco::Net::HTTPRequest;
using Poco::Net::HTTPResponse;
using Poco::Net::HTTPMessage;
using Poco::URI;
using Poco::StreamCopier;
using Poco::Net::HTTPStreamFactory;
using Poco::URIStreamOpener;

using namespace std;

void HTTPFileDownload::downloadTextFile(string url, string filePath)
{
  URI uri(url);

  string path(uri.getPathAndQuery());
  if (path.empty())
  {
    path = "/";
  }

  HTTPClientSession session(uri.getHost(), uri.getPort());
  HTTPRequest request(HTTPRequest::HTTP_GET, path, HTTPMessage::HTTP_1_1);
  HTTPResponse response;

  session.sendRequest(request);
  istream& rs = session.receiveResponse(response);

  if (response.getStatus() != Poco::Net::HTTPResponse::HTTP_OK)
  {
    string message = "Can't get file from '" + url + "'. Status is " + std::to_string(response.getStatus()) + ". Reason is '" + response.getReason() + "'.";
    throw new exception(message.data());
  }

  ofstream file;
  file.open(filePath, ios::out | ios::trunc);
  StreamCopier::copyStream(rs, file);
  file.close();
}

void HTTPFileDownload::downloadBinaryFile(string url, string filePath, FileProgressCallback progressCallback, int bufferSize)
{
  HTTPStreamFactory::registerFactory();

  URI uri(url);
  istream* rs = URIStreamOpener::defaultOpener().open(uri);

  ofstream file;
  file.open(filePath, ios::out | ios::trunc | ios::binary);

  char* buffer = new char[bufferSize];
  long writeCount = 0;
  while (!rs->eof())
  {
    rs->read(buffer, bufferSize);
    streamsize readCount = rs->gcount();
    file.write(buffer, readCount);

    if (progressCallback != 0)
    {
      writeCount += readCount;
      progressCallback(writeCount);
    }
  }

  delete[] buffer;
  delete rs;
  file.close();

  HTTPStreamFactory::unregisterFactory();
}
