/**
 * \file mifarenfccommands.hpp
 * \author Maxime C. <maxime-dev@islog.com>
 * \brief Mifare NFC card.
 */

#ifndef LOGICALACCESS_MIFARENFCCOMMANDS_HPP
#define LOGICALACCESS_MIFARENFCCOMMANDS_HPP

#include "mifarecommands.hpp"
#include "../readercardadapters/nfcreadercardadapter.hpp"

#include <string>
#include <vector>
#include <iostream>

namespace logicalaccess
{
    /**
     * \brief The Mifare card provider class for NFC reader.
     */
    class LIBLOGICALACCESS_API MifareNFCCommands : public virtual MifareCommands
    {
    public:

        /**
         * \brief Constructor.
         */
        MifareNFCCommands();

        /**
         * \brief Destructor.
         */
        virtual ~MifareNFCCommands();

        /**
         * \brief Get the NFC reader/card adapter.
         * \return The NFC reader/card adapter.
         */
        std::shared_ptr<NFCReaderCardAdapter> getNFCReaderCardAdapter() { return std::dynamic_pointer_cast<NFCReaderCardAdapter>(getReaderCardAdapter()); };

        /**
         * \brief Read bytes from the card.
         * \param blockno The block number.
         * \param len The count of bytes to read. (0 <= len < 16)
         * \param buf The buffer in which to place the data.
         * \param buflen The length of buffer.
         * \return The count of bytes red.
         */
        virtual std::vector<unsigned char> readBinary(unsigned char blockno, size_t len);

        /**
         * \brief Write bytes to the card.
         * \param blockno The block number.
         * \param buf The buffer containing the data.
         * \param buflen The length of buffer.
         * \return The count of bytes written.
         */
        virtual void updateBinary(unsigned char blockno, const std::vector<unsigned char>& buf);

		/**
		* \brief Increment a block value.
		* \param blockno The block number.
		* \param value The increment value.
		*/
		virtual void increment(unsigned char blockno, unsigned int value);

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
		virtual void decrement(unsigned char blockno, unsigned int value);

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
         * \param key The key byte array.
         * \param keylen The key byte array length.
         * \param vol Use volatile memory.
         * \return true on success, false otherwise.
         */
        bool loadKey(unsigned char keyno, MifareKeyType keytype, const void* key, size_t keylen, bool vol = false);

        /**
         * \brief Load a key on a given location.
         * \param location The location.
         * \param key The key.
         * \param keytype The mifare key type.
         */
        virtual void loadKey(std::shared_ptr<Location> location, std::shared_ptr<Key> key, MifareKeyType keytype);

        /**
         * \brief Authenticate a block, given a key number.
         * \param blockno The block number.
         * \param key_storage The key storage used for authentication.
         * \param keytype The key type.
         */
        virtual void authenticate(unsigned char blockno, std::shared_ptr<KeyStorage> key_storage, MifareKeyType keytype);

        /**
         * \brief Authenticate a block, given a key number.
         * \param blockno The block number.
         * \param keyno The key number, previously loaded with Mifare::loadKey().
         * \param keytype The key type.
         */
        void authenticate(unsigned char blockno, unsigned char keyno, MifareKeyType keytype);
		
	protected:
		
		std::vector<unsigned char> d_keys[255];
    };
}

#endif /* LOGICALACCESS_MIFARENFCCOMMANDS_HPP */