﻿#include "stdafx.h"
#include "gmgameworld.h"
#include "gameobjects/gmgameobject.h"
#include "gmdata/gmmodel.h"
#include <algorithm>
#include <time.h>
#include "foundation/gamemachine.h"
#include "gameobjects/gmcontrolgameobject.h"

namespace
{
	bool needBlend(GMGameObject* object)
	{
		GMModels& models = object->getModels();
		for (auto& model : models)
		{
			if (model->getShader().getBlend())
				return true;
		}
		return false;
	}
}

GMGameWorld::~GMGameWorld()
{
	D(d);
	for (auto gameObject : d->gameObjects)
	{
		GM_delete(gameObject);
	}

	for (auto control : d->controls)
	{
		GM_delete(control);
	}

	GM_delete(d->physicsWorld);
}

void GMGameWorld::addObjectAndInit(AUTORELEASE GMGameObject* obj)
{
	D(d);
	obj->setWorld(this);
	obj->onAppendingObjectToWorld();
	d->gameObjects.insert(obj);
	GMModels& models = obj->getModels();
	for (auto& model : models)
	{
		GM.createModelPainterAndTransfer(model);
	}
}

void GMGameWorld::renderScene()
{
	D(d);
	static List<GMGameObject*> s_emptyList;
	IGraphicEngine* engine = GM.getGraphicEngine();
	if (getRenderPreference() == GMRenderPreference::PreferForwardRendering)
	{
		engine->draw(d->renderList.deferred, s_emptyList);
		engine->draw(d->renderList.forward, s_emptyList);
	}
	else
	{
		engine->draw(d->renderList.forward, d->renderList.deferred);
	}

	engine->beginBlend();
	auto& controls = getControls();
	engine->draw(controls, s_emptyList);
	engine->endBlend();
}

bool GMGameWorld::removeObject(GMGameObject* obj)
{
	D(d);
	auto& objs = d->gameObjects;
	auto objIter = objs.find(obj);
	if (objIter == objs.end())
		return false;
	GMGameObject* eraseTarget = *objIter;
	obj->onRemovingObjectFromWorld();
	objs.erase(objIter);
	delete eraseTarget;
	return true;
}

void GMGameWorld::simulateGameWorld()
{
	D(d);
	auto phyw = getPhysicsWorld();
	simulateGameObjects(phyw, d->gameObjects);
}

void GMGameWorld::addControl(GMControlGameObject* control)
{
	D(d);
	control->setWorld(this);
	control->onAppendingObjectToWorld();
	d->controls.push_back(control);
}

void GMGameWorld::notifyControls()
{
	D(d);
	for (auto& obj : d->controls)
	{
		gm_cast<GMControlGameObject*>(obj)->notifyControl();
	}
}

void GMGameWorld::clearRenderList()
{
	D(d);
	d->renderList.deferred.clear();
	d->renderList.forward.clear();
}

void GMGameWorld::simulateGameObjects(GMPhysicsWorld* phyw, const Set<GMGameObject*>& gameObjects)
{
	for (auto& gameObject : gameObjects)
	{
		gameObject->simulate();
		if (phyw)
			phyw->simulate(gameObject);
		gameObject->updateAfterSimulate();
	}
}

void GMGameWorld::addToRenderList(GMGameObject* object)
{
	D(d);
	if (object->canDeferredRendering())
	{
		d->renderList.deferred.push_back(object);
	}
	else
	{
		if (needBlend(object))
			d->renderList.forward.push_back(object);
		else
			d->renderList.forward.push_front(object);
	}
}