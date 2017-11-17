#pragma once

#include <logicalaccess/services/uidchanger/uidchangerservice.hpp>

namespace logicalaccess
{
class NFCReaderUnit;

#define MIFARECLASSIC_UID_CHANGERSERVICE "MifareClassicUIDChanger"
class LIBLOGICALACCESS_API MifareClassicUIDChangerCardService : public UIDChangerCardService
{

  public:
    explicit MifareClassicUIDChangerCardService(std::shared_ptr<Chip>);

    void changeUID(const std::vector<uint8_t> &new_uid) override;

    std::string getCSType() override { return MIFARECLASSIC_UID_CHANGERSERVICE; }

  private:
    std::shared_ptr<NFCReaderUnit> reader_unit_;
};
}