/*
 * compile_time.cpp
 * 自動生成的編譯時間字串
 *
 * Created: 2017/8/23 下午 02:51:22
 *  Author: user
 */ 


//
// 說明：如何使用compile_time.cpp
// 1.將這檔案加入專案
// 2.編輯project的Properties，切換到Build Events分頁，為所有的Configuration加上

//       del compile_time.o

// (註：似乎對 All Configurations 直接加入命令沒辦法作用)
//
// 3.在需要編譯時間字串的地方加上宣告

//       const char* system_startup_message();

// 4.在程式中呼叫system_startup_message()取得回傳值後即可使用
// 注意：編譯時間字串是放在Program Space，不可直接讀取
//



#include "Arduino.h"

PROGMEM const char __system_startup_message[] = "  (build: " __DATE__ " - " __TIME__;

const char* system_startup_message()
{
	return __system_startup_message;
}
