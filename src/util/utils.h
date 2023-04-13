// ======================================================================

#ifndef _turbine_utils_h
#define _turbine_utils_h

// ======================================================================

#include "global.h"

// ======================================================================

template< class T, size_t depth >
class Counter {
public:
	Counter() { std::fill( m_data.begin(), m_data.end(), T( 0 ) ); }

public:
	void push( T _elem ) {
		m_data[m_count] = _elem;
		if( ++m_count > m_data.size() )
			m_count = 0u;
	}

	T average() {
		T total = T( 0 );
		size_t count;

		for( T elem : m_data ) {
			if( elem != T( 0 ) ) {
				total += elem;
				count++;
			}
		}

		return total / count;
	}
	T max() {
		T max = m_data[0];

		for( T elem : m_data )
			if( elem > max )
				max = elem;
	}
	T min() {
		T min = m_data[0];

		for( T elem : m_data )
			if( elem < min )
				min = elem;

		return min;
	}

	const array< T, depth >& data() { return m_data; }

private:
	array< T, depth > m_data;
	size_t m_count{ 0u };
};

// ======================================================================

#endif

// ======================================================================
