#include <iostream>
#include <unordered_set>
#include <memory>
#include <string>
#include <sstream>
#include <cassert>

struct vec2
{
	vec2(float x = 0.0f, float y = 0.0f) : x(x), y(y) {}
	const float x;
	const float y;
};

class Shape
{
public:
	virtual bool contains(const vec2 &pos) const = 0;
	virtual void print() const = 0;
};

class CircleShape : public Shape
{
public:
	CircleShape(const vec2 &center, float radius) : _center(center), _radius(radius) {}
	
	virtual bool contains(const vec2 &pos) const
	{
		auto square = [](float x) { return x * x; };
		return square(pos.x - _center.x) + square(pos.y - _center.y) < square(_radius);
	}

	virtual void print() const
	{
		std::cout << "Circle{center(" << _center.x << ", " << _center.y;
		std::cout << "), radius(" << _radius << ")}" << std::endl;
	}

private:
	const vec2 _center;
	const float _radius;
};

class RectangleShape : public Shape
{
public:
	RectangleShape(const vec2 center, const vec2 halfsize) : _center(center), _halfsize(halfsize) {}

	virtual bool contains(const vec2 &pos) const
	{
		if(pos.x > (_center.x + _halfsize.x)) return false;
		if(pos.x < (_center.x - _halfsize.x)) return false;
		if(pos.y > (_center.y + _halfsize.y)) return false;
		if(pos.y < (_center.y - _halfsize.y)) return false;
		return true;
	}

	virtual void print() const
	{
		std::cout << "Rectangle{center(" << _center.x << ", " << _center.y;
		std::cout << "), halfsize(" << _halfsize.x << ", " << _halfsize.y;
		std::cout << ")}" << std::endl;
	}

private:
	const vec2 _center;
	const vec2 _halfsize;
};

class CompositeShape : public Shape
{
public:
	typedef std::unordered_set<std::shared_ptr<Shape> > ChildSet;

	inline void addChild(std::shared_ptr<Shape> child)
	{
		_children.insert(child);
	}

	inline void removeChild(std::shared_ptr<Shape> child)
	{
		_children.erase(child);
	}

	virtual bool contains(const vec2 &pos) const
	{
		for(auto &child : _children)
		{
			if(child->contains(pos))
				return true;	
		}

		return false;
	}

	virtual void print() const
	{
		for(auto &child : _children)
		{
			child->print();
		}
	}

private:
	ChildSet _children;	
};


int main(int argc, char **argv)
{
	auto root = std::make_shared<CompositeShape>();
	root->addChild(std::make_shared<CircleShape>(vec2(1.0f, 0.0f), 1.0f));
	root->addChild(std::make_shared<RectangleShape>(vec2(), vec2(0.5f, 0.5f)));

	root->print();
	std::cout << std::endl;

	std::cout << "Inside rectangle: " << root->contains(vec2(0.0f, 0.0f)) << std::endl;
	std::cout << "Inside circle: " << root->contains(vec2(1.1f, 0.0f)) << std::endl;
	std::cout << "Outside both: " << root->contains(vec2(3.0f, 3.0f)) << std::endl;

	std::system("pause");
	return 0;
}
