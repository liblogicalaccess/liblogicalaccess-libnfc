/**
 * \file libnfcreadercardadapter.cpp
 * \author Maxime C. <maxime-dev@islog.com>
 * \brief LibNFC reader/card adapter.
 */

#include "libnfcreadercardadapter.hpp"
#include "logicalaccess/bufferhelper.hpp"
#include "libnfcdatatransport.hpp"

namespace logicalaccess
{
    LibNFCReaderCardAdapter::LibNFCReaderCardAdapter()
        : ReaderCardAdapter()
    {
    }

    LibNFCReaderCardAdapter::~LibNFCReaderCardAdapter()
    {
    }

    std::vector<unsigned char> LibNFCReaderCardAdapter::adaptCommand(const std::vector<unsigned char>& command)
    {
        return command;
    }

    std::vector<unsigned char> LibNFCReaderCardAdapter::adaptAnswer(const std::vector<unsigned char>& answer)
    {
        return answer;
    }
}