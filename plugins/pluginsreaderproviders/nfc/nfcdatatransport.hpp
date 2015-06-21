/**
 * \file nfcdatatransport.hpp
 * \author Maxime C. <maxime-dev@islog.com>
 * \brief NFC transport for reader/card commands.
 */

#ifndef LOGICALACCESS_NFCDATATRANSPORT_HPP
#define LOGICALACCESS_NFCDATATRANSPORT_HPP

#include "logicalaccess/readerproviders/datatransport.hpp"
#include "nfcreaderunit.hpp"
#include <list>

namespace logicalaccess
{
#define	TRANSPORT_NFC			"NFC"

    /**
     * \brief An NFC data transport class.
     */
    class LIBLOGICALACCESS_API NFCDataTransport : public DataTransport
    {
    public:

        /**
         * \brief Constructor.
         */
        NFCDataTransport();

        /**
         * \brief Destructor.
         */
        virtual ~NFCDataTransport();

        /**
         * \brief Get the transport type of this instance.
         * \return The transport type.
         */
        virtual std::string getTransportType() const { return TRANSPORT_NFC; };

        /**
         * \brief Get the default Xml Node name for this object.
         * \return The Xml node name.
         */
        virtual std::string getDefaultXmlNodeName() const;

		/**
		* \brief Serialize object to Xml Node.
		* \param parentNode The parent XML node.
		*/
		virtual void serialize(boost::property_tree::ptree& parentNode);

		/**
		* \brief UnSerialize object from a Xml Node.
		* \param node The Xml node.
		*/
		virtual void unSerialize(boost::property_tree::ptree& node);

		/**
		* \brief Connect to the transport layer.
		* \return True on success, false otherwise.
		*/
		virtual bool connect();

		/**
		* \brief Disconnect from the transport layer.
		*/
		virtual void disconnect();

		/**
		* \briaf Get if connected to the transport layer.
		* \return True if connected, false otherwise.
		*/
		virtual bool isConnected();

		/**
		* \brief Get the NFC reader unit.
		* \return The NFC reader unit.
		*/
		std::shared_ptr<NFCReaderUnit> getNFCReaderUnit() const { return std::dynamic_pointer_cast<NFCReaderUnit>(getReaderUnit()); };

		/**
		* \brief Get the data transport endpoint name.
		* \return The data transport endpoint name.
		*/
		virtual std::string getName() const;

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

		/**
		* \brief Check the NFC error and throw exception if needed.
		* \param errorFlag The error flag.
		*/
		static void CheckNFCError(int errorFlag);

    protected:
		
		bool d_isConnected;

		std::vector<unsigned char> d_response;
    };
}

#endif /* LOGICALACCESS_NFCDATATRANSPORT_HPP */