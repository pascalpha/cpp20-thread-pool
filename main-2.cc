//
// Created by admin on 2021/3/10.
//

#include<iostream>
#include <chrono>
#include "tpl/executor_service.h"
#include "comm/exchange.h"
#include "comm/instrument.h"
#include "comm/security.h"

int main() {
  using namespace tpl;

  security s1("600030.sh", instrument_t::stock);
  security s2("600030.sse", instrument_t::option);
  std::cout
	  << (s1 == s2) << std::endl
	  << stringify(s1) << std::endl
	//
	  ;
  std::unordered_map<security, exchange_t> map;
  map["600030.sse"] = exchange_t::sse;
  auto it = map.find("600030.sh");

  return 0;
}