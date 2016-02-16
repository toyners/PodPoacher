#include "stdafx.h"
#include "XMLFileParser.h"
#include "Poco/SAX/ContentHandler.h"
#include "Poco/SAX/LexicalHandler.h"
#include "Poco/SAX/SAXParser.h"

using Poco::XML::ContentHandler;
using Poco::XML::LexicalHandler;
using Poco::XML::SAXParser;
using Poco::XML::XMLReader;

using namespace std;

XMLFileParser::XMLFileParser(ContentHandler* contentHander, LexicalHandler* lexicalHandler)
{
  XMLFileParser::contentHandler = contentHander;
  XMLFileParser::lexicalHandler = lexicalHandler;
}

XMLFileParser::~XMLFileParser() {}

void XMLFileParser::ParseFile(string filePath)
{
  SAXParser parser;
  parser.setFeature(XMLReader::FEATURE_NAMESPACES, true);
  parser.setFeature(XMLReader::FEATURE_NAMESPACE_PREFIXES, true);

  if (XMLFileParser::lexicalHandler != 0)
  {
    parser.setProperty(XMLReader::PROPERTY_LEXICAL_HANDLER, XMLFileParser::lexicalHandler);
  }

  parser.setContentHandler(contentHandler);

  parser.parse(filePath);
}
