#pragma once

#include <cstring>
#include <string>

#include "sscma/definations.hpp"
#include "sscma/static_resourse.hpp"
#include "sscma/utility.hpp"

namespace sscma::callback {

using namespace sscma::utility;

void set_info(const std::vector<std::string>& argv) {
    char info[CMD_MAX_LENGTH]{};

    std::strncpy(info, argv[1].c_str(), sizeof(info) - 1);
    auto ret = static_resourse->storage->emplace(el_make_storage_kv("edgelab_info", info)) ? EL_OK : EL_EINVAL;

    std::string ss{
      concat_strings(REPLY_CMD_HEADER,
                     "\"name\": \"",
                     argv[0],
                     "\", \"code\": ",
                     std::to_string(ret),
                     ", \"data\": {\"crc16_maxim\": ",
                     std::to_string(el_crc16_maxim(reinterpret_cast<uint8_t*>(&info[0]), std::strlen(&info[0]))),
                     "}}\n")};

    static_resourse->transport->send_bytes(ss.c_str(), ss.size());
}

void get_info(const std::string& cmd) {
    char info[CMD_MAX_LENGTH]{};
    auto ret = EL_OK;

    if (static_resourse->storage->contains("edgelab_info"))
        ret = static_resourse->storage->get(el_make_storage_kv("edgelab_info", info)) ? EL_OK : EL_EINVAL;

    std::string ss{
      concat_strings(REPLY_CMD_HEADER,
                     "\"name\": \"",
                     cmd,
                     "\", \"code\": ",
                     std::to_string(ret),
                     ", \"data\": {\"crc16_maxim\": ",
                     std::to_string(el_crc16_maxim(reinterpret_cast<uint8_t*>(&info[0]), std::strlen(&info[0]))),
                     ", \"info\": ",
                     quoted(info),
                     "}}\n")};

    static_resourse->transport->send_bytes(ss.c_str(), ss.size());
}

}  // namespace sscma::callback
