#include "header.h"

/*
client.cでやること
メニューの表示
1.ファイル取得
サーバプログラムに接続し、ファイル名を送信する。
サーバプログラムから返信されたファイルの内容を受け取る。
2.設定
設定ファイルを読み込む。なければ作成する。
9.
*/

int client_main(){
	int mode;
	char inputline[256];
	
	memset(inputline,'\0',256);
		
	while(1){
		mode = get_menu();
		
		if ( mode == CLIENT_GET ){				//ファイル取得
			input_a_line(inputline);
			if ( inputline[0] == '\n' ){
				printf("読み込むファイル名を入力して下さい。\n");
			} else {
				error_message(client_receive_transmission(inputline));
			}
		} else if ( mode == CLIENT_CONFIG ){
		} else if ( mode == CLIENT_EXIT ){				//終了
			exit(0);
		}
	}
	return NO_ERROR;
}

char get_menu(){
	char input[50];
	char mode;
	
	printf("1.ファイル取得\n2.設定(未実装)\n9.終了\n\nメニュー番号を入力してください:");
	fflush(stdin);
	fgets(input,50,stdin);
	mode = input[0];
	
	return mode;
}

void input_a_line(char *inputline){
	char input[256];
	char *newline;									//改行検出用
	
	printf("\n読み込むファイル名を入力してください。>>");
	fflush(stdin);
	fgets(input , 255 , stdin);
	
	if (input[0] != '\n') {
		newline = memchr(input , '\n', 255);		//fileの終端にある改行コードを検出する
		*newline = '\0';							//'\0'に置き換える
	}
	strncpy(inputline , input , 255);
}

int client_receive_transmission(char *filename){
	int connecting_socket;
	struct sockaddr_in client_addr;
	struct in_addr servhost;
	int port = DEFAULT_PORT;
	int ret;
	char hostname[16] = {"127.0.0.1"};
	char receive_data[4096];
	
	memset(receive_data,'\0',4096);
	
	//ソケットを作成
	connecting_socket = socket(AF_INET, SOCK_STREAM, 0);
	if ( connecting_socket == -1){
		error_message(ERROR_SOCKET);
	}
	
	printf("%s:に接続します。\n", hostname );

	if ( inet_aton(hostname , &servhost) == 0){
		return ERROR_HOST_UNKNOWN;
	}
	//アドレスファミリー・ポート番号・IPアドレス設定               設定ファイルを適用する部分
	memset(&client_addr, '\0' ,sizeof(client_addr));
	client_addr.sin_family = AF_INET;
	client_addr.sin_port = htons(port);
	client_addr.sin_addr = servhost;

	ret = connect( connecting_socket, (struct sockaddr *)&client_addr, sizeof(client_addr) );
	if ( ret < 0 ){
		close( connecting_socket );
		return ERROR_CONNECT;
	}
	
	printf("サーバーに接続しました。\n");
	transmission_filename( connecting_socket, filename );
	receive_filedata( connecting_socket, receive_data);

	close( connecting_socket );
	
	return NO_ERROR;

}
//ファイル名送信
int transmission_filename(int socketid, char *filename){
	int buf_len = 0;
	
	while(1){
		if ( filename[buf_len] == '\0' ){
			break;
		}
		buf_len += write( socketid , (filename+buf_len) , 255 );
	}
	printf("ファイル名:%sを送信しました。\n",filename);
	return NO_ERROR;
}
//ファイル内容受信
int receive_filedata(int socketid, char *receive_data){
	char bufc = '\0';
	char header[4];
	
	memset(header,'\0',4);
	for(int i=0;i<3;i++){
		read ( socketid , &bufc , 1);
		strncat(header , &bufc , 1);
	}
	if ( header[0] == 'T'){
		while (1){
			read ( socketid , &bufc , 1);
			if ( bufc == '\0' ){
				break;
			} else {
				strncat(receive_data , &bufc , 1);
			}
		}
		printf("%s\n%luバイト受信しました。\n",receive_data,strlen(receive_data));
	} else if ( header[0] == 'F' ){
		printf("ファイルが見つかりませんでした。\n");
	}
	
	return NO_ERROR;
}
