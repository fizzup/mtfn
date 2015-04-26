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
 */

#include <string>
#include <cassert>
#include "mtfn.h"

using namespace std;
using namespace mtfn;

#define is_vowel(a) (is_one_of( (a), "AEIOUY" ))

const int padding_len = 5;

// Define the allowed special characters in iso-8859-1 / UCS-2
const char sm_c_cedilla = 0xe7;
const char cap_c_cedilla = sm_c_cedilla - 0x20;
const char sm_n_tilde = 0xf1;
const char cap_n_tilde = sm_n_tilde - 0x20;

sound::sound( const string& str, bool limit_length )
: m_name( string( padding_len, '_' ) + str + string( padding_len, '_' ) ),
  m_first( m_name.begin() + padding_len ),
  m_last( m_name.end() - padding_len - 1 ),
  m_cursor( m_first ),
  m_has_alternate( false ),
  m_primary( "" ),
  m_alternate( "" ),
  m_length_limited( limit_length )
{
    // Convert to upper case, remove any unexpected characters
    for( string::iterator i = m_name.begin() + padding_len;
         i != m_name.end() - padding_len;
         i++ )
    {
        if ( ( 'A' <= *i && *i <= 'Z' ) || *i == ' ' ||
             *i == cap_c_cedilla || *i == cap_n_tilde )
        {
            continue;
        }
        else if ( ( 'a' <= *i && *i <= 'z' ) ||
                    *i == sm_c_cedilla || *i == sm_n_tilde )
        {
            *i = *i - 0x20;
        }
        else
        {
            i = m_name.erase( i );
        }
    }

    // Skip silent letters at the start of a word.
    if ( is_one_of( m_cursor, 2, "GN", "KN", "PN", "WR", "PS", NULL ) )
    {
        m_cursor += 1;
    }

    while ( !is_ready() )
    {
        switch ( *m_cursor )
        {
            case 'A':
            case 'E':
            case 'I':
            case 'O':
            case 'U':
            case 'Y':
                vowel();
                break;
            case 'B':
                letter_b();
                break;
            case cap_c_cedilla:
                letter_c_cedilla();
                break;
            case 'C':
                letter_c();
                break;
            case 'D':
                letter_d();
                break;
            case 'F':
                letter_f();
                break;
            case 'G':
                letter_g();
                break;
            case 'H':
                letter_h();
                break;
            case 'J':
                letter_j();
                break;
            case 'K':
                letter_k();
                break;
            case 'L':
                letter_l();
                break;
            case 'M':
                letter_m();
                break;
            case 'N':
                letter_n();
                break;
            case cap_n_tilde:
                letter_n_tilde();
                break;
            case 'P':
                letter_p();
                break;
            case 'Q':
                letter_q();
                break;
            case 'R':
                letter_r();
                break;
            case 'S':
                letter_s();
                break;
            case 'T':
                letter_t();
                break;
            case 'V':
                letter_v();
                break;
            case 'W':
                letter_w();
                break;
            case 'X':
                letter_x();
                break;
            case 'Z':
                letter_z();
                break;
            default:
                m_cursor++;
                break;
        }
    }

    if ( limit_length && m_primary.size() > stop_len )
    {
        m_primary = string( m_primary.begin(), m_primary.begin() + stop_len );
    }

    if ( !m_has_alternate )
    {
        m_alternate = string();
    }

    if ( limit_length && m_alternate.size() > stop_len )
    {
        m_alternate =
            string( m_alternate.begin(), m_alternate.begin() + stop_len );
    }
}

sound::sound( const wstring& wstr, bool limit_length )
{
    string str( "" );

    for ( wstring::const_iterator i = wstr.begin();
          i != wstr.end();
          i++ )
    {
        if ( ( L'A' <= *i && *i <= L'Z' ) ||
             ( L'a' <= *i && *i <= L'z' ) ||
             ( *i == L' ' ) ||
             ( *i == cap_c_cedilla ) ||
             ( *i == sm_c_cedilla ) ||
             ( *i == cap_n_tilde ) ||
             ( *i == sm_n_tilde ) )
        {
            str += (char)*i;
        }
    }

    sound( str, limit_length );
}

bool sound::is_slavo_germanic( void )
{
    return ( m_name.find_first_of( 'W' ) != string::npos ||
         m_name.find_first_of( 'K' ) != string::npos ||
         m_name.find( "CZ" ) != string::npos ||
         m_name.find( "WITZ" ) != string::npos );
}

bool sound::is_spanish_ll( void )
{
    string::const_iterator& c( m_cursor );

    if ( c == m_last - 2 && is_one_of( c-1, 4, "ILLO", "ILLA", "ALLE", NULL ) )
    {
        return true;
    }
    else if ( ( is_one_of( m_last - 1, 2, "AS", "OS", NULL ) ||
              is_one_of( *m_last, "AO" ) )
              && string( c-1, c+3 ) == "ALLE" ) 
    {
        return true;
    }
    else
    {
        return false;
    }
}

bool sound::starts_german( void )
{
    return is_one_of( m_first, 4, "VAN ", "VON ", NULL ) ||
           string( m_first, m_first+3 ) == "SCH";
}

bool sound::is_germanic_c( void )
{
    string::const_iterator& c( m_cursor );

    return ( c > m_first+1 &&
         !is_vowel( *(c-2) ) &&
         string( c-1, c+2 ) == "ACH" &&
         !is_one_of( *(c+2), "IE" ) ) ||
         is_one_of( c-2, 6, "BACHER", "MACHER", NULL );
}

//:TRICKY va_start on const string& is undefined, so the first 
// haystack has to be a named parameter.
bool sound::is_one_of( const string& needle, const char* haystack, ... )
{
    va_list ap;
    va_start( ap, haystack );

    bool found = false;
    do
    {
        if ( !found && needle == haystack )
        {
            found = true;
        }
    } while ( ( haystack = (char*)va_arg( ap, char* ) ) != NULL );

    va_end( ap );
    return found;
}

//:TRICKY same trick as for the std::string version
bool sound::is_one_of( const string::const_iterator& beg, int count,
        const char* haystack, ... )
{
    string needle( beg, beg + count );
    va_list ap;
    va_start( ap, haystack );

    bool found = false;
    do
    {
        if ( !found && needle == haystack )
        {
            found = true;
        }

        assert( strlen( haystack ) == count );
    } while ( ( haystack = (char*)va_arg( ap, char* ) ) != NULL );

    va_end( ap );
    return found;
}

bool sound::is_one_of( char needle, const string& haystack )
{
    return ( haystack.find_first_of( needle ) != string::npos );
}

void sound::vowel( void )
{
    string::const_iterator& c( m_cursor );

    if ( c == m_first )
    {
        add( 'A' );
    }

    c++;
}

void sound::letter_b( void )
{
    string::const_iterator& c( m_cursor );

    // "-mb", e.g., "dumb" already skipped over...
    add( 'P' );

    // 'BB' sounds the same as 'B'
    if ( *(c+1) == 'B' )
    {
        c += 2;
    }
    else
    {
        c += 1;
    }
}

void sound::letter_c_cedilla( void )
{
    string::const_iterator& c( m_cursor );

    // Ç sounds like 'S'
    add( "", "S" );
    c++;
}

void sound::letter_c( void )
{
    string::const_iterator& c( m_cursor );

    if ( is_germanic_c() )
    {  
        add( 'K' );
        c += 2;
    }
    else if ( c == m_first && string( c, c+6) == "CAESAR" )
    {
        add( 'S' );
        c += 2;
    }
    else if ( string( c, c+4 ) == "CHIA" )
    {
        add( 'K' );
        c += 2;
    }
    else if ( string( c, c+2 ) == "CH" )
    {
        letter_combo_ch();
    }
    else if ( string( c, c+2 ) == "CZ" &&
              string( c-2, c+2 ) != "WICZ" ) 
    {
        // 'czar'
        add( 'S', 'X' );
        c += 2;
    }
    else if ( string( c+1, c+4 ) == "CIA" )
    {
        // italian like 'focaccia'
        add( 'X' );
        c += 3;
    }
    else if ( string( c, c+2 ) == "CC"  && string( c-1, c+2 ) != "MCC" )
    {
        // double "cc" but not "McClelland"
        return letter_combo_cc();
    }
    else if ( is_one_of( c, 2, "CK", "CG", "CQ", NULL ) )
    {
        add( 'K' );
        c += 2;
    }
    else if ( is_one_of( c, 2, "CI", "CE", "CY", NULL ) )
    {
        //-- Italian vs. English --//
        if ( is_one_of( c, 3, "CIO", "CIE", "CIA", NULL ) )
        {
            add('S', 'X');
        }
        else
        {
            add('S');
        }
        c += 2;
    }
    else
    {
        add( 'K' );
        if ( is_one_of( c+1, 2, " C", " Q", " G", NULL ))
        {
            //-- Mac Caffrey, Mac Gregor --//
            c += 3;
        }
        else if ( is_one_of( *(c+1), "CKQ") &&
                  !is_one_of( c+1, 2, "CE", "CI", NULL ) )
        {
            c += 2;
        }
        else
        {
            c += 1;
        }
    }
}

void sound::letter_combo_ch( void )
{
    string::const_iterator& c( m_cursor );

    if ( c > m_first && string( c, c+4 ) == "CHAE" )
    {
        // michael
        add( 'K', 'X' );
        c += 2;
    }
    else if ( c == m_first && string( c, c+5 ) != "CHORE" &&
            ( is_one_of( c+1, 5, "HARAC", "HARIS", NULL ) ||
              is_one_of( c+1, 3, "HOR", "HYM", "HIA", "HEM", NULL ) ) )
    {
        // words with greek roots, e.g. 'chemistry', 'chorus'
        add( 'K' );
        c += 2;
    }
    else if ( ( is_one_of( m_first, 4, "VAN ", "VON ", NULL ) ||
                string( m_first, m_first+3 ) == "SCH" ) ||
                is_one_of( c-2, 6, "ORCHES", "ARCHIT", "ORCHID", NULL ) ||
                is_one_of( *(c+2), "TS" ) ||
                ( is_one_of( *(c-1), "AOUE_" ) && 
                  is_one_of( *(c+2), "LRNMBHFVW _" ) ) )
    {
        // germanic, greek, or otherwise 'ch' for 'kh'
        add( 'K' );
        c += 2;
    }
    else 
    {
        if ( c > m_first )
        {
            if ( string( m_first, m_first + 2 ) == "MC" )
            {
                // 'mchugh'
                add('K');
            }
            else
            {
                add('X', 'K');
            }
        }
        else
        {
            add ( 'X' );
        }
        c += 2;
    }
}

void sound::letter_combo_cc( void )
{
    string::const_iterator& c( m_cursor );

    // 'bellocchio' but not 'bacchus'
    if ( is_one_of( *(c+2), "IEH" ) && string( c+2, c+4 ) != "HU" )
    {
        //'accident', 'accede' 'succeed'
        if ( ( c == m_first + 1 && *(c-1) == 'A' ) ||
             is_one_of( c-1, 5, "UCCEE", "UCCES", NULL ) )
        {
            add( "KS" );
        }
        //'bacci', 'bertucci', other italian
        else
        {
            add( 'X' );
        }
        c += 3;
    }
    else
    {
        add( 'K' );
        c+= 2;
    }
}

void sound::letter_d( void )
{
    string::const_iterator& c( m_cursor );

    if ( string( c, c+2 ) == "DG" )
    {
        if ( is_one_of( *(c+2), "IEY" ) )
        {
            //e.g. 'edge'
            add( 'J' );
            c += 3;
        }
        else
        {
            //e.g. 'edgar'
            add( "TK" );
            c += 2;
        }
    }
    else
    {
        // 'DT' and 'DD' sound the same as 'D'
        if ( is_one_of( c, 2, "DT", "DD", NULL ) )
        {
            c += 2;
        }
        else
        {
            c += 1;
        }
        add( 'T' );
    }
}

void sound::letter_f( void )
{
    string::const_iterator& c( m_cursor );

    // 'FF' sounds the same as 'F'
    if ( *(c+1) == 'F' )
    {
        c += 2;
    }
    else
    {
        c += 1;
    }

    add( 'F' );
}

void sound::letter_g( void )
{
    string::const_iterator& c( m_cursor );

    if ( *(c+1) == 'H' )
    {
        letter_combo_gh();
    }
    else if ( *(c+1) == 'N' )
    {
        if ( c == m_first+1 && is_vowel( *m_first ) && !is_slavo_germanic() )
        {
            add( "KN", "N" );
        }
        else if ( string( c+2, c+4 ) != "EY" && *(c+1) != 'Y' &&
                 !is_slavo_germanic() )
        {
            //not e.g. 'cagney'
            add( "N", "KN" );
        }
        else
        {
            add( "KN" );
        }

        c+= 2;
    }
    else if ( string( c+1, c+3 ) == "LI" && !is_slavo_germanic() )
    {
        //'tagliaro'
        add( "KL", "L" );
        c += 2;
    }
    else if ( c == m_first &&
         ( *(c+1) == 'Y' ||
         is_one_of( c+1, 2, "ES", "EP", "EB", "EL", "EY", "IB", "IL", "IN", "IE", "EI", "ER", NULL ) ) )
    {
        // -ges-,-gep-,-gel-, -gie- at beginning
        add( 'K', 'J' );
        c += 2;
    }
    else if ( ( string( c+1, c+3 ) == "ER" || *(c+1) == 'Y' ) &&
         !is_one_of( m_first, 6, "DANGER", "RANGER", "MANGER", NULL ) &&
         !is_one_of( *(c-1), "EI" ) &&
         !is_one_of( c-1, 3, "RGY", "OGY", NULL ) )
    {
        // -ger-,  -gy-
        add( 'K', 'J' );
        c += 2;
        return;
    }
    else if ( is_one_of( *(c+1), "EIY" ) ||
         is_one_of( c-1, 4, "AGGI", "OGGI", NULL ) )
    {
        // italian e.g, 'biaggi'
        //obvious germanic
        if ( is_one_of( m_first, 4, "VAN ", "VON ", NULL ) ||
             string( m_first, m_first+3 ) == "SCH" ||
             string( c+1, c+3 ) == "ET" )
        {
            add( 'K' );
        }
        else
        {
            //always soft if french ending
            if ( string( c+1, c+5) == "IER_" )
            {
                add( 'J' );
            }
            else
            {
                add( 'J', 'K' );
            }
        }

        c += 2;
    }
    else if ( *(c+1) == 'G')
    {
        add( 'K' );
        c += 2;
    }
    else
    {
        add( 'K' );
        c += 1;
    }
}

void sound::letter_combo_gh( void )
{
    string::const_iterator& c( m_cursor );

    if ( c > m_first && !is_vowel( *(c-1) ) )
    {
        add( 'K' );
        c += 2;
    }
    else if ( c == m_first )
    {
        if ( *(c+2) == 'I' )
        {
            add( 'J' );
        }
        else
        {
            add( 'K' );
        }
        c += 2;
    }
    else if ( is_one_of( *(c-2), "BHD" ) || is_one_of( *(c-3), "BHD" ) ||
         is_one_of( *(c-4), "BH" ) )
    {
        // Parker's rule (with some further refinements) - e.g., 'hugh'
        c += 2;
    }
    else
    {
        //e.g., 'laugh', 'McLaughlin', 'cough', 'gough', 'rough', 'tough'
        if ( c > m_first + 2  &&
             *(c-1) == 'U' &&
             is_one_of( *(c-3), "CGLRT" ) )
        {
            add( 'F' );
        }
        else if ( c > m_first && *(c-1) != 'I' )
        {
            add( 'K' );
        }

        c += 2;
    }
}

void sound::letter_h( void )
{
    string::const_iterator& c( m_cursor );

    if ( ( c == m_first || is_vowel( *(c-1) ) ) && is_vowel( *(c+1) ) )
    {
	// keep any h that looks like '^h[aeiouy]' or '[aeiouy]h[aeiouy]'
        add( 'H' );
        c += 2;
    }
    else
    {
        c += 1;
    }
}

void sound::letter_j( void )
{
    string::const_iterator& c( m_cursor );

    if ( string( c, c+4 ) == "JOSE"  ||
         string( m_first, m_first+4 ) == "SAN " )
    {
	// obvious spanish, 'jose', 'san jacinto'
        if ( ( ( c == m_first && *(c+4) == ' ' ) ||
	     m_last - m_first == 3 ) ||
             string( m_first, m_first + 4 ) == "SAN " )
        {
            add( 'H' );
        }
        else
        {
            add( 'J', 'H' );
        }

        c += 1;
    }
    else if ( c == m_first && string( c, c+4 ) != "JOSE" )
    {
        add( 'J', 'A' );
    }
    else if ( is_vowel( *(c-1) ) && !is_slavo_germanic() &&
             is_one_of( *(c+1), "AO" ) )
    {
        // spanish pron. of e.g. 'bajador'
        add( 'J', 'H' );
    }
    else if ( c == m_last )
    {
        add( "J", "" );
    }
    else if ( !is_one_of( *(c+1), "LTKSNMBZ" ) &&
              !is_one_of( *(c-1), "SKL" ) )
    {
        add( 'J' );
    }

    if ( *(c+1) == 'J' ) //it could happen!
    {
        c += 2;
    }
    else
    {
        c += 1;
    }
}

void sound::letter_k( void )
{
    string::const_iterator& c( m_cursor );

    if ( *(c+1) == 'K' )
    {
        c += 2;
    }
    else
    {
        c += 1;
    }

    add( 'K' );
}

void sound::letter_l( void )
{
    string::const_iterator& c( m_cursor );

    if ( *(c+1) == 'L' )
    {
        //spanish e.g. 'cabrillo', 'gallegos'
        if ( is_spanish_ll() )
        {
            add( "L", "" );
        }
        else
        {
            add( 'L' );
        }
        c += 2;
    }
    else
    {
        c += 1;
        add( 'L' );
    }
}

void sound::letter_m( void )
{
    string::const_iterator& c( m_cursor );

    // 'dumb', 'thumb', 'dumber', 'dummy', but not 'thumbelina"
    if ( ( string( c-1, c+2 ) == "UMB" &&
         ( c+1 == m_last || string( c+2, c+4 ) == "ER" ) ) ||
         *(c+1) == 'M' )
    {
        c += 2;
    }
    else
    {
        c += 1;
    }

    add( 'M' );
}

void sound::letter_n( void )
{
    string::const_iterator& c( m_cursor );

    // Double 'n' sounds like 'n'
    if ( *(c+1) == 'N' )
    {
        c += 2;
    }
    else
    {
        c += 1;
    }
    
    add( 'N' );
}

void sound::letter_n_tilde( void )
{
    string::const_iterator& c( m_cursor );

    c+= 1;
    add( 'N' );
}

void sound::letter_p( void )
{
    string::const_iterator& c( m_cursor );

    // 'phyllis'
    if ( *(c+1) == 'H' )
    {
        add( 'F' );
        c += 2;
        return;
    }

    if ( is_one_of( *(c+1), "PB" ) )
    {
        // 'campbell', 'steppenwolf'
        c += 2;
    }
    else
    {
        // 'peter'
        c += 1;
    }

    add( 'P' );
}

void sound::letter_q( void )
{
    string::const_iterator& c( m_cursor );

    if ( *(c+1) == 'Q' )
    {
        // 'sadiqqi'
        c += 2;
    }
    else
    {
        // 'qadaffi'
        c += 1;
    }

    add( 'K' );
}

void sound::letter_r( void )
{
    string::const_iterator& c( m_cursor );

    if ( c == m_last &&
         !is_slavo_germanic() &&
         string( c-2, c ) == "IE" &&
         !is_one_of( c-4, 2, "ME", "MA", NULL ) )
    {
        // french 'rogier' but not germanic or 'hochmeier'
        add( "", "R" );
    }
    else
    {
        add( 'R' );
    }

    if ( *(c+1) == 'R' )
    {
        c += 2;
    }
    else
    {
        c += 1;
    }
}

void sound::letter_s( void )
{
    string::const_iterator& c( m_cursor );

    if ( is_one_of( c-1, 3, "ISL", "YSL", NULL ) )
    {
        // special cases 'island', 'isle', 'carlisle', 'carlysle'
        c += 1;
    }
    else if ( c == m_first && string( c, c+5 ) == "SUGAR" )
    {
        // special case 'sugar-'
        add( 'X', 'S' );
        c += 1;
    }
    else if ( string( c, c+2 ) == "SH" )
    {
        if ( is_one_of( c+1, 4, "HEIM", "HOEK", "HOLM", "HOLZ", NULL ) )
        {
            // 'rudesheim'
            add( 'S' );
        }
        else
        {
            add( 'X' );
        }
        
        c += 2;
    }
    else if ( is_one_of( c, 3, "SIO", "SIA", NULL ) )
    {
        // italian & armenian
        if ( is_slavo_germanic() )
        {
            add( 'S' );
        }
        else
        {
            add( 'S', 'X' );
        }
         
        c += 3;
    }
    else if ( ( c == m_first && is_one_of( *(c+1), "MNLW" ) ) || *(c+1) == 'Z' )
    {
        // german & anglicisations, e.g. 'smith' match 'schmidt',
        // 'snider' match 'schneider'
        // also, -sz- in slavic language altho in hungarian it is pronounced 's'
        add( 'S', 'X' );
        if ( *(c+1) == 'Z' )
        {
            c += 2;
        }
        else
        {
            c += 1;
        }
    }
    else if ( string( c, c+2 ) == "SC" )
    {
        if ( *(c+2) == 'H' )
        {
            // Schlesinger's rule
            if ( is_one_of( c+3, 2, "OO", "ER", "EN", "UY", "ED", "EM", NULL ) )
            {
                // dutch origin, e.g. 'school', 'schooner'
                if ( is_one_of( c+3, 2, "ER", "EN", NULL ) )
                {
                    // 'schermerhorn', 'schenker'
                    add( "X", "SK" );
                }
                else
                {
                    add( "SK" );
                }
            }
            else
            {
                if ( c == m_first && !is_vowel( *(c+3) ) && *(c+3) != 'W' )
                {
                    add( 'X', 'S' );
                }
                else
                {
                    add( 'X' );
                }
            }
            
            c += 3;

        }
        else if ( is_one_of( *(c+2), "IEY" ) )
        {
            add( 'S' );
            c += 3;
        }
        else
        {
            add( "SK" );
            c += 3;
        }
    }
    else if ( c == m_last && is_one_of( c-2, 2, "AI", "OI", NULL ) )
    {
        // french e.g. 'resnais', 'artois'
        add( "", "S" );
        c += 1;
    }
    else
    {
        add( 'S' );

        if ( is_one_of( *(c+1), "SZ" ) )
        {
            c += 2;
        }
        else
        {
            c += 1;
        }
    }
}

void sound::letter_t( void )
{
    string::const_iterator& c( m_cursor );

    if ( string( c, c+4 ) == "TION" || is_one_of( c, 3, "TIA", "TCH", NULL ) )
    {
        add( 'X' );
        c += 3;
        return;
    }

    if ( string( c, c+2 ) == "TH" || string( c, c+3 ) == "TTH" )
    {
        if ( is_one_of( c+2, 2, "OM", "AM", NULL ) ||
             is_one_of( m_first, 4, "VAN ", "VON ", NULL ) ||
             string( m_first, m_first + 3 ) == "SCH" )
        {
            // special case 'thomas', 'thames' or germanic
            add( 'T' );
        }
        else
        {
            add( '0', 'T' );
        }
        
        c += 2;
        return;
    }

    if ( is_one_of( *(c+1), "TD" ) )
    {
        c += 2;
    }
    else
    {
        c += 1;
    }

    add( 'T' );
    return;
}

void sound::letter_v( void )
{
    string::const_iterator& c( m_cursor );

    if ( *(c+1) == 'V' )
    {
        c += 2;
    }
    else
    {
        c += 1;
    }

    add( 'F' );

    return;
}

void sound::letter_w( void )
{
    string::const_iterator& c( m_cursor );

    // can also be in middle of word
    if ( string( c, c+2 ) == "WR" )
    {
        add( 'R' );
        c += 2;
        return;
    }

    if ( c == m_first  && ( is_vowel( *(c+1) ) || string( c, c+2 ) == "WH" ) )
    {
        // 'wasserman' should match 'vasserman'
        if ( is_vowel( *(c+1 ) ) )
        {
            add( "A", "F" );
        }
        else
        {
            // need Uomo to match Womo
            add( 'A' );
        }
    }

    // 'arnow' should match 'arnoff'
    if ( ( c == m_last && is_vowel( *(c-1) ) ) ||
         is_one_of( c-1, 5, "EWSKI", "EWSKY", "OWSKI", "OWSKY", NULL ) ||
         string( m_first, m_first + 3 ) == "SCH" )
    {
        add( "", "F" );
        c += 1;
        return;
    }

    // polish e.g. 'filipowicz'
    if ( is_one_of( c, 4, "WICZ", "WITZ", NULL ) )
    {
        add( "TS", "FX" );
        c += 4;
        return;
    }

    // else skip it
    c += 1;
}

void sound::letter_x( void )
{
    string::const_iterator& c( m_cursor );

    if ( c == m_first )
    {
        // Initial 'X' is pronounced 'Z'
        add( 'S' );
    }
    else if ( c != m_last || !( is_one_of( c-3, 3, "IAU", "EAU", NULL ) ||
         is_one_of( c-2, 2, "AU", "OU", NULL ) ) )
    {
        // exclude french trailing 'x' e.g. 'breaux'
        add( "KS" );
    }

    if ( is_one_of( *(c+1), "CX" ) )
    {
        c += 2;
    }
    else
    {
        c += 1;
    }
}

void sound::letter_z( void )
{
    string::const_iterator& c( m_cursor );

    if ( *(c+1) == 'H' )
    {
        // chinese pinyin e.g. 'zhao'
        add( 'J' );
        c += 2;
        return;
    }

    if ( is_one_of( c+1, 2, "ZO", "ZI", "ZA", NULL ) ||
              ( is_slavo_germanic() && c > m_first &&  *(c-1) != 'T' ) )
    {
        add( "S", "TS" );
    }
    else
    {
        add( 'S' );
    }

    if ( *(c+1) == 'Z' )
    {
        c += 2;
    }
    else
    {
        c += 1;
    }
}
