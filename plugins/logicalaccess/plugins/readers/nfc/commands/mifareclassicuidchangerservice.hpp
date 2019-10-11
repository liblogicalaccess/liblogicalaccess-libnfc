#pragma once

#include <logicalaccess/services/uidchanger/uidchangerservice.hpp>
#include <logicalaccess/plugins/readers/nfc/lla_readers_nfc_nfc_api.hpp>

namespace logicalaccess
{
class NFCReaderUnit;

#define MIFARECLASSIC_UID_CHANGERSERVICE "MifareClassicUIDChanger"
class LLA_READERS_NFC_NFC_API MifareClassicUIDChangerCardService : public UIDChangerCardService
{

  public:
    explicit MifareClassicUIDChangerCardService(std::shared_ptr<Chip>);

    void changeUID(const std::vector<uint8_t> &new_uid) override;

    std::string getCSType() override { return MIFARECLASSIC_UID_CHANGERSERVICE; }

  private:
    std::shared_ptr<NFCReaderUnit> reader_unit_;
};
}