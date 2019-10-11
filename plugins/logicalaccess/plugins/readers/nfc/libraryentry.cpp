#include <string>
#include <memory>
#include <logicalaccess/cards/chip.hpp>
#include <logicalaccess/services/cardservice.hpp>
#include <logicalaccess/cards/commands.hpp>
#include <logicalaccess/readerproviders/readerprovider.hpp>
#include <logicalaccess/plugins/readers/nfc/nfcreaderprovider.hpp>
#include <logicalaccess/plugins/readers/nfc/commands/mifareclassicuidchangerservice.hpp>
#include <logicalaccess/plugins/readers/nfc/lla_readers_nfc_nfc_api.hpp>

extern "C" {
LLA_READERS_NFC_NFC_API char *getLibraryName()
{
    return (char *)"NFC";
}

LLA_READERS_NFC_NFC_API void getNFCReader(std::shared_ptr<logicalaccess::ReaderProvider> *rp)
{
    if (rp != nullptr)
    {
        *rp = logicalaccess::NFCReaderProvider::createInstance();
    }
}

LLA_READERS_NFC_NFC_API bool getReaderInfoAt(unsigned int index, char *readername,
                                          size_t readernamelen, void **getterfct)
{
    bool ret = false;
    if (readername != nullptr && readernamelen == PLUGINOBJECT_MAXLEN &&
        getterfct != nullptr)
    {
        switch (index)
        {
        case 0:
        {
            *getterfct = (void *)&getNFCReader;
            sprintf(readername, READER_NFC);
            ret = true;
        }
        break;
        default:;
        }
    }

    return ret;
}

LLA_READERS_NFC_NFC_API void
getCardService(std::shared_ptr<logicalaccess::Chip> c,
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
        service = std::make_shared<logicalaccess::MifareClassicUIDChangerCardService>(c);
    }
}
}