# mtfn
An implementation of Double Metaphone in C++ using only the standard library.

The library implements a *class sound* in the *namespace mtfn*. The
*class sound* has a copy constructor, assignment operator, and equality
operator. It also has type conversion constructors for *std::string* (as
ISO-8859-1) and *std::wstring* (as UCS-2).

For convenience, *class sound* also has an equality comparison operator for
*std::string* and *std::wstring*, with the same character sets as the type
conversion constructors.

## Usage

```C++

using namespace std;
using namespace mtfn;

void print_homonyms( const sound& snd, const vector<string>& names )
{
    vector<string>::const_iterator i = names.begin();

    while ( i != names.end() )
    {
        // If *i sounds the same as snd.
        if ( snd == *i )
        {
            cout << i << endl;
        }

        i++;
    }
}

void calling_function( void )
{
    string search_name = get_search_name();
    vector<string> names = get_names();

    print_homonyms( search_name, names );
}

```

If you only ever want to create sounds that are compliant with the refeence
implementation for doubl emetaphone, the interface of *mtfn* can be simplified
to the one below.

The interface to
*class sound* also includes an option on the type conversion constructors
not to limit the
length of the double metaphone representations, which means that some
methods that would otherwise not be required, because they
could rely on the type conversion constructors, are needed so that they
preserve the option to have unlimited length, if that was specified in the
constructor.

```C++
namespace mtfn
{

class sound
{
public:
    // convert an ISO-8859-1 string into its double metaphone sound
    sound( const std::string& str );
    sound( const char* str );

    // convert a UCS-2 wstring into its double metaphone sound
    sound( const std::wstring& wstr );
    sound( const wchar_t* wstr );

    // Copy constructor
    sound( const sound& init );

    // Assignment operator
    const sound& operator =( const sound& init );

    // Equality test returns true if the sounds are pronounced the same way
    bool operator ==( const sound& rhs ) const;

    // Inequality operator is the logical inverse of the equality operator
    bool operator !=( const sound* lhr ) const;

    // Compare the sound of an std::wstring or std::string to this sound
    bool operator ==( const std::string& rhs ) const;
    bool operator ==( const std::wstring& rhs ) const;

    // Primary English pronounciation in America
    const std::string& primary( void ) const;

    // Alternate English pronounciation in America; returns an empty string
    // if sound::has_alternate() == false.
    const std::string& alternate( void ) const;

    // Returns true if there is an alternate pronounciation
    const bool has_alternate( void ) const;
}; // class sound

// Function to allow sound comparison between std::string and std::wstring
template <typename STRA, typename STRB>
bool sounds_like( const STRA& lhs, const STRB& rhs );

// Function to allow sound comparison between std::string and std::wstring
template <typename CHARA, typename CHARB>
bool sounds_like( const CHARA* lhs, const CHARB* rhs );

}; // namespace mtfn

```
