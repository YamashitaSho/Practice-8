#include "header.h"

/*
server.cでやること

サーバプログラムとしてポート8888で待機する。
clientからファイル名が送られてくるので、文字列ファイルを読み込んで内容を返信する。
*/

int server_main(){
	int listening_socket;
	int connected_socket;
	struct sockaddr_in server_addr;				//<netinet/in.h>に定義されている
	int len;
	int ret;
	int sock_optival = 1;
	int port = DEFAULT_PORT;

	//ソケットを作成
	listening_socket = socket(AF_INET, SOCK_STREAM, 0);
	if ( listening_socket == -1){
		error_message(ERROR_SOCKET);
	}
	//ソケットオプション設定
	//SO_REUSEADDRを指定することでTIME_WAIT状態のポートにもbindできる
	if ( setsockopt(listening_socket, SOL_SOCKET, SO_REUSEADDR, &sock_optival, sizeof(sock_optival) ) == -1) {
		error_message(ERROR_SOCKET_OPTION);
	}
	
	//アドレスファミリー・ポート番号・IPアドレス設定
	server_addr.sin_family = AF_INET;	                               //TCP/IPを使う
	server_addr.sin_port = htons(port);                               //ポートを指定する(ホストバイトオーダーをネットワークバイトオーダーに変換)
	server_addr.sin_addr.s_addr = htonl(INADDR_ANY);                  //IPアドレスを入れる(バインド用の任意のアドレス)

	//ソケットにアドレスを割り付ける
	if ( bind( listening_socket, (struct sockaddr *)&server_addr, sizeof(server_addr) ) < 0 ){
		error_message(ERROR_SOCKET_BIND);
	}


	//ポートを見張るようにOSに命令する
	ret = listen ( listening_socket, SOMAXCONN) ;
	if ( ret == -1 ){
		error_message(ERROR_SOCKET_LISTEN);
	}
	printf("ポート%dで待機します。\n", port);
	while (1){
		struct hostent *peer_host;
		struct sockaddr_in peer_sin;

		//接続待ち
		len = sizeof(peer_sin);
		connected_socket = accept( listening_socket , (struct sockaddr *)&peer_sin , &len );
		if ( connected_socket == -1){
			error_message(ERROR_SOCKET_ACCEPT);
		}
		//相手側のホスト、ポート番号を表示
		peer_host = gethostbyaddr( (char *)(&peer_sin.sin_addr.s_addr) , sizeof(peer_sin.sin_addr), AF_INET);
		if (peer_host == NULL){
			error_message(ERROR_GETHOSTBY_FAILED);
		}
		
		printf("接続:%s[%s]:%d\n", peer_host->h_name, inet_ntoa(peer_sin.sin_addr), ntohs(peer_sin.sin_port) );
		
		//データのやりとり
		server_receive_transmission( connected_socket );

		printf("接続が切れました。引き続きポート%dで接続待ちを行います。\n",port);
		ret = close(connected_socket);
		if (ret == -1){
			error_message(ERROR_SOCKET_CLOSE);
		}
	}
	ret = close(listening_socket);
	if (ret == -1){
		error_message(ERROR_SOCKET_CLOSE);
	}
	return NO_ERROR;
}

int server_receive_transmission(int socketid){
	char filedata[4096];
	char filename[256];
	
	memset(filedata,'\0',4096);
	memset(filename,'\0',256);
	
	error_message( receive_filename(socketid , filename) );
	error_message( transmission_filedata(socketid , filename, filedata) );
	
	return NO_ERROR;
}

int receive_filename(int socketid, char *filename){
	char buf[256];
	char bufc = '\0';
	
	memset(buf,'\0',256);
	
	while (1){
		read(socketid , &bufc , 1);
		if ( bufc == '\0' ){
			break;
		} else {
			strncat(buf , &bufc , 1);
		}
	}
	printf("受信しました。>>%s\n",buf);
	strcpy(filename,"root/");
	strncat(filename,buf,250);
	return 0;
}

int transmission_filedata(int socketid, char *filename, char *filedata){
	char buf[256];
	int buf_len = 0;
	memset(buf,'\0',256);
	
	FILE *fp;
	fp = fopen(filename,"r");
	if ( fp == NULL ){
		printf("FILE NOT FOUND\n");
		strncpy(filedata , "F00" , 3);
	} else {
		strncpy(filedata , "T00" , 3);
		while( fgets(buf,255,fp) != NULL ){
			strncat(filedata , buf , 255);
		}
	}
	
	while(1){
		if ( filedata[buf_len] == '\0' ){
			break;
		}
		buf_len += write(socketid, (filedata+buf_len) , 255);
	}
	printf("送信しました。\n>>\n%s\n",filedata);
	
	return NO_ERROR;
}
