#include "crow_all.h"
#include "hasiru_searcher.hpp"

int main(int argc, char** argv)
{
  hasiru_searcher hasiru;
  hasiru.db_open();

  crow::SimpleApp app;
  crow::mustache::set_base("server");

  CROW_ROUTE(app, "/")
  ([]{
    crow::mustache::context ctx;
    return crow::mustache::load("index.html").render();
  });

  CROW_ROUTE(app, "/api/search")
    .methods("POST"_method)
  ([&](const crow::request& req) {
    auto query = crow::json::load(req.body);
    if (!query) return crow::response(400);

    int from = 0;
    int count = 100;

    int total;
    std::vector<std::string> results;
    results = hasiru.search(query["query"].s(), from, count, &total);

    crow::json::wvalue ret;
    ret["results"] = results;
    ret["total"] = total;
    return crow::response(ret);
  });

  // I don't know how to serve static files...
  CROW_ROUTE(app, "/static/<str>")
  ([](std::string const& path) {
    std::ifstream inf("server/static/" + path);
    if (!inf) return crow::response(404);
    return crow::response {std::string(std::istreambuf_iterator<char>(inf), std::istreambuf_iterator<char>())};
  });

  app.port(18080)
    .multithreaded()
    .run();
}
