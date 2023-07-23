#include "vectors.h"
#include "../client/activeplayer.h"

class Entity
{
public:
	Entity() {};
	Entity(vec2 pos) { this->pos = pos; };
	Entity(int id, vec2 pos) { this->id = id; this->pos = pos; };

public:
	int id = 0;
	vec2 pos{ 0,0 };

private:
	vec2 m_relPos{ 0,0 };
	vec2 m_offset{ 0,0 };
};


class LocalPlayer : public Entity
{
public:
	LocalPlayer() : Entity() {};
	LocalPlayer(vec2 pos) : Entity(pos) {};

public:
	championStats stats;
	vec2 vScreenAARange{ 0,0 };

public:
	bool IsInside(Entity e, float threshold = 1.0f) {
		float r = (e.pos.x - pos.x) * (e.pos.x - pos.x) / (vScreenAARange.x * vScreenAARange.x)
			+ (e.pos.y - pos.y) * (e.pos.y - pos.y) / (vScreenAARange.y * vScreenAARange.y);
		if (r <= threshold)
			return true;
		return false;
	}
};


class Enemy : public Entity
{
public:
	Enemy() : Entity() {};
	Enemy(int id, vec2 pos) : Entity(id, pos) {};

public:
	vec2 direction{ 0,0 };
	float magnitude = 0.0f;
	float speed = 0.0f;

public:
	vec2 Predict(float fPredictTime) {
		if (magnitude == 0.0f || magnitude > 15.0f)
			return pos;
		return pos + ((direction / magnitude) * speed * fPredictTime);
	}
};

class Minion : public Entity
{
public:
	Minion() : Entity() {};
	Minion(vec2 pos, float health) : Entity(pos) {
		this->health = health;
	};

public:
	float health;
};