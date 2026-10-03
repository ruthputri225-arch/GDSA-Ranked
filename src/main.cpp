#include <Geode/Geode.hpp>
#include <Geode/modify/PlayLayer.hpp>
#include <Geode/modify/MenuLayer.hpp>

using namespace geode::prelude;

// Hook PlayLayer: Tambah RP saat tamat level
class $modify(GDSAPlayLayer, PlayLayer) {
    void levelComplete() {
        PlayLayer::levelComplete();

        if (this->m_isPracticeMode) return;

        auto level = this->m_level;
        if (level && level->m_stars > 0) {
            int stars = level->m_stars;
            int attempts = this->m_attempts > 0 ? this->m_attempts : 1;
            
            // Hitung poin sederhana
            int earnedRP = stars * 15;
            int currentRP = Mod::get()->getSavedValue<int>("player_points", 0);
            Mod::get()->setSavedValue<int>("player_points", currentRP + earnedRP);

            std::string msg = "GDSA Ranked: +" + std::to_string(earnedRP) + " RP!";
            Notification::create(msg, NotificationIcon::Success)->show();
        }
    }
};

// Hook MenuLayer: Tombol & Popup Rank di Main Menu
class $modify(GDSAMenuLayer, MenuLayer) {
    bool init() {
        if (!MenuLayer::init()) return false;

        auto btnSpr = CircleButtonSprite::createWithSpriteFrameName(
            "GJ_timeBtn_001.png", 1.0f, CircleBaseColor::Green, CircleBaseSize::Medium
        );

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

        std::string content = "Rank: " + rankStr + "\nPoints: " + std::to_string(pts) + " RP";
        
        // Popup resmi bawaan Geode (anti-crash)
        FLAlertLayer::create("GDSA Ranked", content, "OK")->show();
    }
};
