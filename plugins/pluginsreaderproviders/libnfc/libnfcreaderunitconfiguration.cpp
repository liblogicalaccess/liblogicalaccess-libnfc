/**
 * \file libnfcreaderunitconfiguration.cpp
 * \author Maxime C. <maxime-dev@islog.com>
 * \brief  LibNFC reader unit configuration.
 */

#include "libnfcreaderunitconfiguration.hpp"
#include "libnfcreaderprovider.hpp"

namespace logicalaccess
{
    LibNFCReaderUnitConfiguration::LibNFCReaderUnitConfiguration()
        : ReaderUnitConfiguration(READER_LIBNFC)
    {
        resetConfiguration();
    }

    LibNFCReaderUnitConfiguration::~LibNFCReaderUnitConfiguration()
    {
    }

    void LibNFCReaderUnitConfiguration::resetConfiguration()
    {
        
    }

    void LibNFCReaderUnitConfiguration::serialize(boost::property_tree::ptree& parentNode)
    {
        boost::property_tree::ptree node;
        parentNode.add_child(getDefaultXmlNodeName(), node);
    }

    void LibNFCReaderUnitConfiguration::unSerialize(boost::property_tree::ptree& node)
    {
        
    }

    std::string LibNFCReaderUnitConfiguration::getDefaultXmlNodeName() const
    {
        return "LibNFCReaderUnitConfiguration";
    }
}