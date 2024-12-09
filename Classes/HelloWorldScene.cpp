/****************************************************************************
 Copyright (c) 2017-2018 Xiamen Yaji Software Co., Ltd.

 http://www.cocos2d-x.org

 Permission is hereby granted, free of charge, to any person obtaining a copy
 of this software and associated documentation files (the "Software"), to deal
 in the Software without restriction, including without limitation the rights
 to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 copies of the Software, and to permit persons to whom the Software is
 furnished to do so, subject to the following conditions:

 The above copyright notice and this permission notice shall be included in
 all copies or substantial portions of the Software.

 THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 THE SOFTWARE.
 ****************************************************************************/

#include "HelloWorldScene.h"
#include "SimpleAudioEngine.h"
using namespace CocosDenshion;

USING_NS_CC;

const char* bgm_file = "music/Postcard_from_Celeste_Mountain.wav";
SimpleAudioEngine* audio;

Scene* HelloWorld::createScene()
{
	audio = SimpleAudioEngine::getInstance();
	return HelloWorld::create();
}

// Print useful error message instead of segfaulting when files are not there.
static void problemLoading(const char* filename)
{
	printf("Error while loading: %s\n", filename);
	printf("Depending on how you compiled you might have to add 'Resources/' in front of filenames in HelloWorldScene.cpp\n");
}

// on "init" you need to initialize your instance
bool HelloWorld::init()
{
	//////////////////////////////
	// 1. super init first
	if (!Scene::initWithPhysics())
	{
		return false;
	}

	auto visibleSize = Director::getInstance()->getVisibleSize();
	Vec2 origin = Director::getInstance()->getVisibleOrigin();

	audio->playBackgroundMusic(bgm_file, true);

	auto label = Label::createWithTTF("Klotski With Sofa", "fonts/Marker Felt.ttf", 72);
	if (label == nullptr)
	{
		problemLoading("'fonts/Marker Felt.ttf'");
	}
	else
	{
		label->setAnchorPoint(Vec2(0.5, 0));
		// position the label on the center of the screen
		label->setPosition(Vec2(visibleSize.width / 2, 0));

		// add the label as a child to this layer
		this->addChild(label,5);
	}
	auto label1 = Label::createWithTTF("Idea: @tzh654321", "fonts/arial.ttf", 18);
	label1->setPosition(0, 200);
	label1->setAnchorPoint(Vec2(0, 1));
	this->addChild(label1,4);
	auto label2 = Label::createWithTTF("Code: @ShaoJia07", "fonts/arial.ttf", 18);
	label2->setPosition(0, 180);
	label2->setAnchorPoint(Vec2(0, 1));
	this->addChild(label2,4);
	auto label3 = Label::createWithTTF("Engine: cocos2d-x", "fonts/arial.ttf", 18);
	label3->setPosition(0, 160);
	label3->setAnchorPoint(Vec2(0, 1));
	this->addChild(label3,4);

	_world = this->getPhysicsWorld();
	_world->setDebugDrawMask(PhysicsWorld::DEBUGDRAW_ALL);
	_world->setGravity(Vec2::ZERO);

	auto frame = Node::create();
	auto frame_p = PhysicsBody::createEdgeBox(Size(800, 1000), PhysicsMaterial());
	frame_p->getFirstShape()->setTag(-1);
	frame->setPhysicsBody(frame_p);
	frame->setPosition(400, 700);
	this->addChild(frame,3);
	const PhysicsMaterial pm(0.0001f, 0.0f, 0.0f);
	std::vector<std::pair<Size, Vec2>> list({
		{{2,2},{1,3}},
		{{1,1},{1,0}},
		{{1,1},{2,0}},
		{{1,1},{3,0}},
		{{1,2},{3,1}},
		{{1,2},{3,3}},
		{{1,2},{0,3}},
		{{2,1},{1,2}}
		});
	for (int id = 0;id < list.size();id++) {
		auto i = list[id];
		Size sz = i.first * 200;
		auto pbody = PhysicsBody::createBox(sz - Size(5, 5), pm, sz / 2);
		pbody->setRotationEnable(false);
		if (!id) pbody->getFirstShape()->setTag(1);
		auto node = Sprite::create();
		node->setPhysicsBody(pbody);
		node->setPosition(i.second * 200 + Vec2(0, 200));
		this->addChild(node,3);
	}
	const int cnt = 11;
	const float PI = acosf(-1.0);
	auto pbody = PhysicsBody::create();
	std::vector<Vec2> v;
	auto ins = [&pbody,&v,&pm](){
		auto w = v;
		for (auto& i : w) i *= 200;
		pbody->addShape(PhysicsShapePolygon::create(&w[0], w.size(), pm));
		};
	v.emplace_back(0, 1);
	v.emplace_back(1, 1);
	v.emplace_back(1, 0);
	for (int i = 1;i < cnt;i++) {
		float ang = PI * i / 2 / cnt;
		v.emplace_back(1 - sinf(ang), 1 - cosf(ang));
	}
	ins();
	std::reverse(v.begin(), v.end());
	for (auto& i : v) {
		i.y = 3 - i.y;
	}
	ins();
	for (int i = 0;i < cnt ;i++) {
		v.clear();
		const float a1 = PI * i / cnt;
		const float a2 = PI * (i + 1) / cnt;
		v.emplace_back(1 - sinf(a1) / 2, 1.5f - cosf(a1) / 2);
		v.emplace_back(0, 1.5f - cosf(a1) / 2 );
		v.emplace_back(0, 1.5f - cosf(a2) / 2 );
		v.emplace_back(1 - sinf(a2) / 2, 1.5f - cosf(a2) / 2);
		ins();
	}
	auto node = Sprite::create();
	node->setPhysicsBody(pbody);
	node->setPosition(0,200);
	addChild(node,3);

	_touchListener = EventListenerTouchAllAtOnce::create();
	_touchListener->onTouchesBegan = CC_CALLBACK_2(HelloWorld::onTouchesBegan, this);
	_touchListener->onTouchesMoved = CC_CALLBACK_2(HelloWorld::onTouchesMoved, this);
	_touchListener->onTouchesEnded = CC_CALLBACK_2(HelloWorld::onTouchesEnded, this);
	_touchListener->onTouchesCancelled = CC_CALLBACK_2(HelloWorld::onTouchesEnded, this);
	_eventDispatcher->addEventListenerWithSceneGraphPriority(_touchListener, this);
	MenuItemImage* bgm1 = MenuItemImage::create("pics/bgm1.png", "pics/bgm1.png");
	MenuItemImage* bgm2 = MenuItemImage::create("pics/bgm2.png", "pics/bgm2.png");
	bgm1->setTag(1);
	bgm2->setTag(2);
	MenuItemToggle* toggleItem = MenuItemToggle::createWithCallback(CC_CALLBACK_1(HelloWorld::BgmButtonCallback, this), bgm1, bgm2, NULL);

	auto toggleMenu = Menu::create(toggleItem, NULL);
	toggleMenu->setPosition(Vec2(750, 30));
	this->addChild(toggleMenu, 7);

	schedule([this](float) {
		int cnt = 0;
		cnt+=laser(_drawNodes[0], 300);
		cnt+=laser(_drawNodes[1], 500);
		if (cnt == 2) {
			unschedule("lasers");
			afterWinning();
		}
		}, "lasers");
	return true;
}


bool HelloWorld::laser(DrawNode*& drawNode, float x) {
	Vec2 point(x, 1200);
	PhysicsShape* shape(nullptr);
	PhysicsRayCastCallbackFunc func = [&point, &shape](PhysicsWorld& world, const PhysicsRayCastInfo& info, void* data)->bool
		{
			if (info.shape->getTag() != -1 && info.contact.y < point.y) {
				point = info.contact;
				shape = info.shape;
			}
			return true;
		};
	if (drawNode) {
		removeChild(drawNode);
	}
	drawNode = DrawNode::create();
	const float offset = 200;
	_world->rayCast(func, Vec2(x, 200 - offset), Vec2(x, 1200 + offset), nullptr);
	bool ret = (shape && shape->getTag() == 1);
	drawNode->drawSegment(Vec2(x, 200), Vec2(x, point.y), 1, ret ? Color4F::BLUE : Color4F::RED);
	drawNode->drawDot(point, 3, Color4F::WHITE);
	addChild(drawNode,3);
	return ret;
}

void HelloWorld::afterWinning() {
	_world->setSpeed(0.1f);
	_eventDispatcher->removeEventListener(_touchListener);
	for (auto i : _mouses) {
		removeChild(i.second);
	}
	_mouses.clear();
	auto winning_layer = Layer::create();
	auto mask_img=Sprite::create("pics/translucent_black.png");
	mask_img->setContentSize(Size(800,1200));
	mask_img->setAnchorPoint(Vec2(0,0));
	auto label1 = Label::createWithTTF("You Win!", "fonts/Marker Felt.ttf", 72);
	label1->setPosition(400,650);
	auto label2 = Label::createWithTTF("close the window to exit", "fonts/Marker Felt.ttf", 24);
	label2->setPosition(400,600);

	winning_layer->addChild(mask_img,-1);
	winning_layer->addChild(label1);
	winning_layer->addChild(label2);
	addChild(winning_layer,6);

	if(audio->isBackgroundMusicPlaying()) audio->playEffect("music/win.wav");
}

void HelloWorld::BgmButtonCallback(Object* pSender) {
	MenuItemToggle* toggleItem = (CCMenuItemToggle*)pSender;
	if (toggleItem->getSelectedItem()->getTag() == 1) {
		audio->resumeBackgroundMusic();
	}
	else {
		audio->pauseBackgroundMusic();
		audio->stopAllEffects();
	}
}

void HelloWorld::onTouchesBegan(std::vector<Touch*> touches, Event* event)
{
	for (auto touch : touches) {
		auto location = touch->getLocation();
		auto scene = Director::getInstance()->getRunningScene();
		auto arr = scene->getPhysicsWorld()->getShapes(location);
		if (!arr.empty())
		{
			PhysicsBody* body = arr.front()->getBody();
			Node* mouse = Node::create();
			mouse->setPhysicsBody(PhysicsBody::create(PHYSICS_INFINITY, PHYSICS_INFINITY));
			mouse->getPhysicsBody()->setDynamic(false);
			mouse->setPosition(location);
			this->addChild(mouse,3);
			PhysicsJointPin* joint = PhysicsJointPin::construct(mouse->getPhysicsBody(), body, location);
			joint->setMaxForce(100000.0f);
			scene->getPhysicsWorld()->addJoint(joint);
			_mouses.insert(std::make_pair(touch->getID(), mouse));

		}
	}
}

void HelloWorld::onTouchesMoved(std::vector<Touch*> touches, Event* event)
{
	for (auto touch : touches) {
		auto it = _mouses.find(touch->getID());

		if (it != _mouses.end())
		{
			it->second->setPosition(touch->getLocation());
		}
	}
}

void HelloWorld::onTouchesEnded(std::vector<Touch*> touches, Event* event)
{

	for (auto touch : touches) {
		auto it = _mouses.find(touch->getID());

		if (it != _mouses.end())
		{
			this->removeChild(it->second);
			_mouses.erase(it);
		}
	}
}


void HelloWorld::menuCloseCallback(Ref* pSender)
{
	//Close the cocos2d-x game scene and quit the application
	Director::getInstance()->end();

#if (CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
	exit(0);
#endif

	/*To navigate back to native iOS screen(if present) without quitting the application  ,do not use Director::getInstance()->end() and exit(0) as given above,instead trigger a custom event created in RootViewController.mm as below*/

	//EventCustom customEndEvent("game_scene_close_event");
	//_eventDispatcher->dispatchEvent(&customEndEvent);


}
