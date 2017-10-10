/**
 * \file nfcreaderprovider.cpp
 * \author Maxime C. <maxime-dev@islog.com>
 * \brief NFC reader provider.
 */

#include <algorithm>
#include "nfcreaderprovider.hpp"
#include "logicalaccess/myexception.hpp"

#ifdef __unix__
#include <stdlib.h>
#include <sys/time.h>
#endif

#include <sstream>
#include <iomanip>
#include <assert.h>

#include "nfcreaderunit.hpp"

namespace logicalaccess
{
    NFCReaderProvider::NFCReaderProvider() :
        ReaderProvider()
    {
		nfc_init(&d_context);
		if (d_context == nullptr)
			THROW_EXCEPTION_WITH_LOG(LibLogicalAccessException, "Unable to init libnfc");
    }

    std::shared_ptr<NFCReaderProvider> NFCReaderProvider::createInstance()
    {
        std::shared_ptr<NFCReaderProvider> instance = std::shared_ptr<NFCReaderProvider>(new NFCReaderProvider());
        instance->refreshReaderList();

        return instance;
    }

    NFCReaderProvider::~NFCReaderProvider()
    {
	    NFCReaderProvider::release();
    }

    void NFCReaderProvider::release()
    {
		if (d_context != nullptr)
		{
			nfc_exit(d_context);
			d_context = nullptr;
		}
    }

    std::shared_ptr<ReaderUnit> NFCReaderProvider::createReaderUnit()
    {
        LOG(LogLevel::INFOS) << "Creating default NFC reader unit. "
        << "We will let libnfc pick a reader for us";

        std::shared_ptr<NFCReaderUnit> ret(new NFCReaderUnit(std::string("")));
        ret->setReaderProvider(std::weak_ptr<ReaderProvider>(shared_from_this()));
        ret->connectToReader();
        ret->fetchRealName();
        ret->disconnectFromReader();
        d_readers.push_back(ret);

        return ret;
    }

    bool NFCReaderProvider::refreshReaderList()
    {
        nfc_connstring devices[255];
        size_t device_count = nfc_list_devices(d_context, devices, 255);
        LOG(DEBUGS) << "Found " << device_count << " devices.";
        for (size_t i = 0; i < device_count; ++i)
        {
            auto itr = std::find_if(d_readers.begin(), d_readers.end(),
                                    [&](std::shared_ptr<ReaderUnit> ru)
                                    {
                                        return ru->getName() == devices[i];
                                    });
            if (itr == d_readers.end())
            {
				LOG(DEBUGS) << "THIS = " << this << "  - Found available reader{ " << devices[i] << " }";
                std::shared_ptr<NFCReaderUnit> unit =
                    NFCReaderUnit::createNFCReaderUnit(devices[i]);
                unit->setReaderProvider(
                    std::weak_ptr<ReaderProvider>(shared_from_this()));
                d_readers.push_back(unit);
                //unit->connectToReader();
                //unit->fetchRealName();
                //unit->disconnectFromReader();
            }
        }
        LOG(DEBUGS) << "THIS = " << this << "  - Reader list size = " << d_readers.size();
		for (auto ru : d_readers)
		{
			LOG(DEBUGS) << "THIS = " << this << "  - Reader name: {" << ru->getName() << "} CONNECTED NAME = {" << ru->getConnectedName() << "}";
		}
        return true;
    }
}