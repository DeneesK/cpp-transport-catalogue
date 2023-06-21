#include "json_reader.h"
#include "transport_catalogue.h"

using namespace std;

int main() {
    catalogue::TransportCatalogue db;
    
    request_handler::Requests requests = json_reader::LoadJSON(cin, db);
    
    request_handler::RequestHandler rq_handler(db);
    rq_handler.ProceedRequests(requests, cout);

}