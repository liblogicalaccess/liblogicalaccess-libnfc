/**
 * \file nfcdatatransport.cpp
 * \author Maxime C. <maxime-dev@islog.com>
 * \brief NFC data transport.
 */

#include "nfcdatatransport.hpp"
#include "logicalaccess/cards/readercardadapter.hpp"
#include "logicalaccess/bufferhelper.hpp"
#include "logicalaccess/logs.hpp"
#include "logicalaccess/myexception.hpp"
#include "nfcreaderunit.hpp"

#include <boost/foreach.hpp>
#include <boost/optional.hpp>
#include <boost/array.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>
#include <boost/property_tree/ptree.hpp>
#include <ctime>

namespace logicalaccess
{
    NFCDataTransport::NFCDataTransport()
        : DataTransport()
    {
    }

    NFCDataTransport::~NFCDataTransport()
    {
    }

	bool NFCDataTransport::connect()
	{
		d_isConnected = true;
		return true;
	}

	void NFCDataTransport::disconnect()
	{
		d_isConnected = false;
	}

	bool NFCDataTransport::isConnected()
	{
		return d_isConnected;
	}

	std::string NFCDataTransport::getName() const
	{
		return getNFCReaderUnit()->getName();
	}

    void NFCDataTransport::send(const std::vector<unsigned char>& data)
    {
		d_response.clear();

		EXCEPTION_ASSERT_WITH_LOG(getNFCReaderUnit(), LibLogicalAccessException, "The NFC reader unit object"
			"is null. We cannot send.");

		if (data.size() > 0)
		{
			unsigned char returnedData[255];
			memset(returnedData, 0x00, sizeof(returnedData));
			LOG(LogLevel::COMS) << "APDU command: " << BufferHelper::getHex(data);

#ifndef _WIN64
			int res = nfc_initiator_transceive_bytes(getNFCReaderUnit()->getDevice(), &data[0], data.size(), returnedData, sizeof(returnedData), 0);
            LOG(DEBUGS) << "Received " << res << " bytes from the NFC reader.";
			if (res >= 0)
			{
				d_response = std::vector<unsigned char>(returnedData, returnedData + res);
			}
			else
				CheckNFCError(res);
#endif
		}
    }

	void NFCDataTransport::CheckNFCError(int errorFlag)
	{
		if (errorFlag < 0)
		{
			char conv[64];
			std::string msg = std::string("NFC error : ");
			sprintf(conv, "%d", errorFlag);
			msg += std::string(conv);
			msg += std::string(". ");

#ifndef _WIN64
			switch (errorFlag)
			{
				case NFC_EIO:
					msg += std::string("Input / output error, device may not be usable anymore without re-open it.");
					break;
				case NFC_EINVARG:
					msg += std::string("Invalid argument(s).");
					break;
				case NFC_EDEVNOTSUPP:
					msg += std::string("Operation not supported by device.");
					break;
				case NFC_ENOTSUCHDEV:
					msg += std::string("No such device.");
					break;
				case NFC_EOVFLOW:
					msg += std::string("Buffer overflow.");
					break;
				case NFC_ETIMEOUT:
					msg += std::string("Operation timed out.");
					break;
				case NFC_EOPABORTED:
					msg += std::string("Operation aborted (by user).");
					break;
				case NFC_ENOTIMPL:
					msg += std::string("Not (yet) implemented.");
					break;
				case NFC_ETGRELEASED:
					msg += std::string("Target released.");
					break;
				case NFC_ERFTRANS:
					msg += std::string("Error while RF transmission.");
					break;
				case NFC_EMFCAUTHFAIL:
					msg += std::string("MIFARE Classic: authentication failed.");
					break;
				case NFC_ESOFT:
					msg += std::string("Software error (allocation, file/pipe creation, etc.).");
					break;
				case NFC_ECHIP:
					msg += std::string("Device's internal chip error.");
					break;
			}
#endif

			THROW_EXCEPTION_WITH_LOG(CardException, msg);
		}
	}

    std::vector<unsigned char> NFCDataTransport::receive(long int timeout)
    {
		std::vector<unsigned char> r = d_response;
		LOG(LogLevel::COMS) << "APDU response: " << BufferHelper::getHex(r);

		d_response.clear();
		return r;
    }

    std::string NFCDataTransport::getDefaultXmlNodeName() const
    {
        return "NFCDataTransport";
    }

	void NFCDataTransport::serialize(boost::property_tree::ptree& parentNode)
	{
		boost::property_tree::ptree node;

		node.put("<xmlattr>.type", getTransportType());
		parentNode.add_child(getDefaultXmlNodeName(), node);
	}

	void NFCDataTransport::unSerialize(boost::property_tree::ptree& node)
	{

	}

    std::vector<unsigned char> NFCDataTransport::sendCommand(const std::vector<unsigned char>& command, long int timeout)
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