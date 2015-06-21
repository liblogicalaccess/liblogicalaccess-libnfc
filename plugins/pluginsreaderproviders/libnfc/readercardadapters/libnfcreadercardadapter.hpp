/**
 * \file libnfcreadercardadapter.hpp
 * \author Maxime C. <maxime-dev@islog.com>
 * \brief Default LibNFC reader/card adapter.
 */

#ifndef LOGICALACCESS_DEFAULTLIBNFCREADERCARDADAPTER_HPP
#define LOGICALACCESS_DEFAULTLIBNFCREADERCARDADAPTER_HPP

#include "logicalaccess/cards/readercardadapter.hpp"
#include "../libnfcreaderunit.hpp"

#include <string>
#include <vector>

#include "logicalaccess/logs.hpp"

namespace logicalaccess
{
    /**
     * \brief A default LibNFC reader/card adapter class.
     */
    class LIBLOGICALACCESS_API LibNFCReaderCardAdapter : public ReaderCardAdapter
    {
    public:

        /**
         * \brief Constructor.
         */
        LibNFCReaderCardAdapter();

        /**
         * \brief Destructor.
         */
        virtual ~LibNFCReaderCardAdapter();

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
    };
}

#endif /* LOGICALACCESS_DEFAULTLIBNFCREADERCARDADAPTER_HPP */