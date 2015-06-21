/**
 * \file nfcreadercardadapter.cpp
 * \author Maxime C. <maxime-dev@islog.com>
 * \brief NFC reader/card adapter.
 */

#include "nfcreadercardadapter.hpp"
#include "../nfcreaderunit.hpp"
#include "logicalaccess/bufferhelper.hpp"
#include "../nfcdatatransport.hpp"

namespace logicalaccess
{
    NFCReaderCardAdapter::NFCReaderCardAdapter()
    {
        d_dataTransport.reset(new NFCDataTransport());
    }

    NFCReaderCardAdapter::~NFCReaderCardAdapter()
    {
    }

    std::vector<unsigned char> NFCReaderCardAdapter::adaptCommand(const std::vector<unsigned char>& command)
    {
        return command;
    }

    std::vector<unsigned char> NFCReaderCardAdapter::adaptAnswer(const std::vector<unsigned char>& answer)
    {
        return answer;
    }
}