#include "json_reader.h"
#include "map_renderer.h"

using namespace std;

int main() {
    catalogue::TransportCatalogue db;
    
    json_reader::ReaderResult result = json_reader::LoadJSON(cin, db);
    
    request_handler::RequestHandler rq_handler(db);
    
    map_render::MapRender m_render(result.settings);
    std::ostringstream out;
    m_render.Render(rq_handler, out);
   
    json_reader::RenderJSON(result.requests, rq_handler, out.str(), cout);

}