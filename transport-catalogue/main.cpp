#include "json_reader.h"
#include "map_renderer.h"

using namespace std;

int main() {
    catalogue::TransportCatalogue db;
    
    json_reader::ReaderResult result = json_reader::LoadJSON(cin, db);
    
    request_handler::RequestHandler rq_handler(db);
    // rq_handler.ProceedRequests(result.requests, cout);
    map_render::MapRender m_render(result.settings);
    m_render.Render(rq_handler, cout);
}