//
// Created by admin on 2021/3/10.
//

#ifndef TPL_SRC_COMM_INSTRUMENT_H
#define TPL_SRC_COMM_INSTRUMENT_H

#include "enum_beautifier.h"

namespace tpl {
ENUM(instrument_t, stock, future, option, fund, convertible_bond, etf);
} // namespace tpl

#endif //TPL_SRC_COMM_INSTRUMENT_H
