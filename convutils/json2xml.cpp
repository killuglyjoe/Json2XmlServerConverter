#include <iostream>
#include <sstream>
#include <fstream>

#include "json2xml.hpp"

using namespace std;

namespace Json2Xml
{
    auto convert( const istream &input ) -> void
    {
        ostringstream oss;
        oss << input.rdbuf();

        const auto json_str = json2xml( oss.str().data() );
        cout << json_str << endl;
    }
}

//auto main( const int argc, const char *const argv[] ) -> int
//{
//    switch ( argc ) {
//        case 1: convert( cin );
//                break;
//        case 2: convert( ifstream( argv[1] ));
//                break;
//        default: exit( EXIT_FAILURE );
//    }
//}
