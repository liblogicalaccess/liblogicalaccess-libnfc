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
    : ignore_error_(false)
{
    d_dataTransport.reset(new NFCDataTransport());
}

NFCReaderCardAdapter::~NFCReaderCardAdapter()
{
}

std::vector<unsigned char>
NFCReaderCardAdapter::adaptCommand(const std::vector<unsigned char> &command)
{
    return command;
}

std::vector<unsigned char>
NFCReaderCardAdapter::adaptAnswer(const std::vector<unsigned char> &answer)
{
    return answer;
}

std::vector<unsigned char>
NFCReaderCardAdapter::sendCommand(const std::vector<unsigned char> &command, long timeout)
{
    std::vector<unsigned char> res;

    if (d_dataTransport)
    {
        res = adaptAnswer(d_dataTransport->sendCommand(adaptCommand(command), timeout));
        if (ignore_error_)
            return res;

        if (res.size() > 0 && getResultChecker())
        {
            LOG(LogLevel::DEBUGS) << "Call ResultChecker..." << BufferHelper::getHex(res);
            getResultChecker()->CheckResult(&res[0], res.size());
        }
    }
    else
    {
        LOG(LogLevel::ERRORS)
            << "Cannot transmit the command, data transport is not set!";
    }
    return res;
}

bool NFCReaderCardAdapter::ignoreAllError(bool ignore)
{
    bool tmp      = ignore_error_;
    ignore_error_ = ignore;
    return tmp;
}

bool NFCReaderCardAdapter::ignoreAllError() const
{
    return ignore_error_;
}
}