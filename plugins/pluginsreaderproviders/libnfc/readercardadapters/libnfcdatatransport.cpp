/**
 * \file libnfcdatatransport.cpp
 * \author Maxime C. <maxime-dev@islog.com>
 * \brief LibNFC data transport.
 */

#include "libnfcdatatransport.hpp"
#include "logicalaccess/cards/readercardadapter.hpp"
#include "logicalaccess/bufferhelper.hpp"
#include "libnfcreaderunit.hpp"

#include <boost/foreach.hpp>
#include <boost/optional.hpp>
#include <boost/array.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>
#include <ctime>

namespace logicalaccess
{
    LibNFCDataTransport::LibNFCDataTransport()
        : DataTransport()
    {
    }

    LibNFCDataTransport::~LibNFCDataTransport()
    {
    }

    void LibNFCDataTransport::send(const std::vector<unsigned char>& data)
    {
        
    }

    std::vector<unsigned char> LibNFCDataTransport::receive(long int timeout)
    {
        std::vector<unsigned char> ret;
        return ret;
    }

    std::string LibNFCDataTransport::getDefaultXmlNodeName() const
    {
        return "LibNFCDataTransport";
    }

    std::vector<unsigned char> LibNFCDataTransport::sendCommand(const std::vector<unsigned char>& command, long int timeout)
    {
        LOG(LogLevel::COMS) << "Sending command " << BufferHelper::getHex(command) << " command size {" << command.size() << "} timeout {" << timeout << "}...";

        std::vector<unsigned char> res;
        d_lastCommand = command;
        d_lastResult.clear();

        if (command.size() > 0)
            send(command);

        res = receive(timeout);
        d_lastResult = res;
        LOG(LogLevel::COMS) << "Response received successfully ! Reponse: " << BufferHelper::getHex(res) << " size {" << res.size() << "}";

        return res;
    }
}