#pragma once

#include <logicalaccess/services/uidchanger/uidchangerservice.hpp>

namespace logicalaccess
{
class NFCReaderUnit;

class LIBLOGICALACCESS_API MifareClassicUIDChangerService : public UIDChangerService
{

  public:
    explicit MifareClassicUIDChangerService(std::shared_ptr<Chip>);

    void changeUID(const std::vector<uint8_t> &new_uid) override;

  private:
    std::shared_ptr<NFCReaderUnit> reader_unit_;
};
}