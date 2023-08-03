#include "modules/editor.hpp"

UpdatableLevelEditorLayer* ulel;

std::vector<GameObject*> bg_col_triggers;
//CCSprite* background_sprite;
//CCLayerColor* preview_line;

bool LevelEditorLayer_init(LevelEditorLayer* self, GJGameLevel* lvl)
{
    if (!starry_sky::get_hook_manager()->orig<&LevelEditorLayer_init>(self, lvl))
        return false;

    ulel = (UpdatableLevelEditorLayer*)self;

    //auto win_size = CCDirector::sharedDirector()->getWinSize();
    
    //preview_line = CCLayerColor::create(ccColor4B{255,255,255,125}, 2, win_size.height);
    //self->addChild(prev_line);
    //prev_line->setPosition(ccp(win_size.width / 2.5, 0));

    //background_sprite = (CCSprite*) self->getChildren()->objectAtIndex(1);

    self->scheduleUpdate();

    return true;
}

void UpdatableLevelEditorLayer::update(float dt)
{
    CCRect visible_rect;
    
    auto vr_origin = m_pLayer->convertToNodeSpace(CCPoint(0,0));
    auto vr_dest = CCDirector::sharedDirector()->getWinSize();
    
    visible_rect.origin = vr_origin;
    visible_rect.size = vr_dest;
    
    // handle zooming
    visible_rect.size.width *= 1 / m_pLayer->getScale();
    visible_rect.size.height *= 1 / m_pLayer->getScale();

    // fix blocks disappearing where it shouldn't by extending the visible rect by a few blocks
    visible_rect.origin.x -= (BLOCK_UNIT * 2) + HALF_BLOCK_UNIT;
    visible_rect.origin.y -= (BLOCK_UNIT * 2) + HALF_BLOCK_UNIT;
    
    visible_rect.size.width += BLOCK_UNIT * 5;
    visible_rect.size.height += BLOCK_UNIT * 5;

    //if (!already_highlighted)
    //{
    //    auto d = CCLayerColor::create(ccColor4B{255, 0, 0, 125}, visible_rect.size.width - BLOCK_UNIT, visible_rect.size.height - BLOCK_UNIT);
    //    m_pLayer->addChild(d);
    //    d->setTag(727);
    //    d->setPosition(visible_rect.origin.x + HALF_BLOCK_UNIT, visible_rect.origin.y + HALF_BLOCK_UNIT);
    //    already_highlighted = true;
    //    __android_log_print(ANDROID_LOG_DEBUG, "starry_log_sky", "VR %f %f", d->getPositionX(), d->getPositionY()); 
    //}
    //else
    //{
    //    m_pLayer->getChildByTag(727)->setPosition(visible_rect.origin.x + HALF_BLOCK_UNIT, visible_rect.origin.y + HALF_BLOCK_UNIT);
    //}

    //if (move_prev_line)
    //{
    //    auto speed = SPEEDS[Speed::Normal];
    //    _preview_line->setPositionX(_preview_line->getPositionX() + ((dt * 60) * speed.x_velocity * speed.time_mod));
    //}

    //__android_log_print(ANDROID_LOG_DEBUG, "starry_log_sky", "Preview line x-pos: %f", m_pLayer->convertToNodeSpace(preview_line->getPosition()).x);
    
    for (uint32_t section_id = 0; section_id < m_obSectionArray->count(); section_id++)
    {
        CCArray* section_objects = (CCArray*) m_obSectionArray->objectAtIndex(section_id);

        for (uint32_t index = 0; index < section_objects->count(); index++)
        {
            GameObject* object = (GameObject*) section_objects->objectAtIndex(index);
            CCPoint object_pos = object->getPosition();

            if (CCRect::CCRectContainsPoint(visible_rect, object_pos))
            {
                if (!object->getParent())
                {
                    OrderingData* s = (OrderingData*)object->getUserData();

                    m_pBatchNode->addChild(object, s->z_order);
                    object->setOrderOfArrival(s->order_of_arrival);

                    m_pBatchNode->sortAllChildren();
                }
            }
            else
            {
                if (object->getParent())
                {
                    if (object->getUserData() == nullptr)
                    {
                        OrderingData s = OrderingData {object->getOrderOfArrival(), object->getZOrder()};
                        object->setUserData((void*)&s);
                    }

                    m_pBatchNode->removeChild(object, false);
                }
            }
        }
    }
}

void DrawGridLayer_addToEffects(DrawGridLayer* self, GameObject* obj)
{
    starry_sky::get_hook_manager()->orig<&DrawGridLayer_addToEffects>(self, obj);

    if (obj->m_stTextureName.c_str() == "edit_eTintBGBtn_001.png")
        if (std::find(bg_col_triggers.begin(), bg_col_triggers.end(), obj) != bg_col_triggers.end())
            bg_col_triggers.push_back(obj);
}

void DrawGridLayer_removeFromEffects(DrawGridLayer* self, GameObject* obj)
{
    starry_sky::get_hook_manager()->orig<&DrawGridLayer_removeFromEffects>(self, obj);

    if (obj->m_stTextureName.c_str() == "edit_eTintBGBtn_001.png")
        if (auto it = std::find(bg_col_triggers.begin(), bg_col_triggers.end(), obj); it != bg_col_triggers.end())
            bg_col_triggers.erase(it);
}

void EditorUI_setupCreateMenu(EditorUI* self)
{
    if (self->m_sSelectedObjectTextureNameMaybeIdkIFoundItOnCreate.empty())
    {
        typedef std::string* (*gd_string_assign_t)(void*, const char*, uint);
        gd_string_assign_t gd_string_assign = reinterpret_cast<gd_string_assign_t>(reinterpret_cast<uintptr_t>(get_dlinfo_from_addr(get_function_address(&LevelEditorLayer::addToSection)).dli_fbase) + 0x2E1390);
    
        // this is fucking stupid
        gd_string_assign((void*)&self->m_sSelectedObjectTextureNameMaybeIdkIFoundItOnCreate, " ", 1);
    }
    
    CCArray* unk_array = CCArray::create();
    self->m_pUnkArrayOnSetupCreateMenu = unk_array;
    unk_array->retain();

    CCArray* obj_array = CCArray::create();

    std::vector<std::pair<const char*, int>> objects = {
        std::make_pair("square_01_001.png", 4),
        std::make_pair("square_08_001.png", 4),
        std::make_pair("square_02_001.png", 4),
        std::make_pair("square_03_001.png", 4),
        std::make_pair("square_04_001.png", 4),
        std::make_pair("square_05_001.png", 4),
        std::make_pair("square_06_001.png", 4),
        std::make_pair("square_07_001.png", 4),

        std::make_pair("spike_01_001.png", 4),
        std::make_pair("spike_02_001.png", 4),
        std::make_pair("pit_01_001.png", 4),
        std::make_pair("pit_04_001.png", 4),

        std::make_pair("plank_01_001.png", 4),

        std::make_pair("square_b_01_001.png", 4),
        std::make_pair("square_b_05_001.png", 4),
        std::make_pair("square_b_04_001.png", 4),
        std::make_pair("square_b_06_001.png", 4),

        std::make_pair("bump_01_001.png", 4),
        std::make_pair("gravbump_01_001.png", 4),
        std::make_pair("ring_01_001.png", 4),
        std::make_pair("gravring_01_001.png", 4),

        std::make_pair("portal_01_front_001.png", 4),
        std::make_pair("portal_02_front_001.png", 4),
        std::make_pair("portal_03_front_001.png", 4),
        std::make_pair("portal_04_front_001.png", 4),
        std::make_pair("portal_05_front_001.png", 4),
        std::make_pair("portal_06_front_001.png", 4),
        std::make_pair("portal_07_front_001.png", 4),

        std::make_pair("rod_01_001.png", 4),
        std::make_pair("rod_02_001.png", 4),
        std::make_pair("rod_03_001.png", 4),

        std::make_pair("d_spikes_01_001.png", 4),
        std::make_pair("d_spikes_02_001.png", 4),
        std::make_pair("d_spikes_03_001.png", 4),
        std::make_pair("d_spikes_04_001.png", 4),

        std::make_pair("chain_01_001.png", 4),
        std::make_pair("d_cloud_01_001.png", 4),
        std::make_pair("d_cloud_02_001.png", 4),
        std::make_pair("d_ball_01_001.png", 4),
        std::make_pair("d_ball_02_001.png", 4),
        std::make_pair("d_ball_03_001.png", 4),
        std::make_pair("d_ball_04_001.png", 4),
        std::make_pair("d_ball_05_001.png", 4),
        std::make_pair("d_ball_06_001.png", 4),

        std::make_pair("square_c_01_001.png", 4),
        std::make_pair("square_c_02_001.png", 4),
        std::make_pair("square_c_03_001.png", 4),
        std::make_pair("square_c_04_001.png", 4),
        std::make_pair("square_c_05_001.png", 4),
        std::make_pair("square_c_06_001.png", 4),
        std::make_pair("square_c_07_001.png", 4),

        // force new page
        std::make_pair("", 0),

        std::make_pair("edit_eeNoneBtn_001.png", 4),
        std::make_pair("edit_eeFTBtn_001.png", 4),
        std::make_pair("edit_eeFBBtn_001.png", 4),
        std::make_pair("edit_eeFLBtn_001.png", 4),
        std::make_pair("edit_eeFRBtn_001.png", 4),
        std::make_pair("edit_eeSUBtn_001.png", 4),
        std::make_pair("edit_eeSDBtn_001.png", 4),
        std::make_pair("edit_eeFALBtn_001.png", 4),
        std::make_pair("edit_eeFARBtn_001.png", 4),
        std::make_pair("edit_eeFRHBtn_001.png", 4),
        std::make_pair("edit_eeFRHInvBtn_001.png", 4),

        std::make_pair("", 0),

        std::make_pair("edit_eTintBGBtn_001.png", 4),
        std::make_pair("edit_eTintGBtn_001.png", 4),
        std::make_pair("edit_eStartPosBtn_001.png", 4),
        std::make_pair("edit_eGhostDBtn_001.png", 4),
        std::make_pair("edit_eGhostEBtn_001.png", 4),

        std::make_pair("", 0),

        // unlisted blocks
        std::make_pair("square_b_02_001.png", 4),
        std::make_pair("square_b_03_001.png", 4),
        std::make_pair("square_d_01_001.png", 4),
        std::make_pair("square_d_02_001.png", 4),
        std::make_pair("square_d_03_001.png", 4),
        std::make_pair("square_d_04_001.png", 4),
        std::make_pair("square_d_05_001.png", 4),
        std::make_pair("square_d_06_001.png", 4),
        std::make_pair("square_d_07_001.png", 4),

        std::make_pair("", 0),

        // unlisted triggers
        std::make_pair("edit_eLevelEndBtn_001.png", 4),
        std::make_pair("edit_eBGEOn_001.png", 4),
        std::make_pair("edit_eBGEOff_001.png", 4),
        std::make_pair("edit_eeFABtn_001.png", 4),
    };

    for (auto obj: objects)
    {
        if (obj.first == "" || obj.second == 0)
        {
            auto node = CCNode::create();
            node->setTag(0);
            obj_array->addObject(node);
        }
        else
        {
            obj_array->addObject(self->getCreateBtn(obj.first, obj.second));
        }
    }

    auto director = CCDirector::sharedDirector();
    CCPoint unk_point = ccp(director->getWinSize().width / 2 - 5, 0 + self->m_fEditBarHeight - 6); // y should be getscreenbottom() - float - 6, but this works fine

    self->m_pEditButtonBarForCreateMenu = EditButtonBar::create(obj_array, unk_point);
    self->addChild(self->m_pEditButtonBarForCreateMenu, 10);
    self->updateCreateMenu();
}

void LevelEditorLayer_createObjectsFromSetup(UpdatableLevelEditorLayer* self, std::string str)
{
    if (str.empty() || str.c_str() == " ")
        return;
    
    int s_globalOrderOfArrival = *reinterpret_cast<int*>(reinterpret_cast<uintptr_t>(get_dlinfo_from_addr(get_function_address(&LevelEditorLayer::createObjectsFromSetup)).dli_fbase) + 0x430C4C);
    
    // We have to recreate gd's std::strings to avoid crashes.
    // To give our std::strings to gd, we have to recreate them by creating a CCString then getting the m_sString variable.

    std::stringstream ss(
        std::string(str.c_str())
    );
    std::string split_buffer;

    if (!std::getline(ss, split_buffer, ';'))
        return; // no level settings string?

    self->m_pLevelSettings = LevelSettingsObject::objectFromString(
        CCString::createWithFormat("%s", split_buffer.c_str())->m_sString
    );
    self->m_pLevelSettings->retain();

    while (std::getline(ss, split_buffer, ';'))
    {
        std::string objstr = CCString::createWithFormat("%s", split_buffer.c_str())->m_sString;
        GameObject* obj = GameObject::objectFromString(objstr);
        
        OrderingData s = OrderingData {s_globalOrderOfArrival++, obj->getZOrder()};
        obj->setUserData((void*)&s);
        obj->retain();

        self->addToSection(obj);

        if (obj->getType() == 7 && obj->getShouldSpawn())
            self->m_pGrid->addToEffects(obj);
    }
}

void EditorPauseLayer_customSetup(EditorPauseLayer* self)
{
    starry_sky::get_hook_manager()->orig<&EditorPauseLayer_customSetup>(self);

    CCString* info_text = CCString::createWithFormat("%i objects", ulel->m_nObjectCount);
    CCLabelBMFont* info_label = CCLabelBMFont::create(info_text->getCString(), "bigFont.fnt");
    
    info_label->setAnchorPoint({0, 0.0});
    info_label->setPosition({5, CCDirector::sharedDirector()->getWinSize().height - 50});
    info_label->setScale(0.5);

    self->addChild(info_label);
}

void EditorModule::init(DobbyWrapper* hook_manager)
{
    hook_manager
        ->add_hook("_ZN16LevelEditorLayer6createEP11GJGameLevel", &UpdatableLevelEditorLayer::create)
        ->add_hook(&LevelEditorLayer::init, &LevelEditorLayer_init)
        ->add_hook(&DrawGridLayer::addToEffects, &DrawGridLayer_addToEffects)
        ->add_hook(&DrawGridLayer::removeFromEffects, &DrawGridLayer_removeFromEffects)
        ->add_hook(&EditorUI::setupCreateMenu, &EditorUI_setupCreateMenu)
        ->add_hook(&LevelEditorLayer::createObjectsFromSetup, &LevelEditorLayer_createObjectsFromSetup)
        ->add_hook(&EditorPauseLayer::customSetup, &EditorPauseLayer_customSetup);
};
