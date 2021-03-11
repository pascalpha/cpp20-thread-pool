//
// Created by admin on 2021/3/10.
//

#ifndef TPL_SRC_COMM_EXCHANGE_H
#define TPL_SRC_COMM_EXCHANGE_H

#include <string_view>

#include "enum_beautifier.h"

namespace tpl {

ENUM(exchange_t, sse, szse, unknown);

exchange_t to_exchange(const std::string_view &str) {
  if (str == "sse" || str == "sh") return exchange_t::sse;
  else if (str == "szse" || str == "sz") return exchange_t::szse;
  else return exchange_t::unknown;
}
} // namespace tpl

#endif //TPL_SRC_COMM_EXCHANGE_H
