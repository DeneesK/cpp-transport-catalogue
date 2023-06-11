#include "geo.h"
#include "transport_catalogue.h"
#include "input_reader.h"
#include "stat_reader.h"

using namespace std;

int main() {
    catalogue::TransportCatalogue catal;
    input_reader::InputReader(cin, catal);
    output_reader::StatReader(cin, catal);
}