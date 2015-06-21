/**
 * \file libnfcdatatransport.hpp
 * \author Maxime C. <maxime-dev@islog.com>
 * \brief LibNFC transport for reader/card commands.
 */

#ifndef LOGICALACCESS_LIBNFCDATATRANSPORT_HPP
#define LOGICALACCESS_LIBNFCDATATRANSPORT_HPP

#include "logicalaccess/readerproviders/datatransport.hpp"
#include <list>

namespace logicalaccess
{
#define	TRANSPORT_LIBNFC			"LibNFC"

    /**
     * \brief An LibNFC data transport class.
     */
    class LIBLOGICALACCESS_API LibNFCDataTransport : public TcpDataTransport
    {
    public:

        /**
         * \brief Constructor.
         */
        LibNFCDataTransport();

        /**
         * \brief Destructor.
         */
        virtual ~LibNFCDataTransport();

        /**
         * \brief Get the transport type of this instance.
         * \return The transport type.
         */
        virtual std::string getTransportType() const { return TRANSPORT_LIBNFC; };

        /**
         * \brief Get the default Xml Node name for this object.
         * \return The Xml node name.
         */
        virtual std::string getDefaultXmlNodeName() const;

        /**
         * \brief Send the data using rpleth protocol computation.
         * \param data The data to send.
         */
        virtual void send(const std::vector<unsigned char>& data);

        /**
         * \brief Receive data from reader.
         * \param timeout The time to wait data.
         * \return The data from reader.
         */
        virtual std::vector<unsigned char> receive(long int timeout = 5000);

        /**
         * \brief Send a command to the reader.
         * \param command The command buffer.
         * \param timeout The command timeout.
         * \return the result of the command.
         */
        virtual std::vector<unsigned char> sendCommand(const std::vector<unsigned char>& command, long int timeout = 2000);

    protected:
		
    };
}

#endif /* LOGICALACCESS_LIBNFCDATATRANSPORT_HPP */