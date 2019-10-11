/**
 * \file nfcreaderunitconfiguration.cpp
 * \author Maxime C. <maxime-dev@islog.com>
 * \brief  NFC reader unit configuration.
 */

#include <boost/property_tree/ptree.hpp>
#include <logicalaccess/plugins/readers/nfc/nfcreaderprovider.hpp>
#include <logicalaccess/plugins/readers/nfc/nfcreaderunitconfiguration.hpp>

namespace logicalaccess
{
NFCReaderUnitConfiguration::NFCReaderUnitConfiguration()
    : ReaderUnitConfiguration(READER_NFC)
{
    NFCReaderUnitConfiguration::resetConfiguration();
}

NFCReaderUnitConfiguration::~NFCReaderUnitConfiguration()
{
}

void NFCReaderUnitConfiguration::resetConfiguration()
{
}

void NFCReaderUnitConfiguration::serialize(boost::property_tree::ptree &parentNode)
{
    boost::property_tree::ptree node;
    parentNode.add_child(getDefaultXmlNodeName(), node);
}

void NFCReaderUnitConfiguration::unSerialize(boost::property_tree::ptree & /*node*/)
{
}

std::string NFCReaderUnitConfiguration::getDefaultXmlNodeName() const
{
    return "NFCReaderUnitConfiguration";
}
}
