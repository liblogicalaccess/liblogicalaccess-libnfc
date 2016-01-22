/**
 * /!\ WARNING /!\
 *
 * This test cannot really be trusted. While it run the UID changing code,
 * this code disable all error handling. To make sure the UID was change, you need
 * to physically remove the card from the reader, and read-it somewhere else.
 */

#include <pluginsreaderproviders/nfc/nfcreaderunit.hpp>
#include "logicalaccess/dynlibrary/idynlibrary.hpp"
#include "logicalaccess/dynlibrary/librarymanager.hpp"
#include "logicalaccess/readerproviders/readerconfiguration.hpp"
#include "logicalaccess/services/storage/storagecardservice.hpp"
#include "logicalaccess/plugins/cards/mifare/mifarechip.hpp"
#include "logicalaccess/cards/locationnode.hpp"

#include "pluginsreaderproviders/nfc/nfcreaderunit.hpp"

#include "logicalaccess/plugins/lla-tests/macros.hpp"
#include "logicalaccess/plugins/lla-tests/utils.hpp"

void introduction()
{
    PRINT_TIME("This test target Mifare1K cards. It tests the implementation of UID changing");

    PRINT_TIME("You will have 20 seconds to insert a card. Test log below");
    PRINT_TIME("~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~");

    LLA_SUBTEST_REGISTER("ReadUID");
    LLA_SUBTEST_REGISTER("WriteUID");
}

int main(int ac, char **av)
{
    using namespace logicalaccess;
    prologue(ac, av);
    introduction();
    ReaderProviderPtr provider;
    ReaderUnitPtr readerUnit;
    ChipPtr chip;
    std::tie(provider, readerUnit, chip) = nfc_test_init(); // cannot work with pcsc

    PRINT_TIME("Chip identifier: " <<
                       logicalaccess::BufferHelper::getHex(chip->getChipIdentifier()));

    LLA_ASSERT(chip->getCardType() == "Mifare1K",
               "Chip is not a Mifare1K, but is " + chip->getCardType() +
                       " instead.");

    PRINT_TIME("Reader name = " << readerUnit->getName());
    PRINT_TIME("Reader connected name = " << readerUnit->getConnectedName());

    //return 0;
    auto nfc_ru = std::dynamic_pointer_cast<logicalaccess::NFCReaderUnit>(readerUnit);
    LLA_ASSERT(nfc_ru, "ReaderUnit is not NFC.");

    std::vector<uint8_t> expected = {0x42, 0x42, 0x42, 0x42};
    LLA_ASSERT(chip->getChipIdentifier() == expected, "Invalid identifier (init)");
    LLA_SUBTEST_PASSED("ReadUID");

    nfc_ru->writeChipUid(chip, {0x84, 0x84, 0x84, 0x84});
    expected = {0x84, 0x84, 0x84, 0x84};
    LLA_ASSERT(nfc_ru->getNumber(chip) == expected, "Invalid identifier (first change)");

    nfc_ru->writeChipUid(chip, {0x42, 0x42, 0x42, 0x42});
    expected = {0x42, 0x42, 0x42, 0x42};
    LLA_ASSERT(chip->getChipIdentifier() == expected, "Invalid identifier (final)");
    LLA_SUBTEST_PASSED("WriteUID");

    pcsc_test_shutdown(readerUnit);
    return 0;
}
