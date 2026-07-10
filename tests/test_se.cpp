// Unit tests for MH3U_SE (source/app/MH3U_Save_Editor/mh3u_se.cpp).
//
// MH3U_SE loads a raw MH3U save file into a typed `save_t` structure and writes
// it back out. These tests build a synthetic, correctly-sized save buffer,
// exercise the load path (offset parsing), and verify a load -> save -> reload
// round-trip preserves both untouched bytes and in-memory edits.
//
// NOTE: multi-byte scalar fields (money, mogapoint) are read/written as raw
// memory, so the exact numeric assertions below assume a little-endian host
// (the CI/dev target). The byte-level and round-trip assertions are
// endianness-independent.

#include "test_framework.hpp"
#include "mh3u_se.hpp"

#include <cstdio>
#include <cstring>
#include <fstream>
#include <string>
#include <vector>

namespace
{
	// A distinct temp path per test to avoid collisions.
	std::string tmpPath(const char* tag)
	{
		std::string p = "mh3u_test_";
		p += tag;
		p += ".sav";
		return p;
	}

	std::vector<uint8_t> makeSaveBuffer()
	{
		std::vector<uint8_t> buf(SAVEFILE_SIZE, 0);

		buf[SEX_OFFSET] = 1;
		buf[FACE_OFFSET] = 2;
		buf[HAIR_OFFSET] = 3;

		const char* name = "HUNTER";
		std::memcpy(&buf[NAME_OFFSET], name, std::strlen(name));

		// money = 123456 = 0x0001E240 (little-endian bytes).
		buf[MONEY_OFFSET + 0] = 0x40;
		buf[MONEY_OFFSET + 1] = 0xE2;
		buf[MONEY_OFFSET + 2] = 0x01;
		buf[MONEY_OFFSET + 3] = 0x00;

		buf[VOICE_OFFSET] = 5;

		// inventory[0][0]: id = 0x0102, count = 0x0304 (little-endian).
		buf[INVENTORY_OFFSET + 0] = 0x02;
		buf[INVENTORY_OFFSET + 1] = 0x01;
		buf[INVENTORY_OFFSET + 2] = 0x04;
		buf[INVENTORY_OFFSET + 3] = 0x03;

		// A byte in the equipment box region (verifies the box copy path).
		buf[BOX_OFFSET] = 0x07;
		buf[BOX_OFFSET + 15] = 0x77;

		// mogapoint = 999 = 0x000003E7.
		buf[MOGAPOINT_OFFSET + 0] = 0xE7;
		buf[MOGAPOINT_OFFSET + 1] = 0x03;
		buf[MOGAPOINT_OFFSET + 2] = 0x00;
		buf[MOGAPOINT_OFFSET + 3] = 0x00;

		return buf;
	}

	bool writeFile(const std::string& path, const std::vector<uint8_t>& buf)
	{
		std::ofstream fs(path.c_str(), std::ios::out | std::ios::binary);
		if (!fs) return false;
		fs.write((const char*)&buf[0], (std::streamsize) buf.size());
		return fs.good();
	}

	std::vector<uint8_t> readFile(const std::string& path)
	{
		std::ifstream fs(path.c_str(), std::ios::in | std::ios::binary);
		std::vector<uint8_t> out;
		if (!fs) return out;
		char c;
		while (fs.get(c)) out.push_back((uint8_t) c);
		return out;
	}
}

TEST_CASE(loaded_is_false_before_any_load)
{
	MH3U_SE se;
	CHECK(!se.loaded());
	CHECK(se.savedata == NULL);
}

TEST_CASE(load_missing_file_returns_false)
{
	MH3U_SE se;
	CHECK(!se.load("this_file_does_not_exist_12345.sav"));
	CHECK(!se.loaded());
}

TEST_CASE(load_parses_all_offsets)
{
	std::string path = tmpPath("load");
	CHECK(writeFile(path, makeSaveBuffer()));

	MH3U_SE se;
	CHECK(se.load(path));
	CHECK(se.loaded());

	CHECK_EQ((int) se.savedata->sex, 1);
	CHECK_EQ((int) se.savedata->face, 2);
	CHECK_EQ((int) se.savedata->hair, 3);
	CHECK_EQ((int) se.savedata->voice, 5);
	CHECK_EQ((unsigned long) se.savedata->money, 123456UL);
	CHECK_EQ((unsigned long) se.savedata->mogapoint, 999UL);

	CHECK_EQ((int) se.savedata->name[0], (int) 'H');
	CHECK_EQ((int) se.savedata->name[5], (int) 'R');

	CHECK_EQ((int) se.savedata->inventory[0][0].id, 0x0102);
	CHECK_EQ((int) se.savedata->inventory[0][0].count, 0x0304);

	CHECK_EQ((int) se.savedata->box[0][0][0], 0x07);
	CHECK_EQ((int) se.savedata->box[0][0][15], 0x77);

	std::remove(path.c_str());
}

TEST_CASE(save_roundtrip_preserves_bytes)
{
	std::string in = tmpPath("rt_in");
	std::string out = tmpPath("rt_out");
	std::vector<uint8_t> original = makeSaveBuffer();
	CHECK(writeFile(in, original));

	MH3U_SE se;
	CHECK(se.load(in));
	CHECK(se.save(out));

	std::vector<uint8_t> written = readFile(out);
	CHECK_EQ((int) written.size(), (int) SAVEFILE_SIZE);
	if (written.size() == (size_t) SAVEFILE_SIZE)
	{
		bool identical = true;
		for (size_t i = 0; i < (size_t) SAVEFILE_SIZE; ++i)
		{
			if (written[i] != original[i]) { identical = false; break; }
		}
		CHECK(identical);
	}

	std::remove(in.c_str());
	std::remove(out.c_str());
}

TEST_CASE(edits_are_written_back)
{
	std::string in = tmpPath("edit_in");
	std::string out = tmpPath("edit_out");
	CHECK(writeFile(in, makeSaveBuffer()));

	MH3U_SE se;
	CHECK(se.load(in));

	se.savedata->money = 42;
	se.savedata->sex = 0;
	se.savedata->inventory[0][0].count = 65;
	CHECK(se.save(out));

	MH3U_SE reloaded;
	CHECK(reloaded.load(out));
	CHECK_EQ((unsigned long) reloaded.savedata->money, 42UL);
	CHECK_EQ((int) reloaded.savedata->sex, 0);
	CHECK_EQ((int) reloaded.savedata->inventory[0][0].count, 65);
	// Untouched field survives the round-trip.
	CHECK_EQ((unsigned long) reloaded.savedata->mogapoint, 999UL);

	std::remove(in.c_str());
	std::remove(out.c_str());
}

TEST_CASE(save_without_argument_uses_loaded_filename)
{
	std::string in = tmpPath("noarg");
	CHECK(writeFile(in, makeSaveBuffer()));

	MH3U_SE se;
	CHECK(se.load(in));
	se.savedata->hair = 9;
	// No output path -> writes back to the file it was loaded from.
	CHECK(se.save());

	MH3U_SE reloaded;
	CHECK(reloaded.load(in));
	CHECK_EQ((int) reloaded.savedata->hair, 9);

	std::remove(in.c_str());
}

TEST_CASE(save_before_load_returns_false)
{
	// writeBuffer() returns false when there is no savedata.
	MH3U_SE se;
	CHECK(!se.save(tmpPath("never_written")));
}
