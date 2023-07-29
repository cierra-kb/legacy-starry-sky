#include <cocos2dcpp.h>
#include "dobby_wrapper/dobby_wrapper.hpp"

#include <android/log.h>

static DobbyWrapper* hook_manager;

class LoadingLayer
{
public:
    const char* getLoadingString();
};

const char* ll(LoadingLayer* self)
{
    return "hook test";
}


CCRect ccr(const CCPoint& origin, const CCSize& size)
{
    CCRect rect;
    
    rect.setRect(origin.x, origin.y, size.width, size.height);

    return rect;
}

class SpecialLevelEditorLayer : public LevelEditorLayer
{
public:
    GameObject* subj;
    CCLayerColor* highlight;
    CCSize win_size;
    
    static SpecialLevelEditorLayer* create(GJGameLevel* level)
    {
        auto r = new SpecialLevelEditorLayer();

        if (r->init(level))
        {
            r->autorelease();
            return r;
        }

        delete r;
        return nullptr;
    }

    bool init(GJGameLevel* level)
    {
        if (!hook_manager->orig<&SpecialLevelEditorLayer::init>(this, level))
            return false;
        
        this->scheduleUpdate();

        return true;
    }

    bool already_highlighted;

    virtual void update(float dt) override
    {
        const uint HALF_BLOCK_UNIT = 15;
        const uint BLOCK_UNIT = HALF_BLOCK_UNIT * 2;

        CCRect visible_rect = ccr(m_pBatchNode->convertToNodeSpace(CCPoint(0,0)), CCDirector::sharedDirector()->getWinSize());

        // handle zooming
        visible_rect.size.width *= 1 / m_pLayer->getScale();
        visible_rect.size.height *= 1 / m_pLayer->getScale();

        // fix blocks disappearing half way
        visible_rect.origin.x -= HALF_BLOCK_UNIT;
        visible_rect.origin.y -= HALF_BLOCK_UNIT;
        visible_rect.size.width += BLOCK_UNIT;
        visible_rect.size.height += BLOCK_UNIT;

        if (!already_highlighted)
        {
            auto d = CCLayerColor::create(ccColor4B{255, 0, 0, 125}, visible_rect.size.width, visible_rect.size.height);
            m_pLayer->addChild(d);
            d->setPosition(visible_rect.origin);
            already_highlighted = true;
        }

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

    virtual void addToSection(GameObject* obj);

};

void SpecialLevelEditorLayer::addToSection(GameObject* obj)
{
    return hook_manager->orig<&SpecialLevelEditorLayer::addToSection>(this, obj);
}

void SpecialLevelEditorLayerd_addToSection(SpecialLevelEditorLayer* self,  GameObject* obj)
{
    if (self == nullptr)
        __android_log_print(ANDROID_LOG_DEBUG, "starry_sky", "THIS IS NULL");
    
    if (obj == nullptr)
        __android_log_print(ANDROID_LOG_DEBUG, "starry_sky", "OBJ IS NULL");
    
    //hook_manager->orig<&SpecialLevelEditorLayerd_addToSection>(self, obj);
    self->subj = obj;

    __android_log_print(ANDROID_LOG_DEBUG, "starry_sky", "Object key: %i", obj->m_nObjectKey);
}
__attribute__((constructor))
void starry_sky()
{
    hook_manager = new DobbyWrapper();
    //hook_manager.add_hook(&LoadingLayer::getLoadingString, &ll);
    hook_manager->add_hook(&LevelEditorLayer::addToSection, &SpecialLevelEditorLayer::addToSection);
    hook_manager->add_hook("_ZN16LevelEditorLayer4initEP11GJGameLevel", &SpecialLevelEditorLayer::init);
    hook_manager->add_hook("_ZN16LevelEditorLayer6createEP11GJGameLevel", &SpecialLevelEditorLayer::create);
    
    hook_manager->install_hooks();
}
