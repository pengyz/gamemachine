﻿#ifndef __GMGLRENDERERS_H__
#define __GMGLRENDERERS_H__
#include <gmcommon.h>
#include <gmenums.h>
#include <gmmodel.h>
#include "gmglgraphic_engine.h"
BEGIN_NS

class GMGLTechnique;
class GMGammaHelper
{
public:
	void setGamma(GMGLTechnique* tech, GMGraphicEngine* engine, IShaderProgram* shaderProgram);

private:
	GMfloat m_gamma = 0.f;
};

GM_PRIVATE_OBJECT(GMGLTechnique)
{
	struct TechniqueContext
	{
		GMModel* currentModel = nullptr;
		GMScene* currentScene = nullptr;
	};

	const IRenderContext* context = nullptr;
	GMGLGraphicEngine* engine = nullptr;
	const GMShaderVariablesDesc* variablesDesc = nullptr;
	GMDebugConfig debugConfig;
	GMGammaHelper gammaHelper;
	TechniqueContext techContext;

	Vector<GMShaderVariablesIndices> indexBank;

	struct TextureIndices
	{
		Array<GMint32, (GMsize_t)GMTextureType::EndOfEnum> Texture;
		Array<GMint32, (GMsize_t)GMTextureType::EndOfEnum> Enabled;
	};
	Vector<TextureIndices> textureIndices;

	struct TextureTransformIndices
	{
		Array<GMint32, (GMsize_t)GMTextureType::EndOfEnum> ScrollS;
		Array<GMint32, (GMsize_t)GMTextureType::EndOfEnum> ScrollT;
		Array<GMint32, (GMsize_t)GMTextureType::EndOfEnum> ScaleS;
		Array<GMint32, (GMsize_t)GMTextureType::EndOfEnum> ScaleT;
	};
	Vector<TextureTransformIndices> textureTransformIndices;

	struct ScreenInfoIndices
	{
		GMint32 Multisampling;
		GMint32 ScreenWidth;
		GMint32 ScreenHeight;
	};
	Vector<ScreenInfoIndices> screenInfoIndices;
	Array<GMint32, GMScene::MaxBoneCount> boneVariableIndices;
};

class GMGLTechnique : public GMObject, public ITechnique
{
	GM_DECLARE_PRIVATE(GMGLTechnique)

public:
	GMGLTechnique(const IRenderContext* context);

	virtual void draw(GMModel* model) override;
	virtual IShaderProgram* getShaderProgram() = 0;

protected:
	virtual void beforeDraw(GMModel* model) = 0;
	virtual void afterDraw(GMModel* model) = 0;
	virtual void beginScene(GMScene* scene) override;
	virtual void endScene() override;
	virtual void beginModel(GMModel* model, const GMGameObject* parent) override;
	virtual void endModel() override;

protected:
	virtual void activateTextureTransform(GMModel* model, GMTextureType type);
	virtual GMint32 activateTexture(GMModel* model, GMTextureType type);
	virtual void deactivateTexture(GMTextureType type);
	virtual GMint32 getTextureID(GMTextureType type);
	virtual bool drawTexture(GMModel* model, GMTextureType type);
	virtual GMTextureAsset getTexture(GMTextureSampler& frames);
	virtual void updateCameraMatrices(IShaderProgram* shaderProgram);

protected:
	void prepareScreenInfo(IShaderProgram* shaderProgram);
	void prepareShaderAttributes(GMModel* model);
	void prepareBlend(GMModel* model);
	void prepareFrontFace(GMModel* model);
	void prepareCull(GMModel* model);
	void prepareDepth(GMModel* model);
	void prepareLine(GMModel* model);
	void prepareDebug(GMModel* model);
	void prepareLights();
	GMIlluminationModel prepareIlluminationModel(GMModel* model);

private:
	void updateBoneTransforms(IShaderProgram* shaderProgram, GMModel* model);
	void startDraw(GMModel* model);

public:
	static void dirtyShadowMapAttributes();
};

GM_PRIVATE_OBJECT(GMGLTechnique_3D)
{
	GMRenderMode renderMode = GMRenderMode::Forward;
	GMTextureAsset whiteTexture;
	GMint32 drawDebugNormalIndex = 0;
};

class GMGLTechnique_3D : public GMGLTechnique
{
	GM_DECLARE_PRIVATE_AND_BASE(GMGLTechnique_3D, GMGLTechnique)

public:
	using GMGLTechnique::GMGLTechnique;

public:
	virtual void beginModel(GMModel* model, const GMGameObject* parent) override;
	virtual void beforeDraw(GMModel* model) override;
	virtual void afterDraw(GMModel* model) override;
	virtual IShaderProgram* getShaderProgram() override;

protected:
	void prepareMaterial(const GMShader& shader);
	void prepareTextures(GMModel* model, GMIlluminationModel illuminationModel);
	void drawDebug();

private:
	GMTextureAsset getWhiteTexture();
};

class GMGLTechnique_2D : public GMGLTechnique_3D
{
public:
	using GMGLTechnique_3D::GMGLTechnique_3D;

public:
	virtual void beforeDraw(GMModel* model) override;

protected:
	void prepareTextures(GMModel* model);
};

class GMGLTechnique_CubeMap : public GMGLTechnique_3D
{
public:
	using GMGLTechnique_3D::GMGLTechnique_3D;

public:
	virtual void beginModel(GMModel* model, const GMGameObject* parent) override;
	virtual void beforeDraw(GMModel* model) override;
	virtual void afterDraw(GMModel* model) override;

protected:
	void prepareTextures(GMModel* model);
};

GM_PRIVATE_OBJECT(GMGLTechnique_Filter)
{
	struct HDRState
	{
		GMToneMapping::Mode toneMapping = GMToneMapping::Reinhard;
		bool HDR = false;
	};
	HDRState state;
	GMint32 framebufferIndex = 0;
};

class GMGLTechnique_Filter : public GMGLTechnique
{
	GM_DECLARE_PRIVATE_AND_BASE(GMGLTechnique_Filter, GMGLTechnique)

public:
	using GMGLTechnique::GMGLTechnique;

private:
	virtual void beforeDraw(GMModel* model) override;
	virtual void afterDraw(GMModel* model) override;
	virtual void beginModel(GMModel* model, const GMGameObject* parent) override;
	virtual IShaderProgram* getShaderProgram() override;

protected:
	virtual GMint32 activateTexture(GMModel* model, GMTextureType type);

protected:
	void prepareTextures(GMModel* model);

private:
	void setHDR(IShaderProgram* shaderProgram);
};

GM_PRIVATE_OBJECT(GMGLTechnique_LightPass)
{
	Vector<Vector<GMint32>> gbufferIndices;
};

class GMGLTechnique_LightPass : public GMGLTechnique
{
	GM_DECLARE_PRIVATE_AND_BASE(GMGLTechnique_LightPass, GMGLTechnique)

public:
	using GMGLTechnique::GMGLTechnique;

protected:
	virtual IShaderProgram* getShaderProgram() override;
	virtual void beforeDraw(GMModel* model) override;
	virtual void afterDraw(GMModel* model) override;
	virtual void beginModel(GMModel* model, const GMGameObject* parent) override;

protected:
	void prepareLights();
	void prepareTextures(GMModel* model);
};

class GMGLTechnique_3D_Shadow : public GMGLTechnique_3D
{
public:
	using GMGLTechnique_3D::GMGLTechnique_3D;

protected:
	virtual void beginModel(GMModel* model, const GMGameObject* parent) override;
};

class GMGLTechnique_Particle : public GMGLTechnique_2D
{
public:
	using GMGLTechnique_2D::GMGLTechnique_2D;
};

class GMGLTechnique_Custom : public GMGLTechnique_3D
{
public:
	using GMGLTechnique_3D::GMGLTechnique_3D;

	virtual IShaderProgram* getShaderProgram() override;
};

END_NS
#endif