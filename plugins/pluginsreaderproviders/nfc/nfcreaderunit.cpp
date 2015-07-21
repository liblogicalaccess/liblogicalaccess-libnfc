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
#include "nfcreaderunit.hpp"

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
        else
        {
            THROW_EXCEPTION_WITH_LOG(LibLogicalAccessException,
                                     "No underlying libnfc reader associated with this object.");
        }
#endif
        return inserted;
    }

    bool NFCReaderUnit::waitRemoval(unsigned int maxwait)
    {    
        LOG(DEBUGS) << "Waiting for card removal.";
        std::chrono::steady_clock::time_point wait_until(std::chrono::steady_clock::now()
                                                         + std::chrono::milliseconds(maxwait));
        bool removed = false;

		if (d_insertedChip)
		{
            LogDisabler ld;
#ifndef _WIN64
			if (d_chips.find(d_insertedChip) != d_chips.end())
			{
                // We check whether we can connect to a card or not.

				while (!removed && (std::chrono::steady_clock::now() < wait_until || maxwait == 0))
				{
                    // Call to nfc_initiator_deselect_target() (in disconnect()) causes
                    // nfc_initiator_target_is_present() to return false.
                    /*
                    nfc_target target = d_chips[d_insertedChip];
					removed = (nfc_initiator_target_is_present(d_device, &target) != NFC_SUCCESS);
                    */

                    // We attempt to connect. If we failed to connect, that means the card
                    // has been removed.
                    removed = !connect();
					if (!removed)
					{
						std::this_thread::sleep_for(std::chrono::milliseconds(50));
					}
                    else
                    {
                        d_chips.clear();
                        d_insertedChip = nullptr;
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

                // prevent infinite wait. Setting this in connectToReader() did not work
                // for unkown reason.
				nfc_safe_call(nfc_device_set_property_bool, d_device, NP_INFINITE_SELECT, false);

                int ret = nfc_initiator_select_passive_target(d_device,
                                                              modulation,
                                                              d_chips[d_insertedChip].nti.nai.abtUid,
                                                              d_chips[d_insertedChip].nti.nai.szUidLen,
                                                              &pnti);
                if (ret > 0)
				{
                    LOG(DEBUGS) << "Selected passive target.";
					d_chip_connected = connected = true;
                    d_insertedChip->setChipIdentifier(getCardSerialNumber(d_chips[d_insertedChip]));
				}
                else if (ret == 0)
                {
                    LOG(DEBUGS) << "No target found when selecting passive NFC target.";
                }
                else
                {
                    LOG(ERRORS) << "NFC Error: " << nfc_strerror(d_device);
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
                LOG(DEBUGS) << "Deselecting target";
				nfc_initiator_deselect_target(d_device);
                LOG(DEBUGS) << "Target deselected";
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

		nfc_safe_call(nfc_initiator_init, d_device);

		// Drop the field for a while
		nfc_safe_call(nfc_device_set_property_bool, d_device, NP_ACTIVATE_FIELD, false);

		// Configure the CRC and Parity settings
		nfc_safe_call(nfc_device_set_property_bool, d_device, NP_HANDLE_CRC, true);
		nfc_safe_call(nfc_device_set_property_bool, d_device, NP_HANDLE_PARITY, true);
		nfc_safe_call(nfc_device_set_property_bool, d_device, NP_AUTO_ISO14443_4, true);

		// Enable field so more power consuming cards can power themselves up
		nfc_safe_call(nfc_device_set_property_bool, d_device, NP_ACTIVATE_FIELD, true);

		// Poll for a ISO14443A (MIFARE) tag
		nfc_target candidates[MAX_CANDIDATES];
		int candidates_count;
		nfc_modulation modulation;
		modulation.nmt = NMT_ISO14443A;
		modulation.nbr = NBR_106;
		if ((candidates_count = nfc_initiator_list_passive_targets(d_device, modulation, candidates, MAX_CANDIDATES)) < 0)
			throw LibLogicalAccessException("ISO14443A nfc_initiator_list_passive_targets error");

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
			throw LibLogicalAccessException("FELICA nfc_initiator_list_passive_targets error");

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
       LOG(INFOS) << "Attempting to connect to NFC reader \"" << d_name << "\"";
        if (d_name.empty())
        {
            d_device = nfc_open(getNFCReaderProvider()->getContext(), nullptr);
        }
        else
        {
            d_device = nfc_open(getNFCReaderProvider()->getContext(), d_name.c_str());
        }
		if (d_device == nullptr)
		{
			LOG(ERRORS) << "Failed to instanciate NFC device: " << std::string(nfc_strerror(d_device));
		}
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
            d_device = nullptr;
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

    std::vector<unsigned char> NFCReaderUnit::getNumber(std::shared_ptr<Chip> chip)
    {
#ifndef _WIN64
        if (d_chips.count(chip))
        {
            return getCardSerialNumber(d_chips.at(chip));
        }
#endif 
        return ReaderUnit::getNumber(chip);
    }

    std::vector<uint8_t> NFCReaderUnit::transmitBits(const uint8_t *pbtTx, const size_t szTxBits)
	{
#ifndef _WIN64
        const int MAX_FRAME_LEN = 264;
        uint8_t abtRx[MAX_FRAME_LEN];
        int szRxBits;

        // Transmit the bit frame command, we don't use the arbitrary parity feature
        if ((szRxBits = nfc_initiator_transceive_bits(d_device, pbtTx, szTxBits, NULL, abtRx, sizeof(abtRx), NULL)) < 0)
        {
            LOG(ERRORS) << "NFC write bits error: " << std::string(nfc_strerror(d_device));
            THROW_EXCEPTION_WITH_LOG(LibLogicalAccessException,
                                     "Writing bits to NFC reader failed: " + std::string(nfc_strerror(d_device)));
        }

        return std::vector<uint8_t>(std::begin(abtRx), std::begin(abtRx) + szRxBits / 8 + 1);
#endif 
		return{};
    }

    void NFCReaderUnit::writeChipUid(std::shared_ptr<Chip> c,
                                     const std::vector<uint8_t> &new_uid)
	{
#ifndef _WIN64
        WriteUIDConfigGuard config_guard(*this);
        assert(new_uid.size() == 4);
        LOG(DEBUGS) << "Attempting to change the UID of a card. "
                "This will work only on some non-original \"backup card\"";

        std::vector<uint8_t> abtHalt = {0x50, 0x00, 0x00, 0x00};

        // special unlock command
        uint8_t abtUnlock1[1] = {0x40};
        std::vector<uint8_t> abtUnlock2 = {0x43};
        std::vector<uint8_t> abtWrite = {0xa0, 0x00, 0x5f, 0xb1};
        std::vector<uint8_t> abtData = {0x01, 0x23, 0x45, 0x67, 0x00, 0x08, 0x04, 0x00, 0x46, 0x59, 0x25, 0x58, 0x49, 0x10, 0x23, 0x02, 0x23, 0xeb};

        iso14443a_crc_append(&abtHalt[0], 2);
        getDefaultNFCReaderCardAdapter()->sendCommand(abtHalt);

        // write user-provided UID and compute the checksum
        std::copy(new_uid.begin(), new_uid.end(), abtData.begin());
        abtData[4] = abtData[0] ^ abtData[1] ^ abtData[2] ^ abtData[3];
        iso14443a_crc_append(&abtData[0], 16);

        transmitBits(abtUnlock1, 7);
        getDefaultNFCReaderCardAdapter()->sendCommand(abtUnlock2);
        getDefaultNFCReaderCardAdapter()->sendCommand(abtWrite);
        getDefaultNFCReaderCardAdapter()->sendCommand(abtData);

        // if we reach this point, the UID was changed. However since error are
        // disabled, we a are very likely to reach this point...
        // We need to fix up our internals to match the new UID.
        nfc_target nfct = d_chips[c];
        for (int i = 0; i < 4; ++i)
            nfct.nti.nai.abtUid[i] = new_uid[i];
        d_chips[c] = nfct;
        c->setChipIdentifier(new_uid);
#endif
    }

    NFCReaderUnit::WriteUIDConfigGuard::WriteUIDConfigGuard(NFCReaderUnit &ru) :
            ru_(ru)
    {
#ifndef _WIN64
        int ret;
        // Configure the CRC
        ret = nfc_device_set_property_bool(ru.getDevice(), NP_HANDLE_CRC, false);
        assert(ret >= 0);
        // Use raw send/receive methods
        ret = nfc_device_set_property_bool(ru.getDevice(), NP_EASY_FRAMING, false);
        assert(ret >= 0);
        // Disable 14443-4 autoswitching
        ret = nfc_device_set_property_bool(ru.getDevice(), NP_AUTO_ISO14443_4, false);
        assert(ret >= 0);

        // Disable all error checking. This is required for changing the UID.
        rca_error_flag_ = ru.getDefaultNFCReaderCardAdapter()->ignoreAllError(true);
        dt_error_flag_ = std::dynamic_pointer_cast<NFCDataTransport>(ru.getDefaultNFCReaderCardAdapter()->getDataTransport())
                ->ignoreAllError(true);
#endif
	}

    NFCReaderUnit::WriteUIDConfigGuard::~WriteUIDConfigGuard()
	{
#ifndef _WIN64
        int ret;
        // Revert the various flag to their default value
        ret = nfc_device_set_property_bool(ru_.getDevice(), NP_HANDLE_CRC, true);
        assert(ret >= 0);
        ret = nfc_device_set_property_bool(ru_.getDevice(), NP_EASY_FRAMING, true);
        assert(ret >= 0);
        ret = nfc_device_set_property_bool(ru_.getDevice(), NP_AUTO_ISO14443_4, true);
        assert(ret >= 0);

        ru_.getDefaultNFCReaderCardAdapter()->ignoreAllError(rca_error_flag_);
        std::dynamic_pointer_cast<NFCDataTransport>(ru_.getDefaultNFCReaderCardAdapter()->getDataTransport())
                ->ignoreAllError(dt_error_flag_);
#endif
    }
}
