#include "stdafx.h"
#include "HTTPFileDownload.h"
#include "Poco/Net/HTTPClientSession.h"
#include "Poco/Net/HTTPRequest.h"
#include "Poco/Net/HTTPResponse.h"
#include "Poco/Net/HTTPStreamFactory.h"
#include "Poco/Net/HTTPSClientSession.h"
#include "Poco/URI.h"
#include "Poco/URIStreamOpener.h"
#include "Poco/StreamCopier.h"
#include <iostream>
#include <fstream>
#include <memory>

#include "Poco/Net/SSLManager.h"
#include "Poco/Net/KeyConsoleHandler.h"
#include "Poco/Net/ConsoleCertificateHandler.h"
#include "Poco/Net/Context.h"

using Poco::Net::HTTPClientSession;
using Poco::Net::HTTPSClientSession;
using Poco::Net::HTTPRequest;
using Poco::Net::HTTPResponse;
using Poco::Net::HTTPMessage;
using Poco::URI;
using Poco::StreamCopier;
using Poco::Net::HTTPStreamFactory;
using Poco::URIStreamOpener;

using Poco::Net::SSLManager;
using Poco::Net::ConsoleCertificateHandler;
using Poco::Net::InvalidCertificateHandler;
using Poco::SharedPtr;
using Poco::Net::Context;

void HTTPFileDownload::downloadTextFile(std::string url, std::string filePath)
{
  URI uri(url);

  std::string path(uri.getPathAndQuery());
  if (path.empty())
  {
    path = "/";
  }

  HTTPClientSession session(uri.getHost(), uri.getPort());
  HTTPRequest request(HTTPRequest::HTTP_GET, path, HTTPMessage::HTTP_1_1);
  HTTPResponse response;

  session.sendRequest(request);
  std::istream& rs = session.receiveResponse(response);

  if (response.getStatus() != Poco::Net::HTTPResponse::HTTP_OK)
  {
    std::string message = "Can't get file from '" + url + "'. Status is " + std::to_string(response.getStatus()) + ". Reason is '" + response.getReason() + "'.";
    throw std::exception(message.data());
  }

  std::ofstream file;
  file.open(filePath, std::ios::out | std::ios::trunc);
  StreamCopier::copyStream(rs, file);
  file.close();
}

void HTTPFileDownload::downloadSecureTextFile(std::string url, std::string filePath)
{
  Poco::Net::initializeSSL();

  try
  {
    SharedPtr<InvalidCertificateHandler> ptrCert = new ConsoleCertificateHandler(false);
    Context::Ptr ptrContext = new Context(Context::CLIENT_USE, "", "", "", Context::VERIFY_RELAXED, 9, false, "ALL:!ADH:!LOW:!EXP:!MD5:@STRENGTH");
    SSLManager::instance().initializeClient(0, ptrCert, ptrContext);

    URI uri(url);

    HTTPSClientSession session(uri.getHost(), uri.getPort());
    HTTPRequest request(HTTPRequest::HTTP_GET, uri.getPath());

    session.sendRequest(request);

    HTTPResponse response;
    std::istream& rs = session.receiveResponse(response);

    if (response.getStatus() != Poco::Net::HTTPResponse::HTTP_OK)
    {
      std::string message = "Can't get file from '" + url + "'. Status is " + std::to_string(response.getStatus()) + ". Reason is '" + response.getReason() + "'.";
      throw std::exception(message.data());
    }

    std::ofstream file;
    file.open(filePath, std::ios::out | std::ios::trunc);
    StreamCopier::copyStream(rs, file);
    file.close();
  }
  catch (...)
  {
    Poco::Net::uninitializeSSL();
    throw;
  }

  Poco::Net::uninitializeSSL();
}

void HTTPFileDownload::downloadBinaryFile(std::string url, std::string filePath, FileProgressCallback progressCallback, int bufferSize)
{
  HTTPStreamFactory::registerFactory();

  std::istream* rs;
  try
  {
    URI uri(url);
    rs = URIStreamOpener::defaultOpener().open(uri);
  }
  catch (...)
  {
    HTTPStreamFactory::unregisterFactory();
    throw;
  }

  std::ofstream file;
  file.open(filePath, std::ios::out | std::ios::trunc | std::ios::binary);

  char* buffer = new char[bufferSize];
  long writeCount = 0;
  while (!rs->eof())
  {
    rs->read(buffer, bufferSize);
    std::streamsize readCount = rs->gcount();
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
