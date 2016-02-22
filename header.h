#ifndef __HEADER_H__
#define __HEADER_H__

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>
#include <signal.h>
#include <sys/wait.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <sys/param.h>

#define CONFIG_FILE "tcwebngin.conf"
#define DIRECTORY "root/"
#define FILENAME_KARI "root/tcusrs-00.csv"

#define CONFIG__SYMBOL_HOST "host"
#define DEFAULT_PORT 8888

#define NO_ERROR 0
#define ERROR_ARG_UNKNOWN 1001			//不明な引数
#define ERROR_ARG_TOO_MANY 1002         //引数大杉
#define ERROR_ARG_SEVERAL_MODES 1003    //指定モードが多い

#define ERROR_SOCKET 2001               //ソケット作成失敗
#define ERROR_SOCKET_OPTION 2002        //オプション設定失敗
#define ERROR_SOCKET_BIND 2003          //bind失敗
#define ERROR_SOCKET_LISTEN 2004        //listen失敗
#define ERROR_SOCKET_ACCEPT 2005        //接続受付失敗
#define ERROR_GETHOSTBY_FAILED 2006     //相手のホスト、ポート番号取得失敗
#define ERROR_SOCKET_CLOSE 2007         //ソケットクローズ失敗

#define ERROR_CONNECT 2008              //接続エラー
#define ERROR_HOST_UNKNOWN 2009         //ホスト名が解決できない

#define ERROR_FILENOTFOUND 3001         //ファイルが見つからない

#define CLIENT_GET '1'                  //クライアントモード ファイル取得
#define CLIENT_CONFIG '2'               //設定
#define CLIENT_EXIT '9'                 //終了

#define BUF_LEN 256                     //通信バッファサイズ

struct _commandline {
	int server;
	int client;
};

struct _config {
	char host[120];
};

typedef struct _commandline commandline_t;
typedef struct _config config_t;

int server_main();
int server_receive_transmission(int socketid);
int receive_filename(int socketid, char *filename);
int transmission_filedata(int socketid, char *filename, char *filedata);

int client_main();
char get_menu();
void input_a_line(char *inputline);
int client_receive_transmission(char *filename);
int transmission_filename(int socketid, char *filename);
int receive_filedata(int socketid, char *filedata);

int mode_check(char *argv[], commandline_t *commandline,int argc);
int arg_check(char* arg);
void error_message(int err);


#endif