#include <string>
#include <memory>
#include "logicalaccess/cards/chip.hpp"
#include "logicalaccess/services/cardservice.hpp"
#include "logicalaccess/cards/commands.hpp"
#include "logicalaccess/readerproviders/readerprovider.hpp"
#include "nfcreaderprovider.hpp"
#include "commands/mifareclassicuidchangerservice.hpp"

#ifdef _MSC_VER
#include "logicalaccess/msliblogicalaccess.h"
#else
#ifndef LIBLOGICALACCESS_API
#define LIBLOGICALACCESS_API
#endif
#ifndef DISABLE_PRAGMA_WARNING
#define DISABLE_PRAGMA_WARNING /**< \brief winsmcrd.h was modified to support this macro, to avoid MSVC specific warnings pragma */
#endif
#endif

extern "C"
{
    LIBLOGICALACCESS_API char *getLibraryName()
    {
        return (char *)"NFC";
    }

    LIBLOGICALACCESS_API void getNFCReader(std::shared_ptr<logicalaccess::ReaderProvider>* rp)
    {
        if (rp != nullptr)
        {
            *rp = logicalaccess::NFCReaderProvider::createInstance();
        }
    }

    LIBLOGICALACCESS_API bool getReaderInfoAt(unsigned int index, char* readername, size_t readernamelen, void** getterfct)
    {
        bool ret = false;
        if (readername != nullptr && readernamelen == PLUGINOBJECT_MAXLEN && getterfct != nullptr)
        {
            switch (index)
            {
            case 0:
            {
                *getterfct = (void*)&getNFCReader;
                sprintf(readername, READER_NFC);
                ret = true;
            }
                break;
            default: ;
            }
        }

        return ret;
    }

    LIBLOGICALACCESS_API void getCardService(std::shared_ptr<logicalaccess::Chip> c,
                                             std::shared_ptr<logicalaccess::CardService> &service,
                                             logicalaccess::CardServiceType type)
    {
        // Only support UIDChanger service.
        if (type != logicalaccess::CST_UID_CHANGER)
            return;

        // We must first fetch the reader unit and make sure its a NFCReaderUnit,
        // otherwise we cannot create the Service.
        if (!c || !c->getCommands() || !c->getCommands()->getReaderCardAdapter() ||
            !c->getCommands()->getReaderCardAdapter()->getDataTransport() ||
            !c->getCommands()->getReaderCardAdapter()->getDataTransport()->getReaderUnit())
        {
            return;
        }
        if (c->getCardType() == "Mifare1K" || c->getCardType() == "Mifare4K")
        {
            service = std::make_shared<logicalaccess::MifareClassicUIDChangerService>(c);
        }
    }
}