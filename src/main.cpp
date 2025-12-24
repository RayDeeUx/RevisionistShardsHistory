#include <Geode/modify/GJRewardObject.hpp>

using namespace geode::prelude;

constexpr std::array<std::pair<std::string_view, SpecialRewardItem>, 10> settingsToShards{{
	{"Fire",	SpecialRewardItem::FireShard},
	{"Ice",		SpecialRewardItem::IceShard},
	{"Poison",	SpecialRewardItem::PoisonShard},
	{"Shadow",	SpecialRewardItem::ShadowShard},
	{"Lava",	SpecialRewardItem::LavaShard},
	{"Earth",	SpecialRewardItem::EarthShard},
	{"Blood",	SpecialRewardItem::BloodShard},
	{"Metal",	SpecialRewardItem::MetalShard},
	{"Light",	SpecialRewardItem::LightShard},
	{"Soul",	SpecialRewardItem::SoulShard}
}};

constexpr std::array<std::pair<std::string_view, const char*>, 10> settingsToGSMKeys{{
	{"Fire",	"16"},
	{"Ice",		"17"},
	{"Poison",	"18"},
	{"Shadow",	"19"},
	{"Lava",	"20"},
	{"Earth",	"23"},
	{"Blood",	"24"},
	{"Metal",	"25"},
	{"Light",	"26"},
	{"Soul",	"27"}
}};

constexpr SpecialRewardItem stringToEnum(const std::string_view s) {
	for (auto const &p : settingsToShards) {
		if (p.first == s) return p.second;
	}
	return SpecialRewardItem::FireShard;
}

constexpr const char* stringToConstChar(const std::string_view s) {
	for (auto const &p : settingsToGSMKeys) {
		if (p.first == s) return p.second;
	}
	return "16";
}

class $modify(MyGJRewardObject, GJRewardObject) {
	static GJRewardObject* create(SpecialRewardItem type, int total, int itemID) {
		GJRewardObject* original = GJRewardObject::create(type, total, itemID);
		if (!Mod::get()->getSettingValue<bool>("enabled")) return original;
		if (!original) return original;

		const int rewardType = static_cast<int>(type);
		if (rewardType == 0x6 || rewardType == 0x7 || rewardType == 0x8 || rewardType == 0x9 || rewardType == 0xF) return original;

		const std::string& shardToUse = Mod::get()->getSettingValue<std::string>("shardToUse");
		const int origShardAmt = GameStatsManager::get()->getStat(stringToConstChar(shardToUse));

		SpecialRewardItem newEnum = stringToEnum(shardToUse);
		const int newTotal = (origShardAmt + total > 100) ? 100 - origShardAmt : total;

		GJRewardObject* replacement = GJRewardObject::create(newEnum, newTotal, itemID);
		if (!replacement || newTotal < 1) return original;

		return replacement;
	}
};
