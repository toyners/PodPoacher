#pragma once

#include "stdafx.h"
#include "Poco/SAX/ContentHandler.h"
#include "PodcastDetails.h"
#include "PodcastChannel.h"

class RSSContentHandler : public Poco::XML::ContentHandler
{
private:
  PodcastChannel* channel;

  bool inItems;
  bool inChannelTitleEntity;
  bool inChannelDescriptionEntity;
  bool inChannelWebsiteEntity;

  bool inPodcastTitleEntity;
  bool inPodcastDescriptionEntity;
  bool inPodcastPubDateEntity;
  bool inPodcastWebsiteEntity;
  std::string podcastTitle;
  std::string podcastDescription;
  std::string podcastPubDate;
  std::string podcastURL;
  long podcastSize;

public:
  RSSContentHandler(PodcastChannel* podcastChannel);
  ~RSSContentHandler();
  void setDocumentLocator(const Poco::XML::Locator * loc);
  void startDocument();
  void endDocument();
  void ignorableWhitespace(const Poco::XML::XMLChar ch[], int start, int length);
  void processingInstruction(const Poco::XML::XMLString & target, const Poco::XML::XMLString & data);
  void startPrefixMapping(const Poco::XML::XMLString & prefix, const Poco::XML::XMLString & uri);
  void endPrefixMapping(const Poco::XML::XMLString & prefix);
  void skippedEntity(const Poco::XML::XMLString & name);
  void startElement(const Poco::XML::XMLString & uri, const Poco::XML::XMLString & localName, const Poco::XML::XMLString & qname, const Poco::XML::Attributes & attributes);
  void endElement(const Poco::XML::XMLString & uri, const Poco::XML::XMLString & localName, const Poco::XML::XMLString & qname);
  void characters(const Poco::XML::XMLChar ch[], int start, int length);

  PodcastChannel* getChannel();
};