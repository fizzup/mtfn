#include <iostream>
#include <fstream>
#include <string>
#include "mtfn.h"

using namespace std;
using namespace mtfn;

#define error cerr << __FILE__ << ':' << __LINE__ << ' '

static void test_interface( void );

int main ( int argc, char** argv )
{
    if ( argc < 2 )
    {
        error << "USAGE: mtfn <filename>" << endl;
        return 1;
    }

    ifstream istrm( argv[1] );
    string s;
    while ( getline( istrm, s ) )
    {
        sound snd( s );

        if ( snd.has_alternate() )
        {
            cout << s << ',' << snd.primary() << '/' << snd.alternate() << endl;
        }
        else
        {
            cout << s << ',' << snd.primary() << endl;
        }
    }

    test_interface();

    return 0;
}

static void test_interface( void )
{
    sound snd1( "bacher" ), snd2( "packer" );
    bool worked = true;

    if ( snd1 != snd2 )
    {
        error << "bacher and packer don't sound the same" << endl;
        worked = false;
    }

    snd1 = "ivan";
    if ( snd1 == snd2 )
    {
        error << "ivan sounds like bacher" << endl;
        worked = false;
    }

    snd2 = "evan";
    if ( snd1 != snd2 )
    {
        error << "ivan and evan don't sound the same" << endl;
        worked = false;
    }

    if ( !sounds_like( "evan", "ivan" ) )
    {
        error << "evan and ivan don't sound the same" << endl;
        worked = false;
    }

    snd1 = "before";
    snd2 = "after";

    if ( snd1 == snd2 )
    {
        error << "before and after sound the same" << endl;
        worked = false;
    }

    if ( sounds_like( "monday", "tuesday" ) )
    {
        error << "monday and tuesday sound the same" << endl;
        worked = false;
    }

    if ( sounds_like( L"monday", L"tuesday" ) )
    {
        error << "UCS-2 monday and tuesday sound the same" << endl;
        worked = false;
    }

    if ( !sounds_like( L"plated", L"blotted" ) )
    {
        error << "UCS-2 plated sounds different than blotted" << endl;
    }

    if ( sounds_like( L"antidisestablishmentarianism", "supercalifragilesticexpialidocious" ) )
    {
        error << "Long words sound the same" << endl;
        worked = false;
    }

    if ( !worked )
    {
        exit(1);
    }
}
