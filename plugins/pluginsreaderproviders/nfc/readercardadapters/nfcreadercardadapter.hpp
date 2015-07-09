/**
 * \file nfcreadercardadapter.hpp
 * \author Maxime C. <maxime-dev@islog.com>
 * \brief Default NFC reader/card adapter.
 */

#ifndef LOGICALACCESS_DEFAULTNFCREADERCARDADAPTER_HPP
#define LOGICALACCESS_DEFAULTNFCREADERCARDADAPTER_HPP

#include "readercardadapters/iso7816readercardadapter.hpp"
#include "../nfcreaderprovider.hpp"

#include <string>
#include <vector>

#include "logicalaccess/logs.hpp"

namespace logicalaccess
{
    /**
     * \brief A default NFC reader/card adapter class.
     */
    class LIBLOGICALACCESS_API NFCReaderCardAdapter : public ISO7816ReaderCardAdapter
    {
    public:

        /**
         *\ brief Constructor.
         */
        NFCReaderCardAdapter();

        /**
         * \brief Destructor.
         */
        virtual ~NFCReaderCardAdapter();

        /**
         * \brief Adapt the command to send to the reader.
         * \param command The command to send.
         * \return The adapted command to send.
         */
        virtual std::vector<unsigned char> adaptCommand(const std::vector<unsigned char>& command);

        /**
         * \brief Adapt the asnwer received from the reader.
         * \param answer The answer received.
         * \return The adapted answer received.
         */
        virtual std::vector<unsigned char> adaptAnswer(const std::vector<unsigned char>& answer);

        /**
         * We override this because a 0 byte return is valid with libnfc.
         */
        virtual std::vector<unsigned char> sendCommand(
                const std::vector<unsigned char> &command, long timeout = 3000) override;

    protected:
    };
}

#endif /* LOGICALACCESS_DEFAULTNFCREADERCARDADAPTER_HPP */