#include <Geode/modify/GameStatsManager.hpp>

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
			if (!obj->isSpecialType() && static_cast<int>(obj->m_specialRewardItem) != 7 && static_cast<int>(obj->m_specialRewardItem) != 8) continue;
			if (!Mod::get()->getSettingValue<bool>("overrideDiamondsAndOrbs")) continue;

			const std::string& shardToUse = Mod::get()->getSettingValue<std::string>("shardToUse");
			const std::string_view shardOriginally = enumToString(obj->m_specialRewardItem);
			if (shardToUse == shardOriginally) continue;

			log::info("originally rewarding {} of shard type {} shard", obj->m_total, shardOriginally);

			const int shardCount = GameStatsManager::get()->getStat(stringToConstChar(shardToUse));
			if (shardCount > 99) continue;

			obj->m_specialRewardItem = stringToEnum(shardToUse);
			if (shardCount + obj->m_total > 99) obj->m_total = 100 - shardCount;

			log::info("now rewarding {} of shard type {} shard", obj->m_total, shardToUse);
		}
		GameStatsManager::registerRewardsFromItem(item);
	}
};
