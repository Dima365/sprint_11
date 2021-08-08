#include "tests_transport.h"
#include "log_duration.h"

using namespace std;

namespace transport {
namespace tests {

void Test() {
  Catalogue tc;
  renderer::MapRenderer renderer;
  std::ifstream in("inout/test_input.json");
  assert(in.is_open());
  RequestHandler handler(tc, renderer);
  JsonReader reader(handler, in);
  Bus* bus_ptr = tc.GetBusData("114"s);
  assert(abs(bus_ptr->curvature - 1.23199) < 1e-4);
  assert(bus_ptr->routeLength == 1700);
  assert(bus_ptr->stopsOnRoute == 3);
  assert(bus_ptr->uniqueStops = 2);

  Stop* stop_ptr = tc.GetStopData("Ривьерский мост"s);
  assert(stop_ptr->buses.find("114"s) != stop_ptr->buses.end());
}

void Test_1() {
  Catalogue tc;
  renderer::MapRenderer renderer;
  std::ifstream in("inout/test_1_input.json");
  assert(in.is_open());
  RequestHandler handler(tc, renderer);
  JsonReader reader(handler, in);
  in.seekg(0, ios_base::beg);
  std::ofstream out("inout/test_1_output.json");
  assert(out.is_open());
  reader.Print(out);
  in.close();
  out.close();
  std::ifstream check("inout/test_1_output.json");
  assert(check.is_open());
  std::ifstream expect("inout/test_1_expect.json");
  assert(expect.is_open());
  json::Document doc_check = json::Load(check);
  json::Document doc_expect = json::Load(expect);
  check.close();
  expect.close();
  assert(doc_check == doc_expect);
}

void Test_2() {
  Catalogue tc;
  renderer::MapRenderer renderer;
  RequestHandler handler(tc, renderer);
  std::ifstream in(
      "/home/dima/eclipse_workspace/sprint_10/inout/test_2_input.json");
  assert(in.is_open());
  JsonReader reader(handler, in);
  in.seekg(0, ios_base::beg);
  std::ofstream out(
      "/home/dima/eclipse_workspace/sprint_10/inout/test_2_output.json");
  assert(out.is_open());
  reader.Print(out);
  in.close();
  out.close();
  std::ifstream check(
      "/home/dima/eclipse_workspace/sprint_10/inout/test_2_output.json");
  assert(check.is_open());
  std::ifstream expect(
      "/home/dima/eclipse_workspace/sprint_10/inout/test_2_expect.json");
  assert(expect.is_open());
  json::Document doc_check = json::Load(check);
  json::Document doc_expect = json::Load(expect);
  check.close();
  expect.close();
  assert(doc_check == doc_expect);
}

void Test_3() {
  Catalogue tc;
  renderer::MapRenderer renderer;
  RequestHandler handler(tc, renderer);
  std::ifstream in("inout/test_3_input.json");
  JsonReader reader(handler, in);
  in.seekg(0, ios_base::beg);
  std::ofstream out("inout/test_3_output.json");
  reader.Print(out);
  in.close();
  out.close();
  std::ifstream check("inout/test_3_output.json");
  std::ifstream expect("inout/test_3_expect.json");
  json::Document doc_check = json::Load(check);
  json::Document doc_expect = json::Load(expect);
  check.close();
  expect.close();
  assert(doc_check == doc_expect);
}

void Test_4() {
  Catalogue tc;
  renderer::MapRenderer renderer;
  RequestHandler handler(tc, renderer);
  std::ifstream in("inout/test_4_input.json");
  JsonReader reader(handler, in);
  in.seekg(0, ios_base::beg);
  std::ofstream out("inout/test_4_output.json");
  reader.Print(out);
  in.close();
  out.close();
  std::ifstream check("inout/test_4_output.json");
  std::ifstream expect("inout/test_4_expect.json");
  json::Document doc_check = json::Load(check);
  json::Document doc_expect = json::Load(expect);
  check.close();
  expect.close();
  assert(doc_check == doc_expect);
}

void Test_5() {
  Catalogue tc;
  renderer::MapRenderer renderer;
  RequestHandler handler(tc, renderer);
  std::ifstream in("inout/test_5_input.json");
  JsonReader reader(handler, in);
  in.seekg(0, ios_base::beg);
  std::ofstream out("inout/test_5_output.json");
  reader.Print(out);
  in.close();
  out.close();
  std::ifstream check("inout/test_5_output.json");
  std::ifstream expect("inout/test_5_expect.json");
  json::Document doc_check = json::Load(check);
  json::Document doc_expect = json::Load(expect);
  check.close();
  expect.close();
  assert(doc_check == doc_expect);
}

void Test_6() {
  Catalogue tc;
  renderer::MapRenderer renderer;
  RequestHandler handler(tc, renderer);
  std::ifstream in("inout/test_6_input.json");
  JsonReader reader(handler, in);
  in.seekg(0, ios_base::beg);
  std::ofstream out("inout/test_6_output.json");
  reader.Print(out);
  in.close();
  out.close();
  std::ifstream check("inout/test_6_output.json");
  std::ifstream expect("inout/test_6_expect.json");
  json::Document doc_check = json::Load(check);
  json::Document doc_expect = json::Load(expect);
  check.close();
  expect.close();
  assert(doc_check == doc_expect);
}

void Test_7() {
  Catalogue tc;
  renderer::MapRenderer renderer;
  RequestHandler handler(tc, renderer);
  std::ifstream in("inout/test_7_input.json");
  JsonReader reader(handler, in);
  in.seekg(0, ios_base::beg);
  std::ofstream out("inout/test_7_output.json");
  reader.Print(out);
  in.close();
  out.close();
  std::ifstream check("inout/test_7_output.json");
  std::ifstream expect("inout/test_7_expect.json");
  json::Document doc_check = json::Load(check);
  json::Document doc_expect = json::Load(expect);
  check.close();
  expect.close();
  assert(doc_check == doc_expect);
}

void Test_8() {
  Catalogue tc;
  renderer::MapRenderer renderer;
  RequestHandler handler(tc, renderer);
  std::ifstream in("inout/test_8_input.json");
  JsonReader reader(handler, in);
  in.seekg(0, ios_base::beg);
  std::ofstream out("inout/test_8_output.json");
  reader.Print(out);
  in.close();
  out.close();
  std::ifstream check("inout/test_8_output.json");
  std::ifstream expect("inout/test_8_expect.json");
  json::Document doc_check = json::Load(check);
  json::Document doc_expect = json::Load(expect);
  check.close();
  expect.close();
  assert(doc_check == doc_expect);
}

void Test_9() {
  Catalogue tc;
  renderer::MapRenderer renderer;
  RequestHandler handler(tc, renderer);
  std::ifstream in("inout/test_9_input.json");
  JsonReader reader(handler, in);
  in.seekg(0, ios_base::beg);
  std::ofstream out("inout/test_9_output.json");
  reader.Print(out);
  in.close();
  out.close();
  std::ifstream check("inout/test_9_output.json");
  std::ifstream expect("inout/test_9_expect.json");
  json::Document doc_check = json::Load(check);
  json::Document doc_expect = json::Load(expect);
  check.close();
  expect.close();
  assert(doc_check == doc_expect);
}

void Test_10() {
  std::ifstream in("inout/test_10_input.json");
  std::ofstream out("inout/test_10_output.json");
  LOG_DURATION("Good"s);
  Catalogue tc;
  renderer::MapRenderer renderer;
  RequestHandler handler(tc, renderer);
  JsonReader reader(handler, in);
  reader.Print(out);
/*  std::ifstream check("inout/test_10_output.json");
  std::ifstream expect("inout/test_10_expect.json");
  json::Document doc_check = json::Load(check);
  json::Document doc_expect = json::Load(expect);
  check.close();
  expect.close();
  assert(doc_check == doc_expect);*/
}

void Test_11() {
  std::ifstream in("inout/test_11_input.json");
  std::ofstream out("inout/test_11_output.json");
  LOG_DURATION("Good"s);
  Catalogue tc;
  renderer::MapRenderer renderer;
  RequestHandler handler(tc, renderer);
  JsonReader reader(handler, in);
  reader.Print(out);
/*  std::ifstream check("inout/test_10_output.json");
  std::ifstream expect("inout/test_10_expect.json");
  json::Document doc_check = json::Load(check);
  json::Document doc_expect = json::Load(expect);
  check.close();
  expect.close();
  assert(doc_check == doc_expect);*/
}

}  // namespace tests
}  // namespace transport
