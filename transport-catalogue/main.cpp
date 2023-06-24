#include "json_reader.h"
#include "map_renderer.h"

using namespace std;

int main() {
    catalogue::TransportCatalogue db;
    
    json_reader::ReaderResult result = json_reader::LoadJSON(cin, db);
    
    request_handler::RequestHandler rq_handler(db);
    map_render::MapRender m_render(result.settings);
   
    std::ostringstream os;
    m_render.Render(rq_handler, os);   
    auto strinng_map= os.str();
   
    rq_handler.ProceedRequests(result.requests, strinng_map, cout);

}