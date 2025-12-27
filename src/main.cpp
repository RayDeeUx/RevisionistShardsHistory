#include <Geode/modify/GameStatsManager.hpp>
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

constexpr std::array<std::pair<SpecialRewardItem, const char*>, 10> enumToGSMKeys{{
	{SpecialRewardItem::FireShard,		"16"},
	{SpecialRewardItem::IceShard,		"17"},
	{SpecialRewardItem::PoisonShard,	"18"},
	{SpecialRewardItem::ShadowShard,	"19"},
	{SpecialRewardItem::LavaShard,		"20"},
	{SpecialRewardItem::EarthShard,		"23"},
	{SpecialRewardItem::BloodShard,		"24"},
	{SpecialRewardItem::MetalShard,		"25"},
	{SpecialRewardItem::LightShard,		"26"},
	{SpecialRewardItem::SoulShard,		"27"}
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

constexpr const char* enumToConstChar(const SpecialRewardItem s) {
	for (auto const &p : enumToGSMKeys) {
		if (p.first == s) return p.second;
	}
	return "16";
}

constexpr std::string_view enumToString(const SpecialRewardItem v) {
	for (auto const &p : settingsToShards) {
		if (p.second == v) return p.first;
	}
	return {};
}

class $modify(MyGameStatsManager, GameStatsManager) {
	void registerRewardsFromItem(GJRewardItem* item) {
		if (!Mod::get()->getSettingValue<bool>("enabled")) return GameStatsManager::registerRewardsFromItem(item);
		for (GJRewardObject* obj : CCArrayExt<GJRewardObject*>(item->m_rewardObjects)) {
			if (!obj->isSpecialType()) continue;
			log::info("originally rewarding {} of shard type {} shard", obj->m_total, enumToString(obj->m_specialRewardItem));

			const int shardCount = GameStatsManager::get()->getStat(enumToConstChar(obj->m_specialRewardItem));
			if (shardCount > 99) continue;

			const std::string& shardToUse = Mod::get()->getSettingValue<std::string>("shardToUse");
			obj->m_specialRewardItem = stringToEnum(shardToUse);
			if (shardCount + obj->m_total > 99) obj->m_total = 100 - shardCount;

			log::info("now rewarding {} of shard type {} shard", obj->m_total, shardToUse);
		}
		GameStatsManager::registerRewardsFromItem(item);
	}
};

class $modify(MyGJRewardObject, GJRewardObject) {
	static GJRewardObject* create(SpecialRewardItem type, int total, int itemID) {
		GJRewardObject* original = GJRewardObject::create(type, total, itemID);
		if (!Mod::get()->getSettingValue<bool>("enabled")) return original;
		if (!original) return original;

		log::info("original is not nullptr");

		const int rewardType = static_cast<int>(type);
		if (rewardType == 0x6 || rewardType == 0x7 || rewardType == 0x8 || rewardType == 0x9 || rewardType == 0xF) return original;

		log::info("rewardType is a shard");

		const std::string& shardToUse = Mod::get()->getSettingValue<std::string>("shardToUse");
		const int origShardAmt = GameStatsManager::get()->getStat(stringToConstChar(shardToUse));

		SpecialRewardItem newEnum = stringToEnum(shardToUse);
		const int newTotal = (origShardAmt + total > 100) ? 100 - origShardAmt : total;

		log::info("giving {}x {} shards", newTotal, shardToUse);

		GJRewardObject* replacement = GJRewardObject::create(newEnum, newTotal, itemID);
		if (!replacement || newTotal < 1) return original;

		return replacement;
	}
};
