#include "hot_fix.h"

extern int g_i;
extern int add(int a, int b);

//新函数实现，修改之前的add函数，函数原型须保持不变
int fadd(int a, int b)
{
	return a + b + g_i;
}

//新的变量值，代替原有g_i
int g_fi = 200;

//开始声明补丁信息
FIX_BEGIN

FUNC_BY_ADDR((void *)&add, (void *)&fadd)		//修改用fadd函数代替add函数
VAR_BY_ADDR(&g_i, (void *)&g_fi, sizeof(int))	//用g_fi代替g_i

//另外一种指定原函数的方式：通过符号指定
//FUNC_BY_SYMBOL("_Z3addii", (void *)&fadd)
//VAR_BY_SYMBOL("g_i", (void *)&g_fi, sizeof(int))

//结束
FIX_END
