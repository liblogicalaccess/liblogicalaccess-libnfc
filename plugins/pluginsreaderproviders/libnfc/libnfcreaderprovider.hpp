/**
 * \file libnfcreaderprovider.hpp
 * \author Maxime C. <maxime-dev@islog.com>
 * \brief LibNFC card reader provider.
 */

#ifndef LOGICALACCESS_READERLIBNFC_PROVIDER_HPP
#define LOGICALACCESS_READERLIBNFC_PROVIDER_HPP

#include "logicalaccess/readerproviders/readerprovider.hpp"
#include "libnfcreaderunit.hpp"

#include <string>
#include <vector>

#include "logicalaccess/logs.hpp"

namespace logicalaccess
{
#define READER_LIBNFC		"LibNFC"

    /**
     * \brief LibNFC Reader Provider class.
     */
    class LIBLOGICALACCESS_API LibNFCReaderProvider : public ReaderProvider
    {
    public:

        /**
         * \brief Get the LibNFCReaderProvider instance.
         */
        static std::shared_ptr<LibNFCReaderProvider> getSingletonInstance();

        /**
         * \brief Get a new LibNFCReaderProvider instance.
         */
        std::shared_ptr<LibNFCReaderProvider> createInstance();

        /**
         * \brief Destructor.
         */
        ~LibNFCReaderProvider();

        /**
         * \brief Release the provider resources.
         */
        virtual void release();

        /**
         * \brief Get the reader provider type.
         * \return The reader provider type.
         */
        virtual std::string getRPType() const { return READER_LIBNFC; };

        /**
         * \brief Get the reader provider name.
         * \return The reader provider name.
         */
        virtual std::string getRPName() const { return "LibNFC"; };

        /**
         * \brief List all readers of the system.
         * \return True if the list was updated, false otherwise.
         */
        virtual bool refreshReaderList();

        /**
         * \brief Get reader list for this reader provider.
         * \return The reader list.
         */
        virtual const ReaderList& getReaderList() { return d_readers; };

        /**
         * \brief Create a new reader unit for the reader provider.
         * param ip The reader address
         * param port The reader port
         * \return A reader unit.
         */
        virtual std::shared_ptr<ReaderUnit> createReaderUnit();

    protected:

        /**
         * \brief Constructor.
         */
        LibNFCReaderProvider();

        /**
         * \brief The reader list.
         */
        ReaderList d_readers;
    };
}

#endif /* LOGICALACCESS_READERLIBNFC_PROVIDER_HPP */