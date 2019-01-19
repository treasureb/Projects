/*
 * hot_fix.cpp
 *
 *  Created on: 9 Dec. 2018
 *      Author: manny
 */


#include <signal.h>
#include <dlfcn.h>
#include <errno.h>
#include <stdint.h>
#include <sys/mman.h>
#include <unistd.h>
#include <stdio.h>

#include "hot_fix.h"

/* 插件的入口名字 */
const char *const FIX_ENTRY_NAME = "__g_fix_items__";

int g_signum;
const char *g_fix_path;

static int fix_func(const void* new_func, void *old_func) {
	//跳转指令
	char prefix[] = {'\x48', '\xb8'};	//MOV new_func %rax
	char postfix[] = {'\xff', '\xe0'};	//JMP %rax

	//开启代码可写权限
	size_t page_size= getpagesize();
	const int inst_len = sizeof(prefix) + sizeof(void *) + sizeof(postfix);
	char *align_point = (char *)old_func - ((uint64_t)(char *)old_func % page_size);
	if (0 != mprotect(align_point, (char *)old_func - align_point + inst_len, PROT_READ | PROT_WRITE | PROT_EXEC)) {
		return -1;
	}

	//将跳转指令写入原函数开头
	memcpy(old_func, prefix, sizeof(prefix));
	memcpy((char *)old_func + sizeof(prefix), &new_func, sizeof(void *));
	memcpy((char *)old_func + sizeof(prefix) + sizeof(void *), postfix, sizeof(postfix));

	//关闭代码可写权限
	if (0 != mprotect(align_point, (char *)old_func - align_point + inst_len, PROT_READ | PROT_EXEC)) {
		return -1;
	}
	return 0;
}

static int fix_var(const void *new_value, void *old_addr, size_t len) {
	size_t page_size= getpagesize();
	char *align_point = (char *)old_addr - ((uint64_t)(char *)old_addr % page_size);
	if (0 != mprotect(align_point, (char *)old_addr - align_point + len, PROT_READ | PROT_WRITE)) {
		return -1;
	}
	memcpy(old_addr, new_value, len);
	return 0;
}

static void do_fix(int signum) {
	if (g_signum != signum) {
		return;
	}
	//使用dlopen加载热补丁库，关于热补丁库的实现，后面会讲
	void *lib = dlopen(g_fix_path, RTLD_NOW);
	if (NULL == lib) {
		return;
	}
	//后续会调用库里面的导出函数获取补丁信息（如总共修改了哪些函数，分别对应原有的哪些函数，新实现的函数等），实现比较繁琐，此处从略，有兴趣可参阅完整代码


    /* 根据句柄和符号获取相应的地址 */
	TFixItem *fix_items = (TFixItem *)dlsym(lib, FIX_ENTRY_NAME);
	if (NULL == fix_items) {
		dlclose(lib);
		return;
	}

    /* 这一步没有看懂 */
	void *me = dlopen(NULL, RTLD_NOW);
	if (NULL == me) {
		dlclose(lib);
		return;
	}
	for (size_t i = 0; fix_items[i].type_ != FT_END; i++) {
		if (fix_items[i].type_ == FT_FUNC_BY_ADDR) {
			fix_func(fix_items[i].new_addr_, fix_items[i].old_addr_);
		} else if (fix_items[i].type_ == FT_FUNC_BY_SYMBOL) {
			void *fix_point = dlsym(me, (const char *)fix_items[i].old_addr_);
			if (NULL == fix_point) {
				continue;
			}
			fix_func(fix_items[i].new_addr_, fix_point);
		} else if (fix_items[i].type_ == FT_VAR_BY_ADDR) {
			fix_var(fix_items[i].new_addr_, fix_items[i].old_addr_, fix_items[i].new_len_);
		}
		else if (fix_items[i].type_ == FT_VAR_BY_SYMBOL) {
			void *fix_point = dlsym(me, (const char *)fix_items[i].old_addr_);
			if (NULL == fix_point) {
				continue;
			}
			fix_var(fix_items[i].new_addr_, fix_point, fix_items[i].new_len_);
		}
	}
	dlclose(me);
}

int init(int signum, const char *fix_path) {
	//注册一个信号处理函数，用于加载热补丁库和执行运行时替换
	if (SIG_ERR == signal(signum, do_fix)) {
		return 1;
	}
    //记接收到的信号
	g_signum = signum;
	//记录热补丁路径名
	g_fix_path = fix_path;
	return 0;
}
