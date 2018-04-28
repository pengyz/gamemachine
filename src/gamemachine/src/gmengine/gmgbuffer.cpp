﻿#include "stdafx.h"
#include "gmgbuffer.h"
#include "foundation/utilities/utilities.h"
#include "gameobjects/gmgameobject.h"
#include "foundation/gamemachine.h"
#include "gmgraphicengine.h"

GMGBuffer::GMGBuffer(GMGraphicEngine* engine)
{
	D(d);
	d->engine = engine;
}

GMGBuffer::~GMGBuffer()
{
	D(d);
	GM_delete(d->quad);
	GM_delete(d->quadModel);
	GM_delete(d->geometryFramebuffers);
}

void GMGBuffer::createQuad()
{
	D(d);
	GM_ASSERT(!d->quad);
	GM_ASSERT(!d->quadModel);
	GMPrimitiveCreator::createQuadrangle(GMPrimitiveCreator::one2(), 0, &d->quadModel);
	GM_ASSERT(d->quadModel);
	d->quadModel->setType(GMModelType::LightPassQuad);
	GM.createModelPainterAndTransfer(d->quadModel);
	d->quad = new GMGameObject(GMAssets::createIsolatedAsset(GMAssetType::Model, d->quadModel));
}

void GMGBuffer::init()
{
	D(d);
	if (!d->geometryFramebuffers)
		d->geometryFramebuffers = createGeometryFramebuffers();

	if (!d->quad)
		createQuad();
}

void GMGBuffer::setGeometryPassingState(GMGeometryPassingState state)
{
	D(d);
	d->state = state;
}

GMGeometryPassingState GMGBuffer::getGeometryPassingState()
{
	D(d);
	return d->state;
}

GMGameObject* GMGBuffer::getQuad()
{
	D(d);
	return d->quad;
}

IFramebuffers* GMGBuffer::getGeometryFramebuffers()
{
	D(d);
	GM_ASSERT(d->geometryFramebuffers);
	return d->geometryFramebuffers;
}