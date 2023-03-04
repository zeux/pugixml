#include "../src/pugixml.hpp"
#include "fuzzer/FuzzedDataProvider.h"

#include <stdint.h>
#include <string.h>
#include <string>

extern "C" int LLVMFuzzerTestOneInput(const uint8_t *Data, size_t Size) {
  FuzzedDataProvider fdp(Data, Size);
  std::string text = fdp.ConsumeRandomLengthString(1024);

  try {
    pugi::xpath_variable_set vars;
    size_t var_count = fdp.ConsumeIntegralInRange<size_t>(0, 50);
    std::vector<std::string> var_name_storage = {};
    for (size_t i = 0; i < var_count; i++) {
      var_name_storage.push_back(fdp.ConsumeRandomLengthString(128));

      pugi::xpath_value_type value_type =
          static_cast<pugi::xpath_value_type>(fdp.ConsumeIntegralInRange(0, 5));
      vars.add(var_name_storage.back().c_str(), value_type);
    }
    pugi::xpath_query q(text.c_str(), &vars);

    std::vector<uint8_t> xml_buffer =
        fdp.ConsumeBytes<uint8_t>(fdp.ConsumeIntegralInRange(0, 1024));
    pugi::xml_document doc;
    doc.load_buffer(xml_buffer.data(), xml_buffer.size(), pugi::parse_full);

    bool boolean = q.evaluate_boolean(doc);
    double num = q.evaluate_number(doc);
    pugi::string_t s = q.evaluate_string(doc);
    pugi::xpath_node n = q.evaluate_node(doc);
    pugi::xpath_node_set set = q.evaluate_node_set(doc);

  } catch (pugi::xpath_exception &) {
  }

  return 0;
}
