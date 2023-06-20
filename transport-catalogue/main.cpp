#include "geo.h"
#include "transport_catalogue.h"
#include "input_reader.h"
#include "stat_reader.h"

using namespace std;

int main() {
    catalogue::TransportCatalogue catalogue;
    input_reader::InputReader(cin, catalogue);
    request_reader::StatReader(cin, catalogue, cout);
}