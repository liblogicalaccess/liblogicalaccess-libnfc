#pragma once

#include "logicalaccess/services/uidchanger/uidchangerservice.hpp"

namespace logicalaccess
{
class NFCReaderUnit;

class MifareClassicUIDChangerService : public UIDChangerService {

      public:
        MifareClassicUIDChangerService(std::shared_ptr<Chip>);

        virtual void changeUID(const std::vector<uint8_t> &new_uid);

      private:
        std::shared_ptr<NFCReaderUnit> reader_unit_;
};
}