#include "convutils/json2xml.hpp"

#include <iostream>
#include <sstream>
#include <fstream>

using namespace std;

auto convert( const istream &input ) -> void
{
    ostringstream oss;
    oss << input.rdbuf();

    const auto json_str = Json2Xml::json2xml( oss.str().data() );
    cout << json_str << endl;
}
// test convert
int main(int argc, char *argv[])
{
    convert( std::ifstream( "./foo/bar.json" ));

    return 0;
}
