/**
 * \file nfcreaderprovider.hpp
 * \author Maxime C. <maxime-dev@islog.com>
 * \brief NFC card reader provider.
 */

#ifndef LOGICALACCESS_READERNFC_PROVIDER_HPP
#define LOGICALACCESS_READERNFC_PROVIDER_HPP

#include <logicalaccess/readerproviders/readerprovider.hpp>
#include <logicalaccess/plugins/readers/nfc/nfcreaderunit.hpp>

#include <string>
#include <vector>

#include <logicalaccess/plugins/llacommon/logs.hpp>


namespace logicalaccess
{
#define READER_NFC "NFC"

/**
 * \brief NFC Reader Provider class.
 */
class LIBLOGICALACCESS_API NFCReaderProvider : public ReaderProvider
{
  public:
    /**
    * \brief Create a new NFC reader provider instance.
    * \return The NFC reader provider instance.
    */
    static std::shared_ptr<NFCReaderProvider> createInstance();

    /**
     * \brief Destructor.
     */
    ~NFCReaderProvider();

    /**
     * \brief Release the provider resources.
     */
    void release() override;

    /**
     * \brief Get the reader provider type.
     * \return The reader provider type.
     */
    std::string getRPType() const override
    {
        return READER_NFC;
    }

    /**
 * \brief Get the reader provider name.
 * \return The reader provider name.
 */
    std::string getRPName() const override
    {
        return "NFC";
    }

    /**
     * \brief List all readers of the system.
     * \return True if the list was updated, false otherwise.
     */
    bool refreshReaderList() override;

    /**
     * \brief Get reader list for this reader provider.
     * \return The reader list.
     */
    const ReaderList &getReaderList() override
    {
        return d_readers;
    }

    /**
     * \brief Create a new reader unit for the reader provider.
     * param ip The reader address
     * param port The reader port
     * \return A reader unit.
     */
    std::shared_ptr<ReaderUnit> createReaderUnit() override;

    /**
    * \brief Get the NFC context.
    * \return The NFC context.
    */
    nfc_context *getContext() const
    {
        return d_context;
    }

  protected:
    /**
     * \brief Constructor.
     */
    NFCReaderProvider();

    /**
     * \brief The reader list.
     */
    ReaderList d_readers;

    /**
     * \brief The NFC Context.
    */
    nfc_context *d_context;
};
}

#endif /* LOGICALACCESS_READERNFC_PROVIDER_HPP */
