/**
 * \file libnfcreaderprovider.cpp
 * \author Maxime C. <maxime-dev@islog.com>
 * \brief LibNFC reader provider.
 */

#include "libnfcreaderprovider.hpp"

#ifdef __unix__
#include <stdlib.h>
#include <sys/time.h>
#endif

#include <sstream>
#include <iomanip>

#include "libnfcreaderunit.hpp"

namespace logicalaccess
{
    LibNFCReaderProvider::LibNFCReaderProvider() :
        ReaderProvider()
    {
    }

    std::shared_ptr<LibNFCReaderProvider> LibNFCReaderProvider::getSingletonInstance()
    {
        static std::shared_ptr<LibNFCReaderProvider> instance;
        if (!instance)
        {
            instance = std::shared_ptr<LibNFCReaderProvider>(new LibNFCReaderProvider());
            instance->refreshReaderList();
        }
        return instance;
    }

    std::shared_ptr<LibNFCReaderProvider> LibNFCReaderProvider::createInstance()
    {
        std::shared_ptr<LibNFCReaderProvider> instance = std::shared_ptr<LibNFCReaderProvider>(new LibNFCReaderProvider());
        instance->refreshReaderList();

        return instance;
    }

    LibNFCReaderProvider::~LibNFCReaderProvider()
    {
        release();
    }

    void LibNFCReaderProvider::release()
    {
    }

    std::shared_ptr<ReaderUnit> LibNFCReaderProvider::createReaderUnit()
    {
        LOG(LogLevel::INFOS) << "Creating new reader unit";

        std::shared_ptr<LibNFCReaderUnit> ret(new LibNFCReaderUnit());
        ret->setReaderProvider(std::weak_ptr<ReaderProvider>(shared_from_this()));
        d_readers.push_back(ret);

        return ret;
    }

    bool LibNFCReaderProvider::refreshReaderList()
    {
        d_readers.clear();

        return true;
    }
}