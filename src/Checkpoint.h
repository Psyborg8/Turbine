//================================================================================

#pragma once

//================================================================================

#include "RigidRect.h"

//================================================================================

class Checkpoint : RigidRect {
public:
	Checkpoint() = default;

public:
	size_t getNumber() { return m_number; }
	void setNumber( size_t number ) { m_number = number; }

private:
	size_t m_number;
};