#include <Geode/Geode.hpp>
#include <Geode/modify/PlayLayer.hpp>
#include <Geode/modify/MenuLayer.hpp>

using namespace geode::prelude;

// Tambah RP otomatis saat level selesai
class $modify(GDSAPlayLayer, PlayLayer) {
    void levelComplete() {
        PlayLayer::levelComplete();

        if (this->m_isPracticeMode) return;

        auto level = this->m_level;
        if (level && level->m_stars > 0) {
            int stars = level->m_stars;
            int currentRP = Mod::get()->getSavedValue<int>("player_points", 0);
            int earnedRP = stars * 15;
            Mod::get()->setSavedValue<int>("player_points", currentRP + earnedRP);

            std::string msg = "GDSA Ranked: +" + std::to_string(earnedRP) + " RP!";
            Notification::create(msg, NotificationIcon::Success)->show();
        }
    }
};

// Tombol Cek RP di Menu Utama
class $modify(GDSAMenuLayer, MenuLayer) {
    bool init() {
        if (!MenuLayer::init()) return false;

        auto btnSpr = CCSprite::createWithSpriteFrameName("GJ_timeBtn_001.png");
        if (!btnSpr) {
            btnSpr = CCSprite::create();
        }

        auto rankBtn = CCMenuItemSpriteExtra::create(
            btnSpr, this, menu_selector(GDSAMenuLayer::onGDSARankedBtn)
        );

        auto menu = this->getChildByID("right-side-menu");
        if (menu) {
            menu->addChild(rankBtn);
            menu->updateLayout();
        }

        return true;
    }

    void onGDSARankedBtn(CCObject* sender) {
        int pts = Mod::get()->getSavedValue<int>("player_points", 0);
        
        std::string rankStr = "Easy";
        if (pts > 100) rankStr = "Medium";
        if (pts > 500) rankStr = "Hard";
        if (pts > 1200) rankStr = "Insane";
        if (pts > 3000) rankStr = "Hard Demon";
        if (pts > 6000) rankStr = "Extreme Demon";

        std::string content = "Rank: " + rankStr + " | Total: " + std::to_string(pts) + " RP";
        Notification::create(content, NotificationIcon::Info)->show();
    }
};
