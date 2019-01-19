/*
 * hot_fix.h
 *
 *  Created on: 1 Dec. 2018
 *      Author: manny
 */

#include <string.h>

#ifndef HOT_FIX_H_
#define HOT_FIX_H_

enum FixType {
	FT_FUNC_BY_ADDR,
	FT_FUNC_BY_SYMBOL,
	FT_VAR_BY_ADDR,
	FT_VAR_BY_SYMBOL,
	FT_END
};

struct TFixItem {
	FixType type_;
	void    *old_addr_;
	void    *new_addr_;
	size_t  new_len_;
};

int init(int signum, const char *fix_path);

#define FIX_BEGIN TFixItem __g_fix_items__[] = {
#define FUNC_BY_ADDR(ADDR, NEW_FUNC) {FT_FUNC_BY_ADDR, ADDR, NEW_FUNC, 0},
#define VAR_BY_ADDR(ADDR, NEW_VALUE, VALUE_LEN) {FT_VAR_BY_ADDR, ADDR, NEW_VALUE, VALUE_LEN},
#define FUNC_BY_SYMBOL(SYMBOL, NEW_FUNC) {FT_FUNC_BY_SYMBOL, (void *)SYMBOL, NEW_FUNC, 0},
#define VAR_BY_SYMBOL(SYMBOL, NEW_VALUE, VALUE_LEN) {FT_VAR_BY_SYMBOL, (void *)SYMBOL, NEW_VALUE, VALUE_LEN},
#define FIX_END {FT_END, (void *)0, (void *)0, 0}};

#endif /* HOT_FIX_H_ */
