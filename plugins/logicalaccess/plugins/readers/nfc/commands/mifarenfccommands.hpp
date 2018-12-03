/**
 * \file mifarenfccommands.hpp
 * \author Maxime C. <maxime-dev@islog.com>
 * \brief Mifare NFC card.
 */

#ifndef LOGICALACCESS_MIFARENFCCOMMANDS_HPP
#define LOGICALACCESS_MIFARENFCCOMMANDS_HPP

#include <logicalaccess/plugins/cards/mifare/mifarecommands.hpp>
#include <logicalaccess/plugins/readers/nfc/readercardadapters/nfcreadercardadapter.hpp>

#include <string>
#include <vector>
#include <iostream>

namespace logicalaccess
{
#define CMD_MIFARENFC "MifareNFC"
/**
 * \brief The Mifare card provider class for NFC reader.
 */
class LLA_READERS_NFC_NFC_API MifareNFCCommands : public MifareCommands
{
  public:
    /**
     * \brief Constructor.
     */
    MifareNFCCommands();

    MifareNFCCommands(std::string);

    /**
     * \brief Destructor.
     */
    virtual ~MifareNFCCommands();

    /**
     * \brief Get the NFC reader/card adapter.
     * \return The NFC reader/card adapter.
     */
    std::shared_ptr<NFCReaderCardAdapter> getNFCReaderCardAdapter() const
    {
        return std::dynamic_pointer_cast<NFCReaderCardAdapter>(getReaderCardAdapter());
    }

    /**
 * \brief Read bytes from the card.
 * \param blockno The block number.
 * \param len The count of bytes to read. (0 <= len < 16)
 * \return The count of bytes red.
 */
    std::vector<unsigned char> readBinary(unsigned char blockno, size_t len) override;

    /**
     * \brief Write bytes to the card.
     * \param blockno The block number.
     * \param buf The buffer containing the data.
     * \return The count of bytes written.
     */
    void updateBinary(unsigned char blockno,
                      const std::vector<unsigned char> &buf) override;

    /**
    * \brief Increment a block value.
    * \param blockno The block number.
    * \param value The increment value.
    */
    void increment(unsigned char blockno, unsigned int value) override;

    /**
    * \brief Increment a block value, without transfer.
    * \param blockno The block number.
    * \param value The increment value.
    */
    virtual void increment_raw(unsigned char blockno, unsigned int value);

    /**
    * \brief Decrement a block value.
    * \param blockno The block number.
    * \param value The decrement value.
    */
    void decrement(unsigned char blockno, unsigned int value) override;

    /**
    * \brief Decrement a block value, without transfer.
    * \param blockno The block number.
    * \param value The decrement value.
    */
    virtual void decrement_raw(unsigned char blockno, unsigned int value);

    /**
    * \brief Transfer volatile memory to block value.
    * \param blockno The block number.
    */
    virtual void transfer(unsigned char blockno);

    /**
    * \brief Store block value to volatile memory.
    * \param blockno The block number.
    */
    virtual void restore(unsigned char blockno);

  protected:
    /**
     * \brief Load a key to the reader.
     * \param keyno The key number.
     * \param keytype The mifare key type.
     * \param key The key.
     * \param vol Use volatile memory.
     * \return true on success, false otherwise.
     */
    bool loadKey(unsigned char keyno, MifareKeyType keytype,
                 std::shared_ptr<MifareKey> key, bool vol = false) override;

    /**
     * \brief Load a key on a given location.
     * \param location The location.
     * \param key The key.
     * \param keytype The mifare key type.
     */
    void loadKey(std::shared_ptr<Location> location, MifareKeyType keytype,
                 std::shared_ptr<MifareKey> key) override;

    /**
     * \brief Authenticate a block, given a key number.
     * \param blockno The block number.
     * \param key_storage The key storage used for authentication.
     * \param keytype The key type.
     */
    void authenticate(unsigned char blockno, std::shared_ptr<KeyStorage> key_storage,
                      MifareKeyType keytype) override;

    /**
     * \brief Authenticate a block, given a key number.
     * \param blockno The block number.
     * \param keyno The key number, previously loaded with Mifare::loadKey().
     * \param keytype The key type.
     */
    void authenticate(unsigned char blockno, unsigned char keyno,
                      MifareKeyType keytype) override;

    std::vector<unsigned char> d_keys[255];
};
}

#endif /* LOGICALACCESS_MIFARENFCCOMMANDS_HPP */