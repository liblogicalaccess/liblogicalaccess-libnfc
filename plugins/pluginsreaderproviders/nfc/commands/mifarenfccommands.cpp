/**
 * \file mifarenfccommands.cpp
 * \author Maxime C. <maxime-dev@islog.com>
 * \brief Mifare NFC commands.
 */

#include "../commands/mifarenfccommands.hpp"

#include <iostream>
#include <iomanip>
#include <sstream>

#include "../nfcreaderprovider.hpp"
#include "mifarechip.hpp"
#include "logicalaccess/cards/computermemorykeystorage.hpp"
#include "logicalaccess/cards/readermemorykeystorage.hpp"
#include "logicalaccess/logs.hpp"
#include "logicalaccess/myexception.hpp"

namespace logicalaccess
{
    MifareNFCCommands::MifareNFCCommands()
        : MifareCommands()
    {
    }

    MifareNFCCommands::~MifareNFCCommands()
    {
    }

    bool MifareNFCCommands::loadKey(unsigned char keyno, MifareKeyType keytype, const void* key, size_t keylen, bool vol)
    {
		d_keys[keyno] = std::vector<unsigned char>(static_cast<const unsigned char *>(key), static_cast<const unsigned char *>(key) + keylen);

		return true;
    }

    void MifareNFCCommands::loadKey(std::shared_ptr<Location> location, std::shared_ptr<Key> key, MifareKeyType keytype)
    {
        EXCEPTION_ASSERT_WITH_LOG(location, std::invalid_argument, "location cannot be null.");
        EXCEPTION_ASSERT_WITH_LOG(key, std::invalid_argument, "key cannot be null.");

        std::shared_ptr<MifareLocation> mLocation = std::dynamic_pointer_cast<MifareLocation>(location);
        std::shared_ptr<MifareKey> mKey = std::dynamic_pointer_cast<MifareKey>(key);

        EXCEPTION_ASSERT_WITH_LOG(mLocation, std::invalid_argument, "location must be a MifareLocation.");
        EXCEPTION_ASSERT_WITH_LOG(mKey, std::invalid_argument, "key must be a MifareKey.");

        std::shared_ptr<KeyStorage> key_storage = key->getKeyStorage();

        if (std::dynamic_pointer_cast<ComputerMemoryKeyStorage>(key_storage))
        {
            loadKey(0, keytype, key->getData(), key->getLength());
        }
        else
        {
            THROW_EXCEPTION_WITH_LOG(LibLogicalAccessException, "The key storage type is not supported for this card/reader.");
        }
    }

    void MifareNFCCommands::authenticate(unsigned char blockno, unsigned char keyno, MifareKeyType keytype)
    {
        std::vector<unsigned char> command;

        command.push_back(static_cast<unsigned char>(keytype));
		command.push_back(blockno);
		command.insert(command.end(), d_keys[keyno].begin(), d_keys[keyno].end());
		command.insert(command.end(), d_keys[keyno].begin(), d_keys[keyno].end());
		std::vector<unsigned char> csn = getChip()->getChipIdentifier();
		command.insert(command.end(), csn.end() - 4, csn.end());

		getNFCReaderCardAdapter()->sendCommand(command);
    }

    void MifareNFCCommands::authenticate(unsigned char blockno, std::shared_ptr<KeyStorage> key_storage, MifareKeyType keytype)
    {
        if (std::dynamic_pointer_cast<ComputerMemoryKeyStorage>(key_storage))
        {
            authenticate(blockno, 0, keytype);
        }
        else
        {
            THROW_EXCEPTION_WITH_LOG(LibLogicalAccessException, "The key storage type is not supported for this card/reader.");
        }
    }

    std::vector<unsigned char> MifareNFCCommands::readBinary(unsigned char blockno, size_t len)
    {
        if (len != 16)
        {
            THROW_EXCEPTION_WITH_LOG(std::invalid_argument, "Bad len parameter.");
        }

		std::vector<unsigned char> command;
		command.push_back(0x30);
		command.push_back(blockno);

		return getNFCReaderCardAdapter()->sendCommand(command);
    }

    void MifareNFCCommands::updateBinary(unsigned char blockno, const std::vector<unsigned char>& buf)
    {
        if (buf.size() != 16)
        {
            THROW_EXCEPTION_WITH_LOG(std::invalid_argument, "Bad buf size parameter.");
        }

		std::vector<unsigned char> command;
		command.push_back(0xA0);
		command.push_back(blockno);
		command.insert(command.end(), buf.begin(), buf.end());

		getNFCReaderCardAdapter()->sendCommand(command);
    }

	void MifareNFCCommands::increment(unsigned char blockno, unsigned int value)
	{
		increment_raw(blockno, value);
		transfer(blockno);
	}

	void MifareNFCCommands::decrement(unsigned char blockno, unsigned int value)
	{
		decrement_raw(blockno, value);
		transfer(blockno);
	}

	void MifareNFCCommands::increment_raw(unsigned char blockno, unsigned int value)
	{
		std::vector<unsigned char> command;
		command.push_back(0xC1);
		command.push_back(blockno);
		command.push_back(static_cast<unsigned char>((value >> 24) & 0xff));
		command.push_back(static_cast<unsigned char>((value >> 16) & 0xff));
		command.push_back(static_cast<unsigned char>((value >> 8) & 0xff));
		command.push_back(static_cast<unsigned char>(value & 0xff));

		getNFCReaderCardAdapter()->sendCommand(command);
	}

	void MifareNFCCommands::decrement_raw(unsigned char blockno, unsigned int value)
	{
		std::vector<unsigned char> command;
		command.push_back(0xC0);
		command.push_back(blockno);
		command.push_back(static_cast<unsigned char>((value >> 24) & 0xff));
		command.push_back(static_cast<unsigned char>((value >> 16) & 0xff));
		command.push_back(static_cast<unsigned char>((value >> 8) & 0xff));
		command.push_back(static_cast<unsigned char>(value & 0xff));

		getNFCReaderCardAdapter()->sendCommand(command);
	}

	void MifareNFCCommands::transfer(unsigned char blockno)
	{
		std::vector<unsigned char> command;
		command.push_back(0xB0);
		command.push_back(blockno);

		getNFCReaderCardAdapter()->sendCommand(command);
	}

	void MifareNFCCommands::restore(unsigned char blockno)
	{
		std::vector<unsigned char> command;
		command.push_back(0xC2);
		command.push_back(blockno);
		command.push_back(0x00);
		command.push_back(0x00);
		command.push_back(0x00);
		command.push_back(0x00);

		getNFCReaderCardAdapter()->sendCommand(command);
	}
}