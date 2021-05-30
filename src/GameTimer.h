//================================================================================

#pragma once

//================================================================================

#include "Object.h"

//================================================================================

namespace Gfx {

//--------------------------------------------------------------------------------

class GameTimer : public Object {
public:
	GameTimer();

public:
	void reset();
	void split();
	void stop();

public:
	void onStart() override;
	void onUpdate( sf::Time deltaTime ) override;
	void onRender() override;

protected:
	bool m_active{ true };

	sf::Text m_timerText;
	sf::Text m_splitText;
	sf::Font m_font;

	vector< sf::Time > m_splits;

	sf::Clock m_clock;
};

//--------------------------------------------------------------------------------

} // Gfx

//================================================================================