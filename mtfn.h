/* Copyright (c) 2015 Michael Hamilton.
 *  
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with teh License.
 * You may obtain a copy of the License at
 *
 *          http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License
 *
 * class sound - implements double metaphone.
 *
 * Based on the original reference implementation by Lawrence Philips and
 * the Java implementation in org.apache.commons.codec.language.DoubleMetaphone
 */

#ifndef __MTFN_H__
#define __MTFN_H__

#include <string>

namespace mtfn
{

const int stop_len = 4;

class sound
{
public:
    // str coded in ASCII or ISO-8859-15 (if it includes a "ç" or "ñ" in it)
    sound( const std::string& str, bool limit_length = true );

    // This takes a wstring, but it assumes that all glyphs are in the range
    // [A-Za-zÇçÑñ ], which are the characters typically used in English names.
    // any glyph outside that range is skipped
    sound( const std::wstring& wstr, bool limit_length = true );

    // Copy constructor
    sound( const sound& init )
    : m_name( init.m_name ),
      m_first( init.m_first ),
      m_last( init.m_last ),
      m_cursor( init.m_cursor ),
      m_has_alternate( init.m_has_alternate ),
      m_primary( init.m_primary ),
      m_alternate( init.m_alternate ),
      m_length_limited( init.m_length_limited )
    {};

    // Assignment operator
    const sound& operator =( const sound& init )
    {
        m_primary = init.m_primary;
        m_alternate = init.m_alternate;
        m_name = init.m_name;
        m_cursor = init.m_cursor;
        m_first = init.m_first;
        m_last = init.m_last;
        m_has_alternate = init.m_has_alternate;
        m_length_limited = init.m_length_limited;

        return *this;
    };

    // Equality test returns true if the sounds are pronounced the same way
    bool operator ==( const sound& rhs ) const
    {
        return ( m_primary == rhs.m_primary ) ||
               ( rhs.m_has_alternate && m_primary == rhs.m_alternate ) ||
               ( m_has_alternate && rhs.m_has_alternate && m_alternate == rhs.m_alternate ) ||
               ( m_has_alternate && m_alternate == rhs.m_primary );
    };

    // Works for std::string and std::wstring, based on the two available
    // constructors on mtfn::sound
    // Usage: sound snd( "Needle" );
    //        if ( snd == "Haystack" )
    //           ...
    template <typename STRING>
    bool operator ==( const STRING& rhs ) const
    {
        return *this == sound( rhs, m_length_limited );
    };

    // Primary English pronounciation in America
    const std::string& primary( void ) const { return m_primary; };

    // Alternate English pronounciation in America, returns an empty
    // string if this->has_alternate() == false.
    const std::string& alternate( void ) const { return m_alternate; };

    // Returns true if there is an alternate pronounciation
    const bool has_alternate( void ) const { return m_has_alternate; };

protected:
    void add( char c )
    {
        m_primary += c;
        m_alternate += c;
    };

    void add( const std::string& s )
    {
        m_primary.append( s );
        m_alternate.append( s );
    };

    void add( char c, char a )
    {
        m_has_alternate = true;
        m_primary += c;
        m_alternate += a;
    }

    void add( const std::string& s, const std::string& a )
    {
        m_has_alternate = true;
        m_primary.append( s );
        m_alternate.append( a );
    }

    bool is_ready( void )
    {
        if ( m_cursor > m_last )
        {
            return true;
        }

        if ( m_length_limited )
        {
            return m_primary.size() >= stop_len &&
                   m_alternate.size() >= stop_len;
        }

        return false;
    };

    bool is_slavo_germanic( void );
    bool is_spanish_ll( void );
    bool is_germanic_c( void );
    bool starts_german( void );

    static bool is_one_of( char needle, const std::string& haystack );
    static bool is_one_of( const std::string& needle,
        const char* haystack, ... );
    static bool is_one_of( const std::string::const_iterator& needle_start,
        int count, const char* haystack, ... );

    void vowel( void );
    void letter_b( void );
    void letter_c_cedilla( void );
    void letter_c( void );
    void letter_combo_ch( void );
    void letter_combo_cc( void );
    void letter_d( void );
    void letter_f( void );
    void letter_g( void );
    void letter_combo_gh( void );
    void letter_h( void );
    void letter_j( void );
    void letter_k( void );
    void letter_l( void );
    void letter_m( void );
    void letter_n( void );
    void letter_n_tilde( void );
    void letter_p( void );
    void letter_q( void );
    void letter_r( void );
    void letter_s( void );
    void letter_t( void );
    void letter_v( void );
    void letter_w( void );
    void letter_x( void );
    void letter_z( void );

    // m_name is padded on the left and right with several '_' characters
    // to allow easy reference to earlier and later characters, or to
    // detect the start or end of the name without comparing the 
    // cursor to m_first or m_last.
    std::string m_name;

    std::string::const_iterator m_first;
    std::string::const_iterator m_last;
    std::string::const_iterator m_cursor;

    bool m_has_alternate;
    std::string m_primary;
    std::string m_alternate;

    bool m_length_limited;
private:
};

}; // namespace mtfn

// This lets you compare the sound of a std::string with a std::wstring, 
// a std::string with a std::string, or a std::wstring with a std::wstring
template <typename STRA, typename STRB>
bool sounds_like( const STRA& lhs, const STRB& rhs, bool limit_length = true )
{
    return sound( lhs, limit_length ) == sound( rhs, limit_length );
}


#endif
