//================================================================================

#pragma once

//================================================================================

#include "RigidRect.h"

//================================================================================

namespace Game {

//================================================================================

class Checkpoint : public RigidRect {
public:
	Checkpoint() = default;

// Events
public:
	void onSpawnChildren() override;
	void onStart() override;
	void onProcessCollisions() override;

// Get-Set
public:
	inline size_t getNumber() const { return m_number; }
	inline void setNumber( size_t number ) { m_number = number; }

	inline bool isHit() { return m_flag->fill; }

private:
	shared_ptr< Game::RigidRect > m_flag;
	shared_ptr< Game::RigidRect > m_collider;
	size_t m_number;
};

//================================================================================

} // Game

//================================================================================