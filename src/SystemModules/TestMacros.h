#pragma once

//Shortcut macro; if result is false, run the given failure event.
#define TEST_WITH_FAIL_HANDLE(result, failHandle) if(!result) { failHandle } 

#define TEST_WITH_FAIL_MSG(result, failMsg) TEST_WITH_FAIL_HANDLE(result, printf(failMsg);)
//Shortcut macro; if a function fails, prints the given failure message and returns error -1.
#define P1_RUN(func, failMsg) TEST_WITH_FAIL_HANDLE(func, printf(failMsg); return -1;)

#define TOSTRING_EXPAND(x) #x
#define TOSTRING(x) TOSTRING_EXPAND(x)
#define GFX_TODO static_assert(false, "Feature not implemented:" TOSTRING(__FILE__) ", line " TOSTRING(__LINE__))