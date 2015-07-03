/**
 * \file nfcreaderunit.cpp
 * \author Maxime C. <maxime-dev@islog.com>
 * \brief NFC reader unit.
 */

#include "nfcreaderunit.hpp"

#include <iostream>
#include <iomanip>
#include <sstream>

#include "logicalaccess/dynlibrary/librarymanager.hpp"
#include "logicalaccess/dynlibrary/idynlibrary.hpp"
#include "logicalaccess/myexception.hpp"
#include "logicalaccess/settings.hpp"

#include "logicalaccess/bufferhelper.hpp"
#include "nfcreaderprovider.hpp"
#include "logicalaccess/cards/chip.hpp"
#include <boost/filesystem.hpp>
#include <boost/property_tree/ptree.hpp>
#include <boost/date_time.hpp>
#include <boost/property_tree/xml_parser.hpp>
#include <thread>
#include "readercardadapters/nfcreadercardadapter.hpp"
#include "nfcdatatransport.hpp"
#include "commands/mifarenfccommands.hpp"
#include "commands/desfireev1iso7816commands.hpp"
#include "commands/desfireiso7816resultchecker.hpp"
#include "iso7816resultchecker.hpp"

namespace logicalaccess
{
	// Reader unit code for card detection based on libfreefare project

	#define NXP_MANUFACTURER_CODE 0x04
	#define MAX_CANDIDATES 16

#ifndef _WIN64
	struct supported_tag {
		const char *card_type;
		uint8_t modulation_type;
		uint8_t SAK;
		uint8_t ATS_min_length;
		uint8_t ATS_compare_length;
		uint8_t ATS[5];
		bool(*check_tag_on_reader) (nfc_device *, nfc_iso14443a_info);
	};

	struct supported_tag supported_tags[] = {
		{ "FeliCA", NMT_FELICA, 0x00, 0, 0, { 0x00 }, NULL }, // FeliCA
		{ "Mifare1K", NMT_ISO14443A, 0x08, 0, 0, { 0x00 }, NULL }, // Mifare Classic 1k
		{ "Mifare1K", NMT_ISO14443A, 0x28, 0, 0, { 0x00 }, NULL }, // Mifare Classic 1k (Emulated)
		{ "Mifare1K", NMT_ISO14443A, 0x68, 0, 0, { 0x00 }, NULL }, // Mifare Classic 1k (Emulated)
		{ "Mifare1K", NMT_ISO14443A, 0x88, 0, 0, { 0x00 }, NULL }, // Infineon Mifare Classic 1k
		{ "Mifare4K", NMT_ISO14443A, 0x18, 0, 0, { 0x00 }, NULL }, // Mifare Classic 4k
		{ "Mifare4K", NMT_ISO14443A, 0x38, 0, 0, { 0x00 }, NULL }, // Mifare Classic 4k (Emulated)
		{ "DESFireEV1", NMT_ISO14443A, 0x20, 5, 4, { 0x75, 0x77, 0x81, 0x02 /*, 0xXX */ }, NULL }, // Mifare DESFire
		{ "DESFireEV1", NMT_ISO14443A, 0x60, 4, 3, { 0x78, 0x33, 0x88 /*, 0xXX */ }, NULL }, // Cyanogenmod card emulation
		{ "DESFireEV1", NMT_ISO14443A, 0x60, 4, 3, { 0x78, 0x80, 0x70 /*, 0xXX */ }, NULL }, // Android HCE
		//{ "MifareUltralightC", NMT_ISO14443A, 0x00, 0, 0, { 0x00 }, is_mifare_ultralightc_on_reader }, // Mifare UltraLightC
		{ "MifareUltralight", NMT_ISO14443A, 0x00, 0, 0, { 0x00 }, NULL }, // Mifare UltraLight
	};
#endif

    NFCReaderUnit::NFCReaderUnit(const std::string& name):
        ReaderUnit(),
#ifndef _WIN64
        d_device(NULL),
#endif
        d_name(name),
        d_connectedName(name),
        d_chip_connected(false)
	{
        d_readerUnitConfig.reset(new NFCReaderUnitConfiguration());
        setDefaultReaderCardAdapter(std::shared_ptr<NFCReaderCardAdapter>(new NFCReaderCardAdapter()));

        std::shared_ptr<NFCDataTransport> dataTransport(new NFCDataTransport());
        setDataTransport(dataTransport);
        d_card_type = "UNKNOWN";

        try
        {
            boost::property_tree::ptree pt;
            read_xml((boost::filesystem::current_path().string() + "/NFCReaderUnit.config"), pt);
            d_card_type = pt.get("config.cardType", "UNKNOWN");
        }
        catch (...) {}
    }

    NFCReaderUnit::~NFCReaderUnit()
    {
        disconnectFromReader();
    }

    std::string NFCReaderUnit::getName() const
    {
		return d_name;
    }

    std::string NFCReaderUnit::getConnectedName()
    {
		return d_connectedName;
    }

    void NFCReaderUnit::setCardType(std::string cardType)
    {
        d_card_type = cardType;
    }

    bool NFCReaderUnit::waitInsertion(unsigned int maxwait)
    {
        bool inserted = false;

		if (Settings::getInstance()->SeeWaitInsertionLog)
		{
			LOG(LogLevel::INFOS) << "Waiting card insertion...";
		}

#ifndef _WIN64
		if (d_device != NULL)
		{
			boost::posix_time::ptime currentDate = boost::posix_time::second_clock::local_time();
			boost::posix_time::ptime maxDate = currentDate + boost::posix_time::milliseconds(maxwait);

			while (!inserted && currentDate < maxDate)
			{
				refreshChipList();
				if (d_chips.size() != 0)
				{
					d_insertedChip = d_chips.cbegin()->first;
					inserted = true;
				}
				else
				{
					currentDate = boost::posix_time::second_clock::local_time();
					std::this_thread::sleep_for(std::chrono::milliseconds(50));
				}
			}
		}
#endif

        return inserted;
    }

    bool NFCReaderUnit::waitRemoval(unsigned int maxwait)
    {
        bool removed = false;

		if (d_insertedChip)
		{
#ifndef _WIN64
			if (d_chips.find(d_insertedChip) != d_chips.end())
			{
				boost::posix_time::ptime currentDate = boost::posix_time::second_clock::local_time();
				boost::posix_time::ptime maxDate = currentDate + boost::posix_time::milliseconds(maxwait);

				while (!removed && currentDate < maxDate)
				{
					nfc_target target = d_chips[d_insertedChip];
					removed = (nfc_initiator_target_is_present(d_device, &target) != NFC_SUCCESS);
					if (!removed)
					{
						currentDate = boost::posix_time::second_clock::local_time();
						std::this_thread::sleep_for(std::chrono::milliseconds(50));
					}
				}
			}
			else
			{
				removed = true;
			}
#endif
		}
		else
		{
			removed = true;
		}

        return removed;
    }

#ifndef _WIN64
	std::string NFCReaderUnit::getCardTypeFromTarget(nfc_target target)
	{
		bool found = false;
		struct supported_tag *tag_info;

		/* Ensure the target is supported */
		for (size_t i = 0; i < sizeof(supported_tags) / sizeof(struct supported_tag); i++)
		{
			if (target.nm.nmt != supported_tags[i].modulation_type)
				continue;

			if (target.nm.nmt == NMT_FELICA)
			{
				tag_info = &(supported_tags[i]);
				found = true;
				break;
			}
			if ((target.nm.nmt == NMT_ISO14443A) && ((target.nti.nai.szUidLen == 4) || (target.nti.nai.abtUid[0] == NXP_MANUFACTURER_CODE)) &&
				(target.nti.nai.btSak == supported_tags[i].SAK) &&
				(!supported_tags[i].ATS_min_length || ((target.nti.nai.szAtsLen >= supported_tags[i].ATS_min_length) &&
				(0 == memcmp(target.nti.nai.abtAts, supported_tags[i].ATS, supported_tags[i].ATS_compare_length)))) &&
				((supported_tags[i].check_tag_on_reader == NULL) ||
				supported_tags[i].check_tag_on_reader(d_device, target.nti.nai)))
			{
				tag_info = &(supported_tags[i]);
				found = true;
				break;
			}
		}

		if (tag_info != NULL)
			return tag_info->card_type;

		return "";
	}
#endif

    bool NFCReaderUnit::connect()
    {
		if (isConnected())
		{
			LOG(LogLevel::ERRORS) << EXCEPTION_MSG_CONNECTED;
			disconnect();
		}

		bool connected = d_chip_connected = false;

#ifndef _WIN64
		if (d_insertedChip && d_chips.find(d_insertedChip) != d_chips.end())
		{
			if (d_chips[d_insertedChip].nm.nmt == NMT_ISO14443A)
			{
				nfc_target pnti;
				nfc_modulation modulation;
				modulation.nmt = NMT_ISO14443A;
				modulation.nbr = NBR_106;

				if (nfc_initiator_select_passive_target(d_device, modulation, d_chips[d_insertedChip].nti.nai.abtUid, d_chips[d_insertedChip].nti.nai.szUidLen, &pnti) >= 0)
				{
					d_chip_connected = connected = true;
				}
			}
		}
#endif

		return connected;
    }

    void NFCReaderUnit::disconnect()
    {
#ifndef _WIN64
		if (d_insertedChip && d_chips.find(d_insertedChip) != d_chips.end())
		{
			if (d_chips[d_insertedChip].nm.nmt == NMT_ISO14443A)
			{
				nfc_initiator_deselect_target(d_device);
			}
		}
#endif

		d_chip_connected = false;
    }

    std::shared_ptr<Chip> NFCReaderUnit::createChip(std::string type)
    {
        LOG(LogLevel::INFOS) << "Create chip " << type;
        std::shared_ptr<Chip> chip = ReaderUnit::createChip(type);

		if (chip)
		{
			LOG(LogLevel::INFOS) << "Chip (" << chip->getCardType() << ") created, creating other associated objects...";

			std::shared_ptr<ReaderCardAdapter> rca = getDefaultReaderCardAdapter();
			std::shared_ptr<Commands> commands;
			std::shared_ptr<ResultChecker> resultChecker(new ISO7816ResultChecker()); // default one

			if (type == "Mifare1K" || type == "Mifare4K" || type == "Mifare")
			{
				commands.reset(new MifareNFCCommands());
			}
			else if (type == "DESFireEV1")
			{
				commands.reset(new DESFireEV1ISO7816Commands());
				resultChecker.reset(new DESFireISO7816ResultChecker());
			}
			else if (type == "DESFire")
			{
				commands.reset(new DESFireISO7816Commands());
				resultChecker.reset(new DESFireISO7816ResultChecker());
			}

			if (rca)
			{
				rca->setResultChecker(resultChecker);
				std::shared_ptr<DataTransport> dt = getDataTransport();
				if (dt)
				{
					LOG(LogLevel::INFOS) << "Data transport forced to a specific one.";
					rca->setDataTransport(dt);
				}

				dt = rca->getDataTransport();
				if (dt)
				{
					dt->setReaderUnit(shared_from_this());
				}
			}

			if (commands)
			{
				commands->setReaderCardAdapter(rca);
				commands->setChip(chip);
				chip->setCommands(commands);
			}

			LOG(LogLevel::INFOS) << "Object creation done.";
		}

        return chip;
    }

    std::shared_ptr<Chip> NFCReaderUnit::getSingleChip()
    {
		std::shared_ptr<Chip> chip = d_insertedChip;
		if (!chip)
		{
			std::vector<std::shared_ptr<Chip> > chips = getChipList();
			if (chips.size() > 0)
				chip = chips.front();
		}
        return chip;
    }

    std::vector<std::shared_ptr<Chip> > NFCReaderUnit::getChipList()
    {
		std::vector<std::shared_ptr<Chip> > v;
#ifndef _WIN64
		for (std::map<std::shared_ptr<Chip>, nfc_target>::iterator it = d_chips.begin(); it != d_chips.end(); ++it)
		{
			v.push_back(it->first);
		}
#endif
		return v;
    }

	void NFCReaderUnit::refreshChipList()
	{
#ifndef _WIN64
		nfc_initiator_init(d_device);

		// Drop the field for a while
		nfc_device_set_property_bool(d_device, NP_ACTIVATE_FIELD, false);

		// Configure the CRC and Parity settings
		nfc_device_set_property_bool(d_device, NP_HANDLE_CRC, true);
		nfc_device_set_property_bool(d_device, NP_HANDLE_PARITY, true);
		nfc_device_set_property_bool(d_device, NP_AUTO_ISO14443_4, true);

		// Enable field so more power consuming cards can power themselves up
		nfc_device_set_property_bool(d_device, NP_ACTIVATE_FIELD, true);

		// Poll for a ISO14443A (MIFARE) tag
		nfc_target candidates[MAX_CANDIDATES];
		int candidates_count;
		nfc_modulation modulation;
		modulation.nmt = NMT_ISO14443A;
		modulation.nbr = NBR_106;
		if ((candidates_count = nfc_initiator_list_passive_targets(d_device, modulation, candidates, MAX_CANDIDATES)) < 0)
			throw new LibLogicalAccessException("ISO14443A nfc_initiator_list_passive_targets error");

		for (int c = 0; c < candidates_count; c++)
		{
			std::string ctype = getCardTypeFromTarget(candidates[c]);
			if (ctype != "")
			{
				std::shared_ptr<Chip> chip = createChip(ctype);
				if (chip)
				{
					d_chips[chip] = candidates[c];
				}
			}
		}

		// Poll for a FELICA tag
		modulation.nmt = NMT_FELICA;
		modulation.nbr = NBR_424; // FIXME NBR_212 should also be supported
		if ((candidates_count = nfc_initiator_list_passive_targets(d_device, modulation, candidates, MAX_CANDIDATES)) < 0)
			throw new LibLogicalAccessException("FELICA nfc_initiator_list_passive_targets error");

		for (int c = 0; c < candidates_count; c++)
		{
			std::string ctype = getCardTypeFromTarget(candidates[c]);
			if (ctype != "")
			{
				std::shared_ptr<Chip> chip = createChip(ctype);
				if (chip)
				{
					d_chips[chip] = candidates[c];
				}
			}
		}
#endif
	}

#ifndef _WIN64
	std::vector<unsigned char> NFCReaderUnit::getCardSerialNumber(nfc_target target)
	{
		std::vector<unsigned char> csn;
		char tmp[64];
		memset(tmp, 0x00, sizeof(tmp));

		switch (target.nm.nmt)
		{
		case NMT_FELICA:
			for (size_t i = 0; i < 8; i++)
				csn.push_back(target.nti.nfi.abtId[i]);
			break;
		case NMT_ISO14443A:
			for (size_t i = 0; i < target.nti.nai.szUidLen; i++)
				csn.push_back(target.nti.nai.abtUid[i]);
			break;
		case NMT_DEP:
		case NMT_ISO14443B2CT:
		case NMT_ISO14443B2SR:
		case NMT_ISO14443B:
		case NMT_ISO14443BI:
		case NMT_JEWEL:
			// Cannot determine CSN for these types
			break;
		}

		return csn;
	}
#endif

    std::string NFCReaderUnit::getReaderSerialNumber()
    {
        return "";
    }

	std::shared_ptr<NFCReaderCardAdapter> NFCReaderUnit::getDefaultNFCReaderCardAdapter()
	{
		return std::dynamic_pointer_cast<NFCReaderCardAdapter>(getDefaultReaderCardAdapter());
	}

    bool NFCReaderUnit::isConnected()
    {
        return d_chip_connected;
    }

    bool NFCReaderUnit::connectToReader()
    {
#ifndef _WIN64
		d_device = nfc_open(getNFCReaderProvider()->getContext(), d_name.c_str());
		return (d_device != NULL);
#else
        return false;
#endif
    }

    void NFCReaderUnit::disconnectFromReader()
    {
#ifndef _WIN64
		if (d_device != NULL)
		{
			nfc_close(d_device);
		}
#endif
    }

    void NFCReaderUnit::serialize(boost::property_tree::ptree& parentNode)
    {
		boost::property_tree::ptree node;
		ReaderUnit::serialize(node);
		node.put("Name", d_name);
		parentNode.add_child(getDefaultXmlNodeName(), node);
    }

    void NFCReaderUnit::unSerialize(boost::property_tree::ptree& node)
    {
		d_name = node.get_child("Name").get_value<std::string>();
        ReaderUnit::unSerialize(node);
    }

    std::shared_ptr<NFCReaderProvider> NFCReaderUnit::getNFCReaderProvider() const
    {
        return std::dynamic_pointer_cast<NFCReaderProvider>(getReaderProvider());
    }

	std::shared_ptr<NFCReaderUnit> NFCReaderUnit::createNFCReaderUnit(const std::string& readerName)
	{
		// No dynamic plugin reader instanciation for libnfc as pc/sc for now
		std::shared_ptr<ReaderUnit> reader = std::make_shared<NFCReaderUnit>(readerName);
		return std::dynamic_pointer_cast<NFCReaderUnit>(reader);
	}
}