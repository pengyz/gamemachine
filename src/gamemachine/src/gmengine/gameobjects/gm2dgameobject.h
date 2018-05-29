﻿#ifndef __GM2DGAMEOBJECT_H__
#define __GM2DGAMEOBJECT_H__
#include <gmcommon.h>
#include "gmassets.h"
#include "gmcontrolgameobject.h"
#include <gmutilities.h>
BEGIN_NS

struct ITypoEngine;

GM_PRIVATE_OBJECT(GM2DGameObjectBase)
{
	bool dirty = true;
	GMRect geometry = { 0 };
};

class GM2DGameObjectBase : public GMGameObject
{
	DECLARE_PRIVATE_AND_BASE(GM2DGameObjectBase, GMGameObject)

public:
	using GMGameObject::GMGameObject;

public:
	void setGeometry(const GMRect& geometry);

public:
	inline const GMRect& getGeometry()
	{
		D(d);
		return d->geometry;
	}

protected:
	void setShader(GMShader& shader);

	inline void markDirty()
	{
		D(d);
		d->dirty = true;
	}

	inline void cleanDirty()
	{
		D(d);
		d->dirty = false;
	}

	inline bool isDirty()
	{
		D(d);
		return d->dirty;
	}

protected:
	static GMRectF toViewportRect(const GMRect& rc);
};

enum GMTextColorType
{
	Plain,
	ByScript,
};

GM_PRIVATE_OBJECT(GMTextGameObject)
{
	GMString text;
	GMint lineHeight = 0;
	bool center = false;
	GMsize_t length = 0;
	ITexture* texture = nullptr;
	ITypoEngine* typoEngine = nullptr;
	bool insetTypoEngine = true;
	GMModel* model = nullptr;
	GMTextColorType colorType = ByScript;
	GMFloat4 color = GMFloat4(1, 1, 1, 1);
	Vector<GMVertex> vericesCache;
};

class GMTextGameObject : public GM2DGameObjectBase
{
	DECLARE_PRIVATE_AND_BASE(GMTextGameObject, GM2DGameObjectBase)

public:
	GMTextGameObject();
	GMTextGameObject(ITypoEngine* typo);
	~GMTextGameObject();

public:
	void setText(const GMString& text);
	void setColorType(GMTextColorType type);
	void setColor(const GMVec4& color);
	void setCenter(bool center);

public:
	virtual void onAppendingObjectToWorld() override;
	virtual void draw() override;

private:
	void update();
	GMModel* createModel();
	void updateVertices(GMModel* model);
};

GM_PRIVATE_OBJECT(GMSprite2DGameObject)
{
	GMModel* model = nullptr;
	ITexture* texture = nullptr;
	GMRect textureRc;
	GMint texHeight = 0;
	GMint texWidth = 0;
	GMfloat depth = 0;
	bool needUpdateTexture = true;
	GMFloat4 color = GMFloat4(1, 1, 1, 1);
};

class GMSprite2DGameObject : public GM2DGameObjectBase
{
	DECLARE_PRIVATE_AND_BASE(GMSprite2DGameObject, GM2DGameObjectBase)

	enum
	{
		VerticesCount = 4
	};

public:
	GMSprite2DGameObject() = default;
	~GMSprite2DGameObject();

public:
	virtual void draw() override;

public:
	void setDepth(GMint depth);
	void setTexture(ITexture* tex);
	void setTextureSize(GMint width, GMint height);
	void setTextureRect(const GMRect& rect);
	void setColor(const GMVec4& color);

private:
	void update();
	GMModel* createModel();
	void updateVertices(GMModel* model);
	void updateTexture(GMModel* model);
};

//GMGlyphObject
GM_PRIVATE_OBJECT(GMGlyphObject)
{
	GMString lastRenderText;
	GMString text;
	bool autoResize = true; // 是否按照屏幕大小调整字体
	GMRectF lastGeometry = { 0, 0, -1, -1 };
	GMRect lastClientRect = { 0, 0, -1, -1 };
	ITexture* texture;
	ITypoEngine* typoEngine = nullptr;
	bool insetTypoEngine = true;
};

class GMGlyphObject : public GMControlGameObject
{
	DECLARE_PRIVATE_AND_BASE(GMGlyphObject, GMControlGameObject)
	GM_FRIEND_CLASS(GMImage2DGameObject)

private:
	GMGlyphObject();

public:
	GMGlyphObject(ITypoEngine* typo);
	~GMGlyphObject();

public:
	void setText(const GMString& text);
	void update();

public:
	inline void setAutoResize(bool b) { D(d); d->autoResize = b; }

public:
	virtual void onAppendingObjectToWorld() override;
	virtual void draw() override;

private:
	void constructModel();
	void updateModel();
	void createVertices(GMMesh* mesh);
	void onCreateShader(GMShader& shader);

protected:
	virtual void updateUI() {} //Ignore base
};

//////////////////////////////////////////////////////////////////////////
enum {
	BorderAreaCount = 9,
};

GM_PRIVATE_OBJECT(GMImage2DBorder)
{
	GMAsset texture;
	GMRect borderTextureGeometry;
	GMModel *models[BorderAreaCount] = { nullptr };
	GMGameObject *objects[BorderAreaCount] = { nullptr };
	GMfloat width;
	GMfloat height;
	GMfloat cornerWidth;
	GMfloat cornerHeight;
};

// 表示一个2D边框。
class GMImage2DBorder : public GMObject
{
	DECLARE_PRIVATE(GMImage2DBorder)
	GM_FRIEND_CLASS(GMImage2DGameObject)

private:
	GMImage2DBorder() = default;

public:
	GMImage2DBorder(GMAsset texture,
		const GMRect& borderTextureGeometry,
		GMfloat textureWidth,
		GMfloat textureHeight,
		GMfloat cornerWidth,
		GMfloat cornerHeight
	);
	~GMImage2DBorder();

private:
	bool hasBorder() { D(d); return !!d->texture.asset; }
	// 截取的纹理
	const GMRect& textureGeometry() { D(d); return d->borderTextureGeometry; }
	template <typename T, GMint Size> void release(T* (&)[Size]);
	template <GMint Size> void drawObjects(GMGameObject* (&)[Size]);

private:
	virtual void clone(const GMImage2DBorder& border);
	virtual void createBorder(const GMRect& geometry);
	virtual void draw();
	virtual void setScaling(const GMMat4& scaling);
	virtual void setTranslation(const GMMat4& translation);
	virtual void setRotation(const GMQuat& rotation);
};

//////////////////////////////////////////////////////////////////////////
GM_PRIVATE_OBJECT(GMImage2DGameObject)
{
	ITexture* image = nullptr;
	AUTORELEASE GMGlyphObject* textModel = nullptr;
	AUTORELEASE GMControlGameObject* textMask = nullptr;
	AUTORELEASE GMControlGameObject* background = nullptr;
	GMString text;
	GMImage2DBorder border;
	GMint paddings[4] = { 0 };
};

class GMImage2DGameObject : public GMControlGameObject, public IPrimitiveCreatorShaderCallback
{
	DECLARE_PRIVATE_AND_BASE(GMImage2DGameObject, GMControlGameObject)

public:
	using GMControlGameObject::GMControlGameObject;
	~GMImage2DGameObject();

public:
	// GMImage2DGameObject采用的盒模型是，width和height表示边框的宽度和高度，内容部分应该减去相应的padding
	void setPaddings(GMint left, GMint top, GMint right, GMint bottom);
	void setImage(GMAsset& asset);
	void setText(const GMString& text);
	void setBorder(const GMImage2DBorder& border);

public:
	virtual void onAppendingObjectToWorld() override;
	virtual void draw() override;
	virtual void setScaling(const GMMat4& scaling) override;
	virtual void setTranslation(const GMMat4& translation) override;
	virtual void setRotation(const GMQuat& rotation) override;

	//IPrimitiveCreatorShaderCallback
protected:
	virtual void onCreateShader(GMShader& shader) override;

private:
	void createBackgroundImage();
	void createBorder();
	void createGlyphs();
};

//////////////////////////////////////////////////////////////////////////
GM_PRIVATE_OBJECT(GMListbox2DGameObject)
{
	GMint itemMargins[4] = { 0 };
};

class GMListbox2DGameObject : public GMImage2DGameObject
{
	DECLARE_PRIVATE_AND_BASE(GMListbox2DGameObject, GMImage2DGameObject);

public:
	GMImage2DGameObject* addItem(const GMString& text);
	void setItemMargins(GMint left, GMint top, GMint right, GMint bottom);

public:
	virtual void onAppendingObjectToWorld();

private:
	virtual void onCreateShader(GMShader& shader) override;
	virtual void draw() override;
	virtual void notifyControl() override;

private:
	inline const Vector<GMControlGameObject*>& getItems() { D_BASE(db, GMControlGameObject); return db->children; }
};
END_NS
#endif
