/**
 * \file nfcdatatransport.hpp
 * \author Maxime C. <maxime-dev@islog.com>
 * \brief NFC transport for reader/card commands.
 */

#ifndef LOGICALACCESS_NFCDATATRANSPORT_HPP
#define LOGICALACCESS_NFCDATATRANSPORT_HPP

#include <logicalaccess/readerproviders/datatransport.hpp>
#include <logicalaccess/plugins/readers/nfc/nfcreaderunit.hpp>
#include <list>

namespace logicalaccess
{
#define TRANSPORT_NFC "NFC"

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
    std::string getTransportType() const override
    {
        return TRANSPORT_NFC;
    }

    /**
 * \brief Get the default Xml Node name for this object.
 * \return The Xml node name.
 */
    std::string getDefaultXmlNodeName() const override;

    /**
    * \brief Serialize object to Xml Node.
    * \param parentNode The parent XML node.
    */
    void serialize(boost::property_tree::ptree &parentNode) override;

    /**
    * \brief UnSerialize object from a Xml Node.
    * \param node The Xml node.
    */
    void unSerialize(boost::property_tree::ptree &node) override;

    /**
    * \brief Connect to the transport layer.
    * \return True on success, false otherwise.
    */
    bool connect() override;

    /**
    * \brief Disconnect from the transport layer.
    */
    void disconnect() override;

    /**
    * \briaf Get if connected to the transport layer.
    * \return True if connected, false otherwise.
    */
    bool isConnected() override;

    /**
    * \brief Get the NFC reader unit.
    * \return The NFC reader unit.
    */
    std::shared_ptr<NFCReaderUnit> getNFCReaderUnit() const
    {
        return std::dynamic_pointer_cast<NFCReaderUnit>(getReaderUnit());
    }

    /**
        * \brief Get the data transport endpoint name.
        * \return The data transport endpoint name.
        */
    std::string getName() const override;

    /**
     * \brief Send the data using rpleth protocol computation.
     * \param data The data to send.
     */
    void send(const std::vector<unsigned char> &data) override;

    /**
     * \brief Receive data from reader.
     * \param timeout The time to wait data.
     * \return The data from reader.
     */
    std::vector<unsigned char> receive(long int timeout = 5000) override;

    /**
     * \brief Send a command to the reader.
     * \param command The command buffer.
     * \param timeout The command timeout.
     * \return the result of the command.
     */
    std::vector<unsigned char> sendCommand(const std::vector<unsigned char> &command,
                                           long int timeout = 2000) override;

    /**
    * \brief Check the NFC error and throw exception if needed.
    * \param errorFlag The error flag.
    */
    static void CheckNFCError(int errorFlag);

    /**
   * Set the Ignore All Error flag to `ignore`.
   *
   * This is useful when we want to do hacky thing that are expected to
   * raise error.
   * It returns the previous value (before your change).
   */
    bool ignoreAllError(bool ignore);

    /**
     * Return the value of the Ignore All Error flag
     */
    bool ignoreAllError() const;

  protected:
    bool d_isConnected;

    std::vector<unsigned char> d_response;

    bool ignore_error_;
};
}

#endif /* LOGICALACCESS_NFCDATATRANSPORT_HPP */