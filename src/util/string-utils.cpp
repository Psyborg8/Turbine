//================================================================================

#include "string-utils.h"

//================================================================================

namespace Utils {

//--------------------------------------------------------------------------------

vector< string > tokenize( const string& str, const string& delim, bool fullDelim ) {
    vector< string > out;

    if( fullDelim ) {
        size_t pos = str.find( delim );
        while( pos != string::npos ) {
            pos += delim.size();
            size_t nextPos = str.find( delim, pos );
            if( nextPos == string::npos )
                out.push_back( string( str, pos, str.size() - pos ) );
            else
                out.push_back( string( str, pos, nextPos - pos ) );

            pos = nextPos;
        }
    }
    else {
        size_t start, end = 0;
        while( end < str.size() ) {
            start = end;
            while( start < str.size() && ( delim.find( str[ start ] ) != string::npos ) ) {
                start++;  // skip initial whitespace
            }
            end = start;
            while( end < str.size() && ( delim.find( str[ end ] ) == string::npos ) ) {
                end++; // skip to end of word
            }
            if( end - start != 0 ) {  // just ignore zero-length strings.
                out.push_back( string( str, start, end - start ) );
            }
        }
    }

    return out;
}

string format( const char* fmt... ) {
    std::string s{};
    va_list args, args2;
    va_start( args, fmt );
    va_copy( args2, args );

    s.resize( vsnprintf( nullptr, 0, fmt, args2 ) + 1 );
    va_end( args2 );
    vsprintf( s.data(), fmt, args );
    va_end( args );
    s.pop_back();
    return s;
}

//--------------------------------------------------------------------------------

}

//================================================================================