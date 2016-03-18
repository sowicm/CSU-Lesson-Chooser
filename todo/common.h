
//
// todo : check threadEntry
//        模拟环境调试！
//        控制线程总数 [ 1000 is good ]
//        检查新版，自动更新
//

#define WaitWhileNotBegin 1

#define _CRT_SECURE_NO_WARNINGS

#define S_MSG(x)                  MessageBox(NULL, (x), (x), MB_OK)

#define tododir                   "data\\todo\\"
#define ingdir                    "data\\ing\\"
#define donedir                   "data\\done\\"
#define logsdir                   "data\\logs\\"
//#define cachedir                  //"data\\cache\\"

extern char                       cachedir[261];

#define MAX_ThreadForOneRequest   50
#define MAX_HtmlSize              409600
#define RequestThreadDelay        500
#define ThreadCreateDelay         150

#define AllocTries                3
#define AllocDelay                500
