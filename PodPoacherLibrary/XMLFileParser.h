#pragma once

#include "stdafx.h"
#include "Poco/SAX/ContentHandler.h"
#include "Poco/SAX/LexicalHandler.h"

class XMLFileParser
{
private:
  Poco::XML::ContentHandler* contentHandler;
  Poco::XML::LexicalHandler* lexicalHandler;

public:
  XMLFileParser(Poco::XML::ContentHandler* contentHander, Poco::XML::LexicalHandler* lexicalHandler = 0);
  ~XMLFileParser();
  void ParseFile(std::string filePath);
};

