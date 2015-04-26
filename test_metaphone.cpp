#include <iostream>
#include <fstream>
#include <string>
#include "mtfn.h"

using namespace std;
using namespace mtfn;

int main ( int argc, char** argv )
{
    if ( argc < 2 )
    {
        cerr << "USAGE: mtfn <filename>" << endl;
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

    return 0;
}
