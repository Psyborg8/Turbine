//================================================================================

#pragma once

//================================================================================

template < class T >
class RandomNumbers : public stack< T > {
public:
	inline RandomNumbers() : stack< T >() {}

public:
	inline T topnpop() { T out = stack< T >::top(); stack< T >::pop(); return out; }
};

//================================================================================

class Random {
public:
	Random();
	Random( unsigned int seed );

public:
	int getRandomIntInRange( int min, int max ) const;
	int getRandomIntInRange( int min, int max, function< int( int randomNumber ) > func ) const;
	RandomNumbers< int > getRandomIntInRange( int min, int max, size_t count ) const;
	RandomNumbers< int > getRandomIntInRange( int min, int max, size_t count, function< int( int randomNumber ) > func ) const;

	float getRandomFloatInRange( float min, float max ) const;
	float getRandomFloatInRange( float min, float max, function< float( float randomNumber ) > func ) const;
	RandomNumbers< float > getRandomFloatInRange( float min, float max, size_t count ) const;
	RandomNumbers< float > getRandomFloatInRange( float min, float max, size_t count, function< float( float randomNumber ) > func ) const;

	inline const long long& getSeed() const { return m_seed; }
	inline void setSeed( int seed ) { m_seed = seed; srand( seed ); }

private:
	unsigned int m_seed;
};

//================================================================================