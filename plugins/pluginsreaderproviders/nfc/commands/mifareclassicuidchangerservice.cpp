#include "mifareclassicuidchangerservice.hpp"
#include "../nfcreaderunit.hpp"
#include "logicalaccess/cards/chip.hpp"
#include "logicalaccess/cards/commands.hpp"

using namespace logicalaccess;

MifareClassicUIDChangerService::MifareClassicUIDChangerService(std::shared_ptr<Chip> chip)
    : UIDChangerService(chip)
{
    // Store the reader unit used when the service was created.
    // We will talk directly to the reader-unit when performing an UID change.
    if (getChip() && getChip()->getCommands() &&
        getChip()->getCommands()->getReaderCardAdapter() &&
        getChip()->getCommands()->getReaderCardAdapter()->getDataTransport())
    {
        reader_unit_ =
            std::dynamic_pointer_cast<NFCReaderUnit>(getChip()
                                                         ->getCommands()
                                                         ->getReaderCardAdapter()
                                                         ->getDataTransport()
                                                         ->getReaderUnit());
    }

    EXCEPTION_ASSERT_WITH_LOG(reader_unit_, LibLogicalAccessException,
                              "Cannot retrieve an "
                              "instance of NFCReaderUnit from the card.");
}

void MifareClassicUIDChangerService::changeUID(const std::vector<uint8_t> &new_uid)
{
    EXCEPTION_ASSERT_WITH_LOG(
        new_uid.size() == 4, LibLogicalAccessException,
        "When changing the UID of a Mifare Classic card, the uid must be 4 bytes long.");
    reader_unit_->writeChipUid(getChip(), new_uid);
}
