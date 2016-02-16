#include "stdafx.h"
#include "RSSContentHandler.h"
#include "Poco/SAX/Attributes.h"

using Poco::XML::Locator;
using Poco::XML::Attributes;
using Poco::XML::XMLChar;
using Poco::XML::XMLString;

using namespace std;

RSSContentHandler::RSSContentHandler()
{
  channel = new PodcastChannel();
}

RSSContentHandler::~RSSContentHandler()
{
  delete channel;
}

void RSSContentHandler::startElement(const XMLString& uri, const XMLString& localName, const XMLString& qname, const Attributes& attributes)
{
  if (!inItems)
  {
    if (localName == "title")
    {
      inChannelTitleEntity = true;
      return;
    }

    if (localName == "description")
    {
      inChannelDescriptionEntity = true;
      return;
    }

    if (localName == "link")
    {
      inChannelWebsiteEntity = true;
      return;
    }
  }

  if (localName == "item")
  {
    inItems = true;
    return;
  }

  if (!inItems)
  {
    return;
  }
  
  if (localName == "title" && podcastTitle == "")
  {
    inPodcastTitleEntity = true;
    return;
  }

  if (localName == "description" && podcastDescription == "")
  {
    inPodcastDescriptionEntity = true;
    return;
  }

  if (localName == "pubDate" && podcastPubDate == "")
  {
    inPodcastPubDateEntity = true;
    return;
  }

  if (localName == "enclosure")
  {
    for (int i = 0; i < attributes.getLength(); ++i)
    {
      if (attributes.getLocalName(i) == "url")
      {
        podcastURL = attributes.getValue(i);
      }
      else if (attributes.getLocalName(i) == "length")
      {
        string value = attributes.getValue(i);
        if (value == "")
        {
          podcastSize = 0;
        }
        else
        {
          podcastSize = std::stol(value);
        }
      }
    }
  }
}

void RSSContentHandler::endElement(const XMLString& uri, const XMLString& localName, const XMLString& qname)
{
  if (!inItems)
  {
    return;
  }

  if (localName == "item")
  {
    channel->addPodcastDetails(podcastTitle, podcastDescription, podcastPubDate, podcastURL, podcastSize);
    podcastTitle = podcastDescription = podcastPubDate = podcastURL = "";
    podcastSize = 0;
  }
}

void RSSContentHandler::characters(const XMLChar ch[], int start, int length)
{
  if (inChannelTitleEntity)
  {
    channel->setTitle(string(&ch[start], length));
    inChannelTitleEntity = false;
    return;
  }

  if (inChannelDescriptionEntity)
  {
    channel->setDescription(string(&ch[start], length));
    inChannelDescriptionEntity = false;
    return;
  }

  if (inChannelWebsiteEntity)
  {
    channel->setWebsite(string(&ch[start], length));
    inChannelWebsiteEntity = false;
    return;
  }

  if (inPodcastTitleEntity)
  {
    podcastTitle = string(&ch[start], length);
    inPodcastTitleEntity = false;
    return;
  }

  if (inPodcastDescriptionEntity)
  {
    podcastDescription = string(&ch[start], length);
    inPodcastDescriptionEntity = false;
    return;
  }

  if (inPodcastPubDateEntity)
  {
    podcastPubDate = string(&ch[start], length);
    inPodcastPubDateEntity = false;
    return;
  }
}

PodcastChannel * RSSContentHandler::getChannel()
{
  return channel;
}


void RSSContentHandler::setDocumentLocator(const Locator * loc) {}

void RSSContentHandler::startDocument() {}

void RSSContentHandler::endDocument() {}

void RSSContentHandler::ignorableWhitespace(const XMLChar ch[], int start, int length) {}

void RSSContentHandler::processingInstruction(const XMLString& target, const XMLString& data) {}

void RSSContentHandler::startPrefixMapping(const XMLString& prefix, const XMLString& uri) {}

void RSSContentHandler::endPrefixMapping(const XMLString& prefix) {}

void RSSContentHandler::skippedEntity(const XMLString& name) {}
