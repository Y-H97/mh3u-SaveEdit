// Unit tests for MH3U_Armory (source/app/MH3U_Save_Editor/mh3u_se.cpp).
//
// MH3U_Armory converts between the raw 16-byte `equipment_t` blobs stored in a
// save file and the typed armor_t / charm_t / weapon_t structures, and maps
// equipment types to subtypes. These are pure, deterministic functions and are
// the highest-value target for unit testing in this (otherwise untested) code
// base.

#include "test_framework.hpp"
#include "mh3u_se.hpp"

using namespace MH3U_Type;

// ---------- convertSubtype ----------

TEST_CASE(subtype_armor_types_map_to_armor)
{
	CHECK_EQ(MH3U_Armory::convertSubtype(ChestType), ArmorSubtype);
	CHECK_EQ(MH3U_Armory::convertSubtype(ArmsType), ArmorSubtype);
	CHECK_EQ(MH3U_Armory::convertSubtype(WaistType), ArmorSubtype);
	CHECK_EQ(MH3U_Armory::convertSubtype(LegsType), ArmorSubtype);
	CHECK_EQ(MH3U_Armory::convertSubtype(HeadType), ArmorSubtype);
}

TEST_CASE(subtype_charm_type_maps_to_charm)
{
	CHECK_EQ(MH3U_Armory::convertSubtype(CharmType), CharmSubtype);
}

TEST_CASE(subtype_weapon_types_map_to_weapon)
{
	CHECK_EQ(MH3U_Armory::convertSubtype(GSType), WeaponSubtype);
	CHECK_EQ(MH3U_Armory::convertSubtype(SNSType), WeaponSubtype);
	CHECK_EQ(MH3U_Armory::convertSubtype(HType), WeaponSubtype);
	CHECK_EQ(MH3U_Armory::convertSubtype(LType), WeaponSubtype);
	CHECK_EQ(MH3U_Armory::convertSubtype(HBGType), WeaponSubtype);
	CHECK_EQ(MH3U_Armory::convertSubtype(LBGType), WeaponSubtype);
	CHECK_EQ(MH3U_Armory::convertSubtype(LSType), WeaponSubtype);
	CHECK_EQ(MH3U_Armory::convertSubtype(SAType), WeaponSubtype);
	CHECK_EQ(MH3U_Armory::convertSubtype(GLType), WeaponSubtype);
	CHECK_EQ(MH3U_Armory::convertSubtype(BowType), WeaponSubtype);
	CHECK_EQ(MH3U_Armory::convertSubtype(DBType), WeaponSubtype);
	CHECK_EQ(MH3U_Armory::convertSubtype(HHType), WeaponSubtype);
}

TEST_CASE(subtype_none_and_unknown_map_to_none)
{
	CHECK_EQ(MH3U_Armory::convertSubtype(NoneType), NoneSubtype);
	CHECK_EQ(MH3U_Armory::convertSubtype(UnknowType), NoneSubtype);
}

TEST_CASE(subtype_uint8_overload_matches_enum_overload)
{
	// The uint8_t overload should delegate to the enum overload.
	CHECK_EQ(MH3U_Armory::convertSubtype((uint8_t) ChestType), ArmorSubtype);
	CHECK_EQ(MH3U_Armory::convertSubtype((uint8_t) CharmType), CharmSubtype);
	CHECK_EQ(MH3U_Armory::convertSubtype((uint8_t) BowType), WeaponSubtype);
	// A value outside the known enum range falls through to NoneSubtype.
	CHECK_EQ(MH3U_Armory::convertSubtype((uint8_t) 200), NoneSubtype);
}

// ---------- armor conversion ----------

TEST_CASE(equipment_to_armor_unpacks_fields)
{
	equipment_t eq = {
		ChestType, // [0] equipmentType
		7,         // [1] upgradeLevel
		0x34,      // [2] identifier low
		0x12,      // [3] identifier high  -> 0x1234
		0x31,      // [4] foo31
		0x11,      // [5] blueComponent
		0x22,      // [6] greenComponent
		0x33,      // [7] redComponent
		0xAA,      // [8] firstJewel low
		0x00,      // [9] firstJewel high  -> 0x00AA
		0xBB,      // [10] secondJewel low
		0x01,      // [11] secondJewel high -> 0x01BB
		0xCC,      // [12] thirdJewel low
		0x02,      // [13] thirdJewel high -> 0x02CC
		0x81,      // [14] foo81
		0x82       // [15] foo82
	};

	armor_t a = MH3U_Armory::convertEquipmentToArmor(eq);

	CHECK_EQ((int) a.equipmentType, (int) ChestType);
	CHECK_EQ((int) a.upgradeLevel, 7);
	CHECK_EQ((int) a.identifier, 0x1234);
	CHECK_EQ((int) a.foo31, 0x31);
	CHECK_EQ((int) a.blueComponent, 0x11);
	CHECK_EQ((int) a.greenComponent, 0x22);
	CHECK_EQ((int) a.redComponent, 0x33);
	CHECK_EQ((int) a.firstJewelIdentifier, 0x00AA);
	CHECK_EQ((int) a.secondJewelIdentifier, 0x01BB);
	CHECK_EQ((int) a.thirdJewelIdentifier, 0x02CC);
	CHECK_EQ((int) a.foo81, 0x81);
	CHECK_EQ((int) a.foo82, 0x82);
}

TEST_CASE(armor_to_equipment_packs_little_endian)
{
	armor_t a;
	a.equipmentType = LegsType;
	a.upgradeLevel = 3;
	a.identifier = 0xABCD;
	a.foo31 = 0x31;
	a.blueComponent = 1;
	a.greenComponent = 2;
	a.redComponent = 3;
	a.firstJewelIdentifier = 0x1122;
	a.secondJewelIdentifier = 0x3344;
	a.thirdJewelIdentifier = 0x5566;
	a.foo81 = 0x81;
	a.foo82 = 0x82;

	equipment_t eq;
	MH3U_Armory::convertArmorToEquipment(a, eq);

	CHECK_EQ((int) eq[0], (int) LegsType);
	CHECK_EQ((int) eq[1], 3);
	CHECK_EQ((int) eq[2], 0xCD); // low byte
	CHECK_EQ((int) eq[3], 0xAB); // high byte
	CHECK_EQ((int) eq[8], 0x22);
	CHECK_EQ((int) eq[9], 0x11);
	CHECK_EQ((int) eq[10], 0x44);
	CHECK_EQ((int) eq[11], 0x33);
	CHECK_EQ((int) eq[12], 0x66);
	CHECK_EQ((int) eq[13], 0x55);
	CHECK_EQ((int) eq[14], 0x81);
	CHECK_EQ((int) eq[15], 0x82);
}

TEST_CASE(armor_roundtrip_is_identity)
{
	equipment_t original = { 1, 9, 0xEF, 0xBE, 5, 6, 7, 8, 0x0D, 0xF0, 0x11, 0x22, 0x33, 0x44, 0x55, 0x66 };

	armor_t a = MH3U_Armory::convertEquipmentToArmor(original);
	equipment_t rebuilt;
	MH3U_Armory::convertArmorToEquipment(a, rebuilt);

	for (int i = 0; i < EQUIPMENT_SIZE; ++i)
	{
		CHECK_EQ((int) rebuilt[i], (int) original[i]);
	}
}

// ---------- charm conversion ----------

TEST_CASE(equipment_to_charm_unpacks_fields)
{
	equipment_t eq = {
		CharmType, // [0]
		3,         // [1] slotsCount
		0x78,      // [2] identifier low
		0x56,      // [3] identifier high -> 0x5678
		0x0A,      // [4] firstSkillIdentifier
		0x0B,      // [5] firstSkillValue
		0x0C,      // [6] secondSkillIdentifier
		0x0D,      // [7] secondSkillValue
		0x01, 0x00, // [8][9] firstJewel  -> 0x0001
		0x02, 0x00, // [10][11] secondJewel -> 0x0002
		0x03, 0x00, // [12][13] thirdJewel -> 0x0003
		0x81, 0x82  // [14][15]
	};

	charm_t c = MH3U_Armory::convertEquipmentToCharm(eq);

	CHECK_EQ((int) c.equipmentType, (int) CharmType);
	CHECK_EQ((int) c.slotsCount, 3);
	CHECK_EQ((int) c.identifier, 0x5678);
	CHECK_EQ((int) c.firstSkillIdentifier, 0x0A);
	CHECK_EQ((int) c.firstSkillValue, 0x0B);
	CHECK_EQ((int) c.secondSkillIdentifier, 0x0C);
	CHECK_EQ((int) c.secondSkillValue, 0x0D);
	CHECK_EQ((int) c.firstJewelIdentifier, 1);
	CHECK_EQ((int) c.secondJewelIdentifier, 2);
	CHECK_EQ((int) c.thirdJewelIdentifier, 3);
	CHECK_EQ((int) c.foo81, 0x81);
	CHECK_EQ((int) c.foo82, 0x82);
}

TEST_CASE(charm_roundtrip_is_identity)
{
	equipment_t original = { 6, 2, 0x01, 0x02, 10, 11, 12, 13, 0x21, 0x43, 0x65, 0x87, 0x09, 0x00, 0xAB, 0xCD };

	charm_t c = MH3U_Armory::convertEquipmentToCharm(original);
	equipment_t rebuilt;
	MH3U_Armory::convertCharmToEquipment(c, rebuilt);

	for (int i = 0; i < EQUIPMENT_SIZE; ++i)
	{
		CHECK_EQ((int) rebuilt[i], (int) original[i]);
	}
}

// ---------- weapon conversion ----------

TEST_CASE(equipment_to_weapon_unpacks_fields)
{
	equipment_t eq = {
		GSType,     // [0]
		0x12,       // [1] foo12
		0xFF, 0x00, // [2][3] identifier -> 0x00FF
		0x31, 0x32, 0x41, 0x42, // [4..7]
		0x10, 0x00, // [8][9] firstJewel -> 0x0010
		0x20, 0x00, // [10][11] secondJewel -> 0x0020
		0x30, 0x00, // [12][13] thirdJewel -> 0x0030
		0x81, 0x82  // [14][15]
	};

	weapon_t w = MH3U_Armory::convertEquipmentToWeapon(eq);

	CHECK_EQ((int) w.equipmentType, (int) GSType);
	CHECK_EQ((int) w.foo12, 0x12);
	CHECK_EQ((int) w.identifier, 0x00FF);
	CHECK_EQ((int) w.foo31, 0x31);
	CHECK_EQ((int) w.foo32, 0x32);
	CHECK_EQ((int) w.foo41, 0x41);
	CHECK_EQ((int) w.foo42, 0x42);
	CHECK_EQ((int) w.firstJewelIdentifier, 0x10);
	CHECK_EQ((int) w.secondJewelIdentifier, 0x20);
	CHECK_EQ((int) w.thirdJewelIdentifier, 0x30);
	CHECK_EQ((int) w.foo81, 0x81);
	CHECK_EQ((int) w.foo82, 0x82);
}

TEST_CASE(weapon_roundtrip_is_identity)
{
	equipment_t original = { 17, 0x99, 0xCD, 0xAB, 1, 2, 3, 4, 0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77, 0x88 };

	weapon_t w = MH3U_Armory::convertEquipmentToWeapon(original);
	equipment_t rebuilt;
	MH3U_Armory::convertWeaponToEquipment(w, rebuilt);

	for (int i = 0; i < EQUIPMENT_SIZE; ++i)
	{
		CHECK_EQ((int) rebuilt[i], (int) original[i]);
	}
}
