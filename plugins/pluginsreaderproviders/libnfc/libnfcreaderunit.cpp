/**
 * \file libnfcreaderunit.cpp
 * \author Maxime C. <maxime-dev@islog.com>
 * \brief LibNFC reader unit.
 */

#include "libnfcreaderunit.hpp"

#include <iostream>
#include <iomanip>
#include <sstream>

#include "logicalaccess/dynlibrary/librarymanager.hpp"
#include "logicalaccess/dynlibrary/idynlibrary.hpp"

#include "logicalaccess/bufferhelper.hpp"
#include "libnfcreaderprovider.hpp"
#include "logicalaccess/cards/chip.hpp"
#include <boost/filesystem.hpp>
#include "libnfcdatatransport.hpp"

namespace logicalaccess
{
    LibNFCReaderUnit::LibNFCReaderUnit()
        : ReaderUnit()
    {
        d_readerUnitConfig.reset(new LibNFCReaderUnitConfiguration());
        setDefaultReaderCardAdapter(std::shared_ptr<LibNFCReaderCardAdapter>(new LibNFCReaderCardAdapter()));

        std::shared_ptr<LibNFCDataTransport> dataTransport(new LibNFCDataTransport());
        setDataTransport(dataTransport);
        d_card_type = "UNKNOWN";

        try
        {
            boost::property_tree::ptree pt;
            read_xml((boost::filesystem::current_path().string() + "/LibNFCReaderUnit.config"), pt);
            d_card_type = pt.get("config.cardType", "UNKNOWN");
        }
        catch (...) {}
    }

    LibNFCReaderUnit::~LibNFCReaderUnit()
    {
        disconnectFromReader();
    }

    std::string LibNFCReaderUnit::getName() const
    {
        string ret = "";
        return ret;
    }

    std::string LibNFCReaderUnit::getConnectedName()
    {
        return getName();
    }

    void LibNFCReaderUnit::setCardType(std::string cardType)
    {
        d_card_type = cardType;
    }

    bool LibNFCReaderUnit::waitInsertion(unsigned int maxwait)
    {
        bool inserted = false;

        return inserted;
    }

    bool LibNFCReaderUnit::waitRemoval(unsigned int maxwait)
    {
        bool removed = false;

        return removed;
    }

    bool LibNFCReaderUnit::connect()
    {
        return true;
    }

    void LibNFCReaderUnit::disconnect()
    {

        LOG(LogLevel::INFOS) << "Disconnected from the chip";
    }

    std::shared_ptr<Chip> LibNFCReaderUnit::createChip(std::string type)
    {
        LOG(LogLevel::INFOS) << "Create chip " << type;
        std::shared_ptr<Chip> chip = ReaderUnit::createChip(type);

        return chip;
    }

    std::shared_ptr<Chip> LibNFCReaderUnit::getSingleChip()
    {
        std::shared_ptr<Chip> chip = d_insertedChip;
        return chip;
    }

    std::vector<std::shared_ptr<Chip> > LibNFCReaderUnit::getChipList()
    {
        std::vector<std::shared_ptr<Chip> > chipList;
        std::shared_ptr<Chip> singleChip = getSingleChip();
        if (singleChip)
        {
            chipList.push_back(singleChip);
        }
        return chipList;
    }

    std::shared_ptr<LibNFCReaderCardAdapter> LibNFCReaderUnit::getDefaultLibNFCReaderCardAdapter()
    {
        std::shared_ptr<ReaderCardAdapter> adapter = getDefaultReaderCardAdapter();
        if (adapter)
        {
            if (!adapter->getDataTransport())
            {
                adapter->setDataTransport(getDataTransport());
            }
        }
        return std::dynamic_pointer_cast<LibNFCReaderCardAdapter>(adapter);
    }

    string LibNFCReaderUnit::getReaderSerialNumber()
    {
        return "";
    }

    bool LibNFCReaderUnit::isConnected()
    {
        return bool(d_insertedChip);
    }

    bool LibNFCReaderUnit::connectToReader()
    {
		return true;
    }

    void LibNFCReaderUnit::disconnectFromReader()
    {
        
    }

    void LibNFCReaderUnit::serialize(boost::property_tree::ptree& parentNode)
    {
        boost::property_tree::ptree node;
        ReaderUnit::serialize(node);
        parentNode.add_child(getDefaultXmlNodeName(), node);
    }

    void LibNFCReaderUnit::unSerialize(boost::property_tree::ptree& node)
    {
        ReaderUnit::unSerialize(node);
    }

    std::shared_ptr<LibNFCReaderProvider> LibNFCReaderUnit::getLibNFCReaderProvider() const
    {
        return std::dynamic_pointer_cast<LibNFCReaderProvider>(getReaderProvider());
    }
}