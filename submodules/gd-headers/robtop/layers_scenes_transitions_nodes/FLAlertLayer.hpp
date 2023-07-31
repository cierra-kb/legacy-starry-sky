#ifndef __FLALERTLAYER_HPP__
#define __FLALERTLAYER_HPP__

#include <cocos2d.h>

using namespace cocos2d;

class FLAlertLayer;
class FLAlertLayerProtocol
{
public:
    virtual void FLAlert_Clicked(FLAlertLayer*, bool);
};

class FLAlertLayer : public CCLayerColor
{
public:
    CCMenu *m_pMenu;
    void* unk;
    void* unk2;
    FLAlertLayerProtocol *m_pPParent;
    CCNode *m_pTargetScene;
    bool m_bReverseKeyBack;
    CCLayer *m_pInternalLayer;

    virtual ~FLAlertLayer();
    virtual bool ccTouchBegan(CCTouch *pTouch, CCEvent *pEvent);
    virtual void ccTouchMoved(CCTouch *pTouch, CCEvent *pEvent);
    virtual void ccTouchEnded(CCTouch *pTouch, CCEvent *pEvent);
    virtual void ccTouchCancelled(CCTouch *pTouch, CCEvent *pEvent);
    virtual void registerWithTouchDispatcher();
    virtual bool show();
    virtual void keyBackClicked();

    virtual FLAlertLayerProtocol* getPParent();
    virtual void setPParent(FLAlertLayerProtocol*);

    virtual CCNode* getTargetScene();
    virtual void setTargetScene(CCNode*);

    virtual bool getReverseKeyBack();
    virtual void setReverseKeyBack(bool);
    virtual CCLayer* getInternalLayer();
};

#endif
