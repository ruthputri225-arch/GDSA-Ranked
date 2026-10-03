#include <Geode/Geode.hpp>
#include <Geode/modify/PlayLayer.hpp>
#include <Geode/modify/MenuLayer.hpp>
#include <chrono>
#include <cmath>

using namespace geode::prelude;

// Class Pengelola Data & Logika GDSA Ranked
class GDSARankedManager {
public:
    static GDSARankedManager* get() {
        static GDSARankedManager instance;
        return &instance;
    }

    // Ambil Poin Pemain
    int getPoints() {
        return Mod::get()->getSavedValue<int>("player_points", 0);
    }

    // Tambah Poin Pemain
    void addPoints(int amount) {
        int current = getPoints();
        Mod::get()->setSavedValue<int>("player_points", current + amount);
    }

    // Penentuan Tier Rank Berdasarkan RP (Ranked Points)
    std::string getRankTitle() {
        int pts = getPoints();
        if (pts <= 100) return "Easy";
        if (pts <= 500) return "Medium";
        if (pts <= 1200) return "Hard";
        if (pts <= 3000) return "Insane";
        if (pts <= 6000) return "Hard Demon";
        return "Extreme Demon";
    }

    // Rotasi Season Otomatis Setiap 2 Bulan (60 Hari)
    std::string getCurrentSeason() {
        auto now = std::chrono::system_clock::now().time_since_epoch();
        long long days = std::chrono::duration_cast<std::chrono::hours>(now).count() / 24;
        
        int cycleDay = days % 120;
        if (cycleDay < 60) {
            return "Basic Season (Easy - Insane)";
        } else {
            return "Demon Season (Easy Demon - Extreme Demon)";
        }
    }

    // Rumus Poin: Bintang Level & Jumlah Attempt
    int calculatePoints(int levelStars, int attempts) {
        if (attempts <= 0) attempts = 1;
        
        int basePoints = levelStars * 15;
        // Multiplier: Semakin sedikit attempt, semakin tinggi poinnya
        float multiplier = 10.0f / (9.0f + std::log10((float)attempts));
        
        return static_cast<int>(basePoints * multiplier);
    }
};

// Hook PlayLayer: Deteksi Selesai Level
class $modify(GDSAPlayLayer, PlayLayer) {
    void levelComplete() {
        PlayLayer::levelComplete();

        // Abaikan jika dalam Practice Mode
        if (this->m_isPracticeMode) return;

        GJGameLevel* level = this->m_level;
        int stars = level->m_stars;
        int attempts = this->m_attempts;

        if (stars > 0) {
            int earnedPoints = GDSARankedManager::get()->calculatePoints(stars, attempts);
            GDSARankedManager::get()->addPoints(earnedPoints);

            std::string msg = "GDSA Ranked: +" + std::to_string(earnedPoints) + " RP!";
            Notification::create(msg, NotificationIcon::Success)->show();
        }
    }
};

// UI Custom Pop-up GDSA Ranked
class GDSARankedPopup : public FLAlertLayer {
public:
    static GDSARankedPopup* create() {
        auto ret = new GDSARankedPopup();
        if (ret && ret->init(280.0f, 190.0f, "BG_Square01.png", "GDSA Ranked")) {
            ret->autorelease();
            return ret;
        }
        CC_SAFE_DELETE(ret);
        return nullptr;
    }

    bool init(float w, float h, const char* bg, const char* title) {
        if (!FLAlertLayer::init(150)) return false;

        auto winSize = CCDirector::sharedDirector()->getWinSize();
        
        // Background Pop-up
        auto bgNode = extension::CCScale9Sprite::create(bg);
        bgNode->setContentSize({w, h});
        bgNode->setPosition(winSize / 2);
        this->m_mainLayer->addChild(bgNode);

        // Judul Pop-up
        auto titleLabel = CCLabelBMFont::create(title, "goldFont.fnt");
        titleLabel->setPosition({winSize.width / 2, winSize.height / 2 + 65.0f});
        titleLabel->setScale(0.8f);
        this->m_mainLayer->addChild(titleLabel);

        // Informasi Rank, RP, dan Season
        std::string rankStr = "Rank: " + GDSARankedManager::get()->getRankTitle();
        std::string ptsStr = "Points: " + std::to_string(GDSARankedManager::get()->getPoints()) + " RP";
        std::string seasonStr = GDSARankedManager::get()->getCurrentSeason();

        auto rankLabel = CCLabelBMFont::create(rankStr.c_str(), "bigFont.fnt");
        rankLabel->setPosition({winSize.width / 2, winSize.height / 2 + 25.0f});
        rankLabel->setScale(0.5f);
        this->m_mainLayer->addChild(rankLabel);

        auto ptsLabel = CCLabelBMFont::create(ptsStr.c_str(), "bigFont.fnt");
        ptsLabel->setPosition({winSize.width / 2, winSize.height / 2 - 5.0f});
        ptsLabel->setScale(0.45f);
        this->m_mainLayer->addChild(ptsLabel);

        auto seasonLabel = CCLabelBMFont::create(seasonStr.c_str(), "chatFont.fnt");
        seasonLabel->setPosition({winSize.width / 2, winSize.height / 2 - 35.0f});
        seasonLabel->setScale(0.6f);
        seasonLabel->setColor({255, 200, 0});
        this->m_mainLayer->addChild(seasonLabel);

        // Tombol Close
        auto closeBtnSpr = ButtonSprite::create("OK");
        auto closeBtn = CCMenuItemSpriteExtra::create(
            closeBtnSpr, this, menu_selector(GDSARankedPopup::onClose)
        );
        
        auto menu = CCMenu::create();
        menu->addChild(closeBtn);
        menu->setPosition({winSize.width / 2, winSize.height / 2 - 68.0f});
        this->m_mainLayer->addChild(menu);

        this->setTouchEnabled(true);
        return true;
    }

    void onClose(CCObject* sender) {
        this->keyBackClicked();
    }
};

// Hook MenuLayer: Menambahkan Tombol di Main Menu
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
        auto popup = GDSARankedPopup::create();
        popup->show();
    }
};
