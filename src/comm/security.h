#pragma clang diagnostic push
#pragma ide diagnostic ignored "google-explicit-constructor"
//
// Created by admin on 2021/3/10.
//

#ifndef TPL_SRC_COMM_SECURITY_H
#define TPL_SRC_COMM_SECURITY_H

#include <string>
#include <unordered_map>
#include <functional>

#include "exchange.h"
#include "instrument.h"

namespace tpl {
class security {
  friend std::string stringify(security const &);

  std::string symbol;

  exchange_t exchange = exchange_t::unknown;

  instrument_t instrument = instrument_t::stock;

public:
  security(std::string sym, exchange_t ex, instrument_t inst = instrument_t::stock)
	  : symbol(std::move(sym)), exchange(ex), instrument(inst) {}

  security(const std::string_view &sym, instrument_t inst = instrument_t::stock)
	  : instrument(inst) { construct_from_suffixed_symbol(sym); }

  security(const char *sym) : security(std::string_view(sym)) {}

  security(security const &) = default;

  security(security &&) = default;

  security &operator=(security const &) = default;

  security &operator=(security &&) = default;

  [[nodiscard]] std::string_view get_symbol() const { return symbol; }

  [[nodiscard]] exchange_t get_exchange() const { return exchange; }

  [[nodiscard]] instrument_t get_instrument() const { return instrument; }

  [[nodiscard]] std::string stringify() const {
	return (symbol + ".").append(::tpl::stringify(exchange));
  }

private:
  inline void construct_from_suffixed_symbol(std::string_view const &sym) {
	auto sep = sym.find('.');
	symbol = sym.substr(0, sep);
	exchange = to_exchange(sym.substr(sep + 1));
  }
};

bool operator==(security const &lhs, security const &rhs) {
  return lhs.get_symbol() == rhs.get_symbol() && lhs.get_exchange() == rhs.get_exchange();
}

std::string stringify(security const &sec) {
  return sec.stringify();
}
} // namespace tpl


namespace std {
template<> struct hash<tpl::security> {
  std::size_t operator()(tpl::security const &s) const noexcept {
	return std::hash<std::string>{}(s.stringify());
  }
};
} // namespace std

#endif //TPL_SRC_COMM_SECURITY_H

#pragma clang diagnostic pop