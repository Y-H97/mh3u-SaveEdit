// Unit tests for MH3U_DS (source/app/MH3U_Save_Editor/mh3u_ds.cpp).
//
// MH3U_DS loads the localized reference data (item/skill/armor/... name lists)
// from disk into static datasets. In the (non-DEBUG) build it reads files from
// "../../../../data/<lang>/<name>.txt" relative to the current working
// directory. To test it hermetically we build a temporary data tree, chdir into
// a 4-level-deep directory so the relative path resolves, and drive the public
// readData / accessor / deleteData API.
//
// MH3U_DS is entirely static, so the full lifecycle is exercised inside a single
// test case to keep the shared state deterministic.

#include "test_framework.hpp"
#include "mh3u_ds.hpp"

#include <cstdlib>
#include <fstream>
#include <string>
#include <vector>
#include <unistd.h>

namespace
{
	void writeRaw(const std::string& path, const std::string& content)
	{
		std::ofstream fs(path.c_str(), std::ios::out | std::ios::binary);
		fs.write(content.data(), (std::streamsize) content.size());
	}

	// Writes every data file MH3U_DS::readData expects into <dir> (the language
	// folder). Most files get generic content; a couple use specific content so
	// we can assert on the parsing.
	void writeLangData(const std::string& dir)
	{
		const char* names[] = {
			"faces", "hairs", "sexs", "voices", "items", "skills", "jewels",
			"equipment_types", "chest_armors", "arms_armors", "waist_armors",
			"legs_armors", "head_armors", "charms", "gs_weapons", "sns_weapons",
			"h_weapons", "l_weapons", "hbg_weapons", "lbg_weapons", "ls_weapons",
			"sa_weapons", "gl_weapons", "bow_weapons", "db_weapons", "hh_weapons"
		};
		for (size_t i = 0; i < sizeof(names) / sizeof(names[0]); ++i)
		{
			writeRaw(dir + "/" + names[i] + ".txt", "X\nY");
		}
	}
}

TEST_CASE(ds_full_lifecycle)
{
	char cwdBuf[4096];
	CHECK(getcwd(cwdBuf, sizeof(cwdBuf)) != NULL);
	std::string originalCwd = cwdBuf;

	// Build: <root>/deep/a/b/c  and  <root>/data/{en,fr}
	// so that from .../deep/a/b/c the path "../../../../data" == <root>/data.
	std::string root = originalCwd + "/mh3u_ds_tmp";
	std::system(("rm -rf " + root).c_str());
	CHECK(std::system(("mkdir -p " + root + "/deep/a/b/c").c_str()) == 0);
	CHECK(std::system(("mkdir -p " + root + "/data/en").c_str()) == 0);
	CHECK(std::system(("mkdir -p " + root + "/data/fr").c_str()) == 0);

	writeLangData(root + "/data/en");
	writeLangData(root + "/data/fr");

	// Specific content to assert the parser behaviour precisely.
	// No trailing newline -> exactly two entries.
	writeRaw(root + "/data/en/faces.txt", "AAA\nBBB");
	// CRLF line endings: the '\r' is retained in the identifier (documents the
	// getline-on-'\n' behaviour of MH3U_DS::readFile).
	writeRaw(root + "/data/en/sexs.txt", "Male\r\nFemale");
	writeRaw(root + "/data/fr/faces.txt", "CCC\nDDD\nEEE");

	CHECK(chdir((root + "/deep/a/b/c").c_str()) == 0);

	// Initial state.
	CHECK_EQ((int) MH3U_DS::lang(), (int) LANG_NONE);
	// LANG_NONE is a no-op and returns false.
	CHECK(!MH3U_DS::readData(LANG_NONE));
	CHECK_EQ((int) MH3U_DS::lang(), (int) LANG_NONE);

	// Load English.
	CHECK(MH3U_DS::readData(LANG_EN));
	CHECK_EQ((int) MH3U_DS::lang(), (int) LANG_EN);

	const dataset_t* faces = MH3U_DS::faces();
	CHECK(faces != NULL);
	if (faces)
	{
		CHECK_EQ((int) faces->size(), 2);
		CHECK((*faces)[0].identifier == std::string("AAA"));
		CHECK((*faces)[1].identifier == std::string("BBB"));
		CHECK_EQ((unsigned long) (*faces)[0].count, 1UL);
		CHECK_EQ((unsigned long) (*faces)[1].count, 2UL);
	}

	const dataset_t* sexs = MH3U_DS::sexs();
	CHECK(sexs != NULL);
	if (sexs)
	{
		CHECK_EQ((int) sexs->size(), 2);
		// "Male\r" -> 5 chars including the retained carriage return.
		CHECK_EQ((int) (*sexs)[0].identifier.size(), 5);
		CHECK((*sexs)[1].identifier == std::string("Female"));
	}

	// A generic dataset is still populated ("X", "Y").
	const dataset_t* items = MH3U_DS::items();
	CHECK(items != NULL);
	if (items) CHECK_EQ((int) items->size(), 2);

	// Every other accessor is non-null after a successful load.
	CHECK(MH3U_DS::hairs() != NULL);
	CHECK(MH3U_DS::voices() != NULL);
	CHECK(MH3U_DS::skills() != NULL);
	CHECK(MH3U_DS::jewels() != NULL);
	CHECK(MH3U_DS::equipmentTypes() != NULL);
	CHECK(MH3U_DS::charms() != NULL);
	CHECK(MH3U_DS::hhWeapons() != NULL);

	// Re-loading the same language is a no-op (returns false, keeps data).
	CHECK(!MH3U_DS::readData(LANG_EN));
	CHECK_EQ((int) MH3U_DS::lang(), (int) LANG_EN);

	// Switch to French: data is reloaded.
	CHECK(MH3U_DS::readData(LANG_FR));
	CHECK_EQ((int) MH3U_DS::lang(), (int) LANG_FR);
	const dataset_t* frFaces = MH3U_DS::faces();
	CHECK(frFaces != NULL);
	if (frFaces) CHECK_EQ((int) frFaces->size(), 3);

	// Cleanup of the datasets succeeds.
	CHECK(MH3U_DS::deleteData());

	// Restore cwd and remove temp tree.
	CHECK(chdir(originalCwd.c_str()) == 0);
	std::system(("rm -rf " + root).c_str());
}
