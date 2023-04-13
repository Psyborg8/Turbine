//================================================================================

#pragma once

//================================================================================

#include "global.h"
#include "world.h"

//================================================================================

namespace Editor {

//--------------------------------------------------------------------------------

class EditorWindow {
public:
	EditorWindow() { m_id = s_nextID++; }
	virtual ~EditorWindow() = default;

public:
	virtual void init( World* world ) {}
	virtual void open() {}
	virtual void update( sf::Time delta ) {}
	virtual void renderLeftWindow() {}
	virtual void renderRightWindow() {}
	virtual void close() {}
	virtual void save() {}
	virtual void onEvent( sf::Event e ) {}
	virtual void rename( string name ) {}
	virtual string getName() const { return string(); }

	int getID() const { return m_id; }

private:
	int m_id;

	static int s_nextID;
};

//--------------------------------------------------------------------------------

}	 // namespace Editor

//================================================================================
