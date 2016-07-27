#pragma once

#include "Colors.h"
#include "IMoveable.h"
#include "Drawable.h"

class GameObject :public IDrawable, public IMoveable
{
public:

	GameObject() = default;
	virtual ~GameObject() = default;

	virtual void SetPosition(int X, int Y) override;
	virtual void SetSize(int Width, int Height) override;
	virtual void SetColor(Color C) override;

	virtual std::pair<int, int> GetPosition()const override;
	virtual std::pair<int, int> GetSize()const override;
	virtual Color GetColor()const override;
};

