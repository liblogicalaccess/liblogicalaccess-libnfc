/**
 * \file nfcreaderprovider.cpp
 * \author Maxime C. <maxime-dev@islog.com>
 * \brief NFC reader provider.
 */

#include "nfcreaderprovider.hpp"
#include "logicalaccess/myexception.hpp"

#ifdef __unix__
#include <stdlib.h>
#include <sys/time.h>
#endif

#include <sstream>
#include <iomanip>

#include "nfcreaderunit.hpp"

namespace logicalaccess
{
    NFCReaderProvider::NFCReaderProvider() :
        ReaderProvider()
    {
		nfc_init(&d_context);
		if (d_context == NULL)
			THROW_EXCEPTION_WITH_LOG(LibLogicalAccessException, "Unable to init libnfc");
    }

    std::shared_ptr<NFCReaderProvider> NFCReaderProvider::getSingletonInstance()
    {
        static std::shared_ptr<NFCReaderProvider> instance;
        if (!instance)
        {
            instance = std::shared_ptr<NFCReaderProvider>(new NFCReaderProvider());
            instance->refreshReaderList();
        }
        return instance;
    }

    std::shared_ptr<NFCReaderProvider> NFCReaderProvider::createInstance()
    {
        std::shared_ptr<NFCReaderProvider> instance = std::shared_ptr<NFCReaderProvider>(new NFCReaderProvider());
        instance->refreshReaderList();

        return instance;
    }

    NFCReaderProvider::~NFCReaderProvider()
    {
        release();
    }

    void NFCReaderProvider::release()
    {
		if (d_context != NULL)
			nfc_exit(d_context);
    }

    std::shared_ptr<ReaderUnit> NFCReaderProvider::createReaderUnit()
    {
        LOG(LogLevel::INFOS) << "Creating new reader unit";

        std::shared_ptr<NFCReaderUnit> ret(new NFCReaderUnit(std::string("")));
        ret->setReaderProvider(std::weak_ptr<ReaderProvider>(shared_from_this()));
        d_readers.push_back(ret);

        return ret;
    }

    bool NFCReaderProvider::refreshReaderList()
    {
        d_readers.clear();

		nfc_connstring devices[255];
		size_t device_count = nfc_list_devices(d_context, devices, 255);
		for (size_t i = 0; i < device_count; ++i)
		{
			std::shared_ptr<NFCReaderUnit> unit = NFCReaderUnit::createNFCReaderUnit(devices[i]);
			unit->setReaderProvider(std::weak_ptr<ReaderProvider>(shared_from_this()));
			d_readers.push_back(unit);
		}

        return true;
    }
}