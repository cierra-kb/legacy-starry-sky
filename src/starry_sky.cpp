#include <android/log.h>
#include <cocos2dcpp.h>
#include <vector>

#include "dobby_wrapper/dobby_wrapper.hpp"
#include "consts.hpp"



static DobbyWrapper* hook_manager;

CCRect ccr(const CCPoint& origin, const CCSize& size)
{
    CCRect rect;
    
    rect.setRect(origin.x, origin.y, size.width, size.height);

    return rect;
}

/*
class SpecialLevelEditorLayer;

static SpecialLevelEditorLayer* _current_lel_instance;


class SpecialLevelEditorLayer : public LevelEditorLayer
{
private:
    // the one that moves with you
    CCLayerColor* _static_preview_line;
    // the one that moves on music playback
    CCLayerColor* _preview_line;
    CCSprite* _bg_sprite;
    std::vector<GameObject*> _bg_color_triggers;

    int time_for_x_pos(float x)
    {
        constexpr double BLOCKS_PER_SECOND = SPEEDS[Speed::Normal].time_mod * SPEEDS[Speed::Normal].x_velocity * 60;
        return x / BLOCKS_PER_SECOND;
    }

public:
    static SpecialLevelEditorLayer* create(GJGameLevel* level)
    {
        auto r = new SpecialLevelEditorLayer();

        if (r->init(level))
        {
            r->autorelease();
            _current_lel_instance = r;
            return r;
        }

        delete r;
        return nullptr;
    }

    typedef bool (*init_O_t)(LevelEditorLayer*, GJGameLevel*);
    static init_O_t init_O;

    bool init(GJGameLevel* level)
    {
        if (!init_O(dynamic_cast<LevelEditorLayer*>(this), level))
            return false;
        
        auto win_size = CCDirector::sharedDirector()->getWinSize();
        
        auto static_prev_line = CCLayerColor::create(ccColor4B{255,255,255,125}, 2, win_size.height);
        this->addChild(static_prev_line);
        static_prev_line->setPosition(ccp(win_size.width / 2.5, 0));
        _static_preview_line = static_prev_line;

        auto prev_line = CCLayerColor::create(ccColor4B{0,255,0,125}, 2, win_size.height);
        m_pLayer->addChild(prev_line);
        prev_line->setPosition(m_pLayer->convertToNodeSpace(m_pGrid->convertToWorldSpace(ccp(300, 0)))); 
        _preview_line = prev_line;

        CocosDenshion::SimpleAudioEngine::sharedEngine()->preloadBackgroundMusic(LevelTools::getAudioFileName(member_by_offset<int>(this->m_pLevel, 0x144)));

        _bg_sprite = (CCSprite*) this->getChildren()->objectAtIndex(1);
        _bg_sprite->setColor(ccColor3B { 28, 251, 255});
        this->scheduleUpdate();

        return true;
    }

    bool already_highlighted;
    bool move_prev_line;

    virtual void update(float dt) override
    {
        const uint HALF_BLOCK_UNIT = 15;
        const uint BLOCK_UNIT = HALF_BLOCK_UNIT * 2;
        
        CCRect visible_rect = ccr(m_pLayer->convertToNodeSpace(CCPoint(0,0)), CCDirector::sharedDirector()->getWinSize());

        // handle zooming
        visible_rect.size.width *= 1 / m_pLayer->getScale();
        visible_rect.size.height *= 1 / m_pLayer->getScale();

        // fix blocks disappearing where it shouldn't by extending the visible rect by a few blocks
        visible_rect.origin.x -= (BLOCK_UNIT * 2) + HALF_BLOCK_UNIT;
        visible_rect.origin.y -= (BLOCK_UNIT * 2) + HALF_BLOCK_UNIT;
        
        visible_rect.size.width += BLOCK_UNIT * 5;
        visible_rect.size.height += BLOCK_UNIT * 5;

        if (!already_highlighted)
        {
            auto d = CCLayerColor::create(ccColor4B{255, 0, 0, 125}, visible_rect.size.width - BLOCK_UNIT, visible_rect.size.height - BLOCK_UNIT);
            m_pLayer->addChild(d);
            d->setPosition(visible_rect.origin.x + HALF_BLOCK_UNIT, visible_rect.origin.y + HALF_BLOCK_UNIT);
            already_highlighted = true;
        }
        
        if (move_prev_line)
        {
            auto speed = SPEEDS[Speed::Normal];
            _preview_line->setPositionX(_preview_line->getPositionX() + ((dt * 60) * speed.x_velocity * speed.time_mod));
        }
        //__android_log_print(ANDROID_LOG_DEBUG, "starry_log_sky", "Preview line x-pos: %f", m_pLayer->convertToNodeSpace(_preview_line->getPosition()).x);

        for (uint32_t section_id = 0; section_id < m_obSectionArray->count(); section_id++)
        {
            CCArray* section_objects = (CCArray*) m_obSectionArray->objectAtIndex(section_id);

            for (uint32_t index = 0; index < section_objects->count(); index++)
            {
                GameObject* object = (GameObject*) section_objects->objectAtIndex(index);
                CCPoint object_pos = object->getPosition();

                if (CCRect::CCRectContainsPoint(visible_rect, object_pos))
                {
                    object->setVisible(true);
                }
                else
                {
                    object->setVisible(false);
                }
            }
        }
    }

    typedef void (*addToSection_O_t)(LevelEditorLayer*, GameObject*);
    static addToSection_O_t addToSection_O;

    void addToSection(GameObject* obj)
    {
        //hook_manager->orig<&SpecialLevelEditorLayer::addToSection>(dynamic_cast<LevelEditorLayer*>(this), obj);
        addToSection_O(dynamic_cast<LevelEditorLayer*>(this), obj);


        if (obj->getM_ID() == BG_TRIGGER_ID && std::find(_bg_color_triggers.begin(), _bg_color_triggers.end(), obj) == _bg_color_triggers.end())
        {
            __android_log_print(ANDROID_LOG_DEBUG, "starry_log_sky", "added bg trigger");
            _bg_color_triggers.push_back(obj);
        }
        //__android_log_print(ANDROID_LOG_DEBUG, "starry_log_sky", "Object type: %i; ID: %i; Colour: %hhu %hhu %hhu", obj->getType(), obj->getM_ID(), obj->getTintColor().r,  obj->getTintColor().g,  obj->getTintColor().b);
    }

    void removeObject(GameObject* obj)
    {
        //hook_manager->orig<&SpecialLevelEditorLayer::removeObject>(this, obj);

        if (!obj)
            return;
        
        this->m_obRedoList->removeAllObjects();

        auto v = dynamic_cast<LevelEditorLayer*>(this);

        v->addToUndoList(UndoObject::create(obj, UndoCommand::Delete));
        v->removeObjectFromSection(obj);

        if (obj->getType() == 7 && obj->getShouldSpawn())
        {
            m_pGrid->removeFromEffects(obj);
        };

        obj->removeFromParentAndCleanup(true);
    
        if (obj->getM_ID() == BG_TRIGGER_ID)
        {
            auto it = std::find(_bg_color_triggers.begin(), _bg_color_triggers.end(), obj);

            if (it != _bg_color_triggers.end())
            {
                __android_log_print(ANDROID_LOG_DEBUG, "starry_log_sky", "remove bg trigger");
                _bg_color_triggers.erase(it);
            }
        }
    }
};

SpecialLevelEditorLayer::addToSection_O_t SpecialLevelEditorLayer::addToSection_O = nullptr;
SpecialLevelEditorLayer::init_O_t SpecialLevelEditorLayer::init_O = nullptr;
*/

void EditorUI_setupCreateMenu(EditorUI* self)
{
    CCArray* unk_array = CCArray::create();
    self->m_pUnkArrayOnSetupCreateMenu = unk_array;
    unk_array->retain();

    CCArray* obj_array = CCArray::create();

    const std::vector<std::pair<const char*, int>> object_info = {
        std::make_pair("square_d_01_001.png", 4),
        std::make_pair("square_d_02_001.png", 4),
        std::make_pair("square_d_03_001.png", 4),
        std::make_pair("square_d_04_001.png", 4),
        std::make_pair("square_d_05_001.png", 4),
        std::make_pair("square_d_06_001.png", 4),
        std::make_pair("square_d_07_001.png", 4),
    };

    for (auto obj: object_info)
    {
        obj_array->addObject(self->getCreateBtn(obj.first, obj.second));
    }

    auto director = CCDirector::sharedDirector();
    CCPoint unk_point = ccp(director->getWinSize().width / 2 - 5, 0 + self->m_fEditBarHeight - 6); // y should be from getscreenbottom() - float - 6, but this works fine

    self->m_pEditButtonBarForCreateMenu = EditButtonBar::create(obj_array, unk_point);
    self->addChild(self->m_pEditButtonBarForCreateMenu, 10);
    self->updateCreateMenu();
}
__attribute__((constructor))
void starry_sky()
{
    hook_manager = new DobbyWrapper();

    hook_manager->add_hook(&EditorUI::setupCreateMenu, &EditorUI_setupCreateMenu);

    //hook_manager->add_hook("_ZN16LevelEditorLayer4initEP11GJGameLevel", &SpecialLevelEditorLayer::init, &SpecialLevelEditorLayer::init_O);
    //hook_manager->add_hook("_ZN16LevelEditorLayer6createEP11GJGameLevel", &SpecialLevelEditorLayer::create);


    //hook_manager->add_hook(&LevelEditorLayer::addToSection, &SpecialLevelEditorLayer::addToSection, &SpecialLevelEditorLayer::addToSection_O);
    //hook_manager->add_hook(&LevelEditorLayer::removeObject, &SpecialLevelEditorLayer::removeObject);

    //hook_manager->add_hook(&EditorPauseLayer::customSetup, &EditorPauseLayer_customSetup);
    //hook_manager->add_hook("_ZN16EditorPauseLayer6createEP16LevelEditorLayer", &EditorPauseLayer_create);
    hook_manager->install_hooks();
}
