// Ohmok 오목(인공 지능 포함)
// 제작 환경 : win 98, visual studio c++6.0 API 사용 
// 제작자 : 황주연 
// 제작 일시 : 2002년 8월 27일 ~
// update : 2004년 1월 6일

#include <windows.h>
#include <stdlib.h>
#include <stdio.h>
#include <wingdi.h>
#include "resource.h"
#include "base.h"
#include "OhmokAI.h"
#define random(n) (rand()%n)

LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);

const int direction_x[8]={-1,0,+1,+1,+1,0,-1,-1};
const int direction_y[8]={-1,-1,-1,0,+1,+1,+1,0};
void drawscreen();//바둑판 그려주기
void drawstone();//돌 그려주기
int iscomplete(int stone_color);//승패 확인 
int search(int increase_x, int increase_y, int stone_color);//연속된 5개 찾기
															//증감값에 따라 검색하는 줄이 달라진다.
															//리턴값 : 승리일경우 VICTORY 리턴 0 is nothing
int samsam(int x, int y, int stone_color);
void end(int stone_color);//승부가 결정됐을경우 다시 할것인지를 묻는다.
void samsam_end(int stone_color);//삼삼으로 게임이 끝났을 경우

void putstone(int x, int y, int stone_color);//돌을 놓는다.
void AI();//컴퓨터 인공지능
BOOL checkpattern(int x, int y, int increase_x, int increase_y , const char *pattern, int stone_color, int checkback);//패턴 검색
BOOL Excheckpattern(int x, int y, int increase_x, int increase_y , const char*pattern, int stone_color,int checkback);//7방향 검색
//발견시 1 미발견 0    stone_color는 지정색이 1이 되고 0은 빈공간 2는 다른돌 3은 어느것이든지를 의미

COhmokAI	CEnemy;
int omok[19][19]; // 오목 배열
int value_of_estimate[19][19]; // AI에서 사용하는 평가표 
char tempstr[128]; // 임시 string 코딩 마친후 지워줄것
int is_samsam=0;//삼삼에 사용 1이면 첫번째 세개연속 발견 2면 삼삼

int turn_time;
int x_pos,y_pos;// 배열에 사용되는 위치
BOOL game_start=FALSE;
int user, comp;//comp는 필요없을때 지워주자~

HBITMAP hBackground, hWhite, hBlack;
HDC hDC, hMemBackgroundDC, hMemWhiteDC, hMemBlackDC;
HINSTANCE g_hInst;
HWND hWnd;

int APIENTRY WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
	MSG Message;
	WNDCLASS WndClass;
	g_hInst = hInstance;

	WndClass.cbClsExtra = 0;
	WndClass.cbWndExtra = 0;
	WndClass.hbrBackground=(HBRUSH)GetStockObject(WHITE_BRUSH);
	WndClass.hCursor = LoadCursor(NULL, IDC_ARROW);
	WndClass.hIcon = LoadIcon(g_hInst, MAKEINTRESOURCE(IDI_ICON1));
	WndClass.hInstance= hInstance;
	WndClass.lpfnWndProc=(WNDPROC)WndProc;
	WndClass.lpszClassName= "OHmok";
	WndClass.lpszMenuName = MAKEINTRESOURCE(IDR_MENU1);
	WndClass.style = CS_HREDRAW | CS_VREDRAW;
	RegisterClass(&WndClass);
	
	hWnd=CreateWindow("OHmok","Oh! mok!!", 
		WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU,
		50,50,373,411, NULL, NULL, g_hInst, NULL);
	ShowWindow(hWnd, nCmdShow);
	UpdateWindow(hWnd);

	while(GetMessage(&Message,0,0,0)) {
		TranslateMessage(&Message);
		DispatchMessage(&Message);
	}
	return Message.wParam;
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT iMessage, WPARAM wParam, LPARAM lParam)
{
	//int x, y;
	switch(iMessage){
	case WM_CREATE:
		hDC=GetDC(hWnd);
		hDC=CreateCompatibleDC(hDC);
		hMemBackgroundDC=CreateCompatibleDC(hDC);
		hMemBlackDC=CreateCompatibleDC(hDC);
		hMemWhiteDC=CreateCompatibleDC(hDC);
		ReleaseDC(hWnd,hDC);
		hBackground=LoadBitmap(g_hInst,MAKEINTRESOURCE(IDB_BACKGROUND));
		SelectObject(hMemBackgroundDC, hBackground);
		hBlack=LoadBitmap(g_hInst,MAKEINTRESOURCE(IDB_BLACK));
		SelectObject(hMemBlackDC, hBlack);
		hWhite=LoadBitmap(g_hInst,MAKEINTRESOURCE(IDB_WHITE));
		SelectObject(hMemWhiteDC, hWhite);
		//game_start = FALSE;
		return 0;

	case WM_PAINT:
		PAINTSTRUCT ps;
		hDC=BeginPaint(hWnd,&ps);
		EndPaint(hWnd,&ps);
		drawscreen();
		if(game_start) //시작 메뉴 만들어 준후 바꿔줄것 if(game_start)로..
			drawstone();
		return 0;

	/*case WM_MOUSEMOVE:	
		hDC=GetDC(hWnd);
		sprintf(tempstr, "%d %d %d %d",(LOWORD(lParam)-3)/19+1,(HIWORD(lParam)-3)/19+1 ,LOWORD(lParam),HIWORD(lParam) );
		TextOut(hDC,0,350,tempstr,20);
		ReleaseDC(hWnd, hDC);
		return 0;*/

	case WM_LBUTTONDOWN:
		if(!game_start) return 0;//게임이 시작 되지 않았으면 아무것도 하지 않는다
		x_pos=(LOWORD(lParam)-3)/19;
		y_pos=(HIWORD(lParam)-3)/19;
		if(omok[x_pos][y_pos] != EMPTY_SPACE || x_pos > 18 || x_pos < 0 || y_pos >18 || y_pos < 0)
			return 0;//영역을 벗어나거나 빈공간이 아니면 아무것도 하지 않는다.
		if(samsam(x_pos, y_pos, user) == SAMSAM)
		{
			MessageBox(hWnd, "삼삼이라 놓을수 없습니다.", "삼삼!", MB_OK);
			return 0;
		}
		putstone(x_pos, y_pos, user);
		if(iscomplete(user) == VICTORY) {
			end(user);//끝났을때 처리...
			return 0;
		}
		if(iscomplete(user) ==SAMSAM)//삼삼으로 끝날경우
		{
			samsam_end(user);
			return 0;
		}
		
		AI();
		if(iscomplete(comp) == VICTORY) {
			end(comp);
			return 0;
		}
		if(iscomplete(user) ==SAMSAM)
		{
			samsam_end(user);
			return 0;
		}
		return 0;

	case WM_COMMAND:
		switch(wParam){
		case ID_START_BLACK://AI 완성후 수정 요망
			//cur_player = BLACK_STONE;
			game_start = TRUE;
			user = BLACK_STONE;
			comp = WHITE_STONE;
			turn_time = 0;
			EnableMenuItem(GetMenu(hWnd),ID_START_BLACK,MF_BYCOMMAND | MF_DISABLED | MF_GRAYED);
			EnableMenuItem(GetMenu(hWnd),ID_START_WHITE,MF_BYCOMMAND | MF_DISABLED | MF_GRAYED);
			break;
		case ID_START_WHITE:
			//cur_player = BLACK_STONE;
			game_start = TRUE;
			user = WHITE_STONE;
			comp = BLACK_STONE;
			turn_time = 0; 
			EnableMenuItem(GetMenu(hWnd),ID_START_BLACK,MF_BYCOMMAND | MF_DISABLED | MF_GRAYED);
			EnableMenuItem(GetMenu(hWnd),ID_START_WHITE,MF_BYCOMMAND | MF_DISABLED | MF_GRAYED);
			AI();
			break;
		case ID_END:
			DestroyWindow(hWnd);
			break;
		}
		return 0;

	case WM_DESTROY:
		DeleteDC(hMemBackgroundDC);
		DeleteDC(hMemBlackDC);
		DeleteDC(hMemWhiteDC);
		DeleteDC(hDC);
		DeleteObject(hBackground);
		DeleteObject(hBlack);
		DeleteObject(hWhite);
		PostQuitMessage(0);
		return 0;
	}
	return (DefWindowProc(hWnd, iMessage, wParam, lParam));
}

void drawscreen()
{
	hDC=GetDC(hWnd);
	BitBlt(hDC,0,0,367,367,hMemBackgroundDC,0,0,SRCCOPY);
	ReleaseDC(hWnd, hDC);
}

void drawstone()
{
	int i,j;
	hDC=GetDC(hWnd);
	for(i=0; i<19; i++)
		for(j=0; j<19; j++){
			switch(omok[i][j])
			{
			case BLACK_STONE:
				TransparentBlt(hDC,i*19+3,j*19+1,18,18,hMemBlackDC,
					0,0,18,18,RGB(255,255,255));
				break;
			case WHITE_STONE:
				TransparentBlt(hDC,i*19+3,j*19+1,18,18,hMemWhiteDC,
					0,0,18,18,RGB(255,255,255));
				break;
			case EMPTY_SPACE:
				break;
			}
		}

			
	ReleaseDC(hWnd,hDC);
}

int iscomplete(int stone_color)
{
	if(search(-1,-1, stone_color) == VICTORY) return VICTORY;//왼쪽-위 대각선
	if(search(-1,0, stone_color) == VICTORY) return VICTORY;//가로 대각선
	if(search(-1,+1, stone_color) == VICTORY) return VICTORY;//왼쪽-아래 대각선
	if(search(0,-1, stone_color) == VICTORY) return VICTORY;//세로 대각선
	return 0;
}

int search(int increase_x, int increase_y, int stone_color)
{
	int i,stone=1;
	for(i=1; i < 5; i++){
		if( x_pos + (i * increase_x) >= 0 && x_pos + (i * increase_x) <= 18 && 
			y_pos + (i * increase_y) >= 0 && y_pos + (i * increase_y) <= 18 ) {
			if(omok[x_pos + (i * increase_x)][y_pos + (i * increase_y)] == stone_color ) {
				stone++;
				if(stone>=5) return VICTORY;
			} 
			else break;
		}
		else break;
	}

	for(i=1; i < 5; i++){
		if(x_pos - (i * increase_x) >= 0 && x_pos - (i * increase_x) <= 18 && 
			y_pos - (i * increase_y) >= 0 && y_pos - (i * increase_y) <= 18) {
			if(omok[x_pos - (i * increase_x)][y_pos - (i * increase_y)] == stone_color ) {
				stone++;
				if(stone >= 5) return VICTORY;
			} else return 0;
		}
		else return 0;
	}
	return 0;
}
		
int check_samsam(int x, int y, int stone_color)
{
	omok[x][y] = stone_color;
	if(samsam(x, y, stone_color)) {
		omok[x][y] = EMPTY_SPACE;
		return SAMSAM;
	}
	omok[x][y] = EMPTY_SPACE;
	return 0;
}

int samsam(int x, int y,int stone_color)
{
	is_samsam = 0;
	if(checkpattern(x, y, -1,-1, "00110", stone_color, -1) 
		&& !checkpattern(x, y, -1,-1, "001101", stone_color, -1) 
		&& !checkpattern(x, y, -1,-1, "10011", stone_color, -2) )
		is_samsam++;
	if(checkpattern(x, y, 0,-1, "00110", stone_color, -1)
		&& !checkpattern(x, y,  0,-1, "001101", stone_color, -1) 
		&& !checkpattern(x, y,  0,-1, "10011", stone_color, -2) )
		is_samsam++;
	if(checkpattern(x, y, +1,-1, "00110", stone_color, -1)
		&& !checkpattern(x, y, +1,-1, "001101", stone_color, -1) 
		&& !checkpattern(x, y, +1,-1, "10011", stone_color, -2) )
		is_samsam++;
	if(checkpattern(x, y, +1,0, "00110", stone_color, -1)
		&& !checkpattern(x, y, +1, 0, "001101", stone_color, -1) 
		&& !checkpattern(x, y, +1, 0, "10011", stone_color, -2) )
		is_samsam++;
	if(checkpattern(x, y, +1,+1, "00110", stone_color, -1)
		&& !checkpattern(x, y, +1,+1, "001101", stone_color, -1) 
		&& !checkpattern(x, y, +1,+1, "10011", stone_color, -2) )
		is_samsam++;
	if(checkpattern(x, y, 0,+1, "00110", stone_color, -1)
		&& !checkpattern(x, y,  0,+1, "001101", stone_color, -1) 
		&& !checkpattern(x, y,  0,+1, "10011", stone_color, -2) )
		is_samsam++;
	if(checkpattern(x, y, -1,+1, "00110", stone_color, -1)
		&& !checkpattern(x, y, -1,+1, "001101", stone_color, -1) 
		&& !checkpattern(x, y, -1,+1, "10011", stone_color, -2) )
		is_samsam++;
	if(checkpattern(x, y, -1,0, "00110", stone_color, -1)
		&& !checkpattern(x, y, -1, 0, "001101", stone_color, -1) 
		&& !checkpattern(x, y, -1, 0, "10011", stone_color, -2) )
		is_samsam++;
    ////////////////////////////////////////////////////////////////
	if(checkpattern(x, y, -1,-1, "01010", stone_color, -2))
		if(!checkpattern(x, y, -1,-1, "010101", stone_color, -2) && 
			!checkpattern(x, y, -1,-1, "101010", stone_color, -3)) 
			is_samsam++;
	if(checkpattern(x, y, 0,-1, "01010", stone_color, -2))
		if(!checkpattern(x, y, 0,-1, "010101", stone_color, -2) && 
			!checkpattern(x, y, 0,-1, "101010", stone_color, -3)) 
		is_samsam++;
	if(checkpattern(x, y, +1,-1, "01010", stone_color, -2))
		if(!checkpattern(x, y, +1,-1, "010101", stone_color, -2) && 
			!checkpattern(x, y, +1,-1, "101010", stone_color, -3)) 
		is_samsam++;
	if(checkpattern(x, y, +1,0, "01010", stone_color, -2))
		if(!checkpattern(x, y, +1, 0, "010101", stone_color, -2) && 
			!checkpattern(x, y, +1, 0, "101010", stone_color, -3)) 
		is_samsam++;
	if(is_samsam >= 2) return SAMSAM;
	return 0;
}

void end(int stone_color)
{
	if(stone_color == WHITE_STONE)
		sprintf(tempstr, "하얀돌의 승리입니다.");
	if(stone_color == BLACK_STONE)
		sprintf(tempstr, "검은돌의 승리입니다.");

	if(MessageBox(hWnd, "한게임더???", tempstr, MB_YESNO) == IDYES){
		memset(omok,0,sizeof(omok));
		InvalidateRect(hWnd,NULL,TRUE);
		UpdateWindow(hWnd);
		game_start = FALSE;
		EnableMenuItem(GetMenu(hWnd),ID_START_BLACK,MF_BYCOMMAND | MF_ENABLED );
		EnableMenuItem(GetMenu(hWnd),ID_START_WHITE,MF_BYCOMMAND | MF_ENABLED );
		return;
	}
	DestroyWindow(hWnd);				
}

void samsam_end(int stone_color)
{
	if(stone_color == WHITE_STONE)
		sprintf(tempstr, "하얀돌의 삼삼패입니다.");
	if(stone_color == BLACK_STONE)
		sprintf(tempstr, "검은돌의 삼삼패입니다.");

	if(MessageBox(hWnd, "한게임더???", tempstr, MB_YESNO) == IDYES){
		memset(omok,0,sizeof(omok));
		InvalidateRect(hWnd,NULL,TRUE);
		UpdateWindow(hWnd);
		game_start = FALSE;
		EnableMenuItem(GetMenu(hWnd),ID_START_BLACK,MF_BYCOMMAND | MF_ENABLED );
		EnableMenuItem(GetMenu(hWnd),ID_START_WHITE,MF_BYCOMMAND | MF_ENABLED );
		return;
	}
	DestroyWindow(hWnd);				
}

void putstone(int x,int y, int stone_color)
{
	HDC c;
	hDC=GetDC(hWnd);
	omok[x][y] = stone_color;
	x_pos= x;
	y_pos= y;

	if(stone_color == BLACK_STONE)
		c=hMemBlackDC;
	else 
		c=hMemWhiteDC;
	
	TransparentBlt(hDC,x * 19 + 3,y * 19 +1 ,18,18,c,
		0,0,18,18,RGB(255,255,255));
	turn_time++;
	ReleaseDC(hWnd, hDC);
}

BOOL Excheckpattern(int x, int y, int increase_x, int increase_y , const char*pattern, int stone_color,int checkback)
//증감값 방향만 제외하고 나머지 일곱 방향의 패턴 검색 
//두줄 공격이나 두줄 수비시 중복 피하기 위함
{
	if(!(increase_x== -1 && increase_y == -1))
		 if(checkpattern(x, y, -1, -1 , pattern, stone_color, checkback)) return TRUE;
	if(!(increase_x== 0 && increase_y == -1))
		 if(checkpattern(x, y,  0, -1 , pattern, stone_color, checkback)) return TRUE;
	if(!(increase_x== +1 && increase_y == -1))
		 if(checkpattern(x, y, +1, -1 , pattern, stone_color, checkback)) return TRUE;
	if(!(increase_x== +1 && increase_y == 0))
		 if(checkpattern(x, y, +1,  0 , pattern, stone_color, checkback)) return TRUE;
	if(!(increase_x== +1 && increase_y == +1))
		 if(checkpattern(x, y, +1, +1 , pattern, stone_color, checkback)) return TRUE;
	if(!(increase_x== 0 && increase_y == +1))
		 if(checkpattern(x, y,  0, +1 , pattern, stone_color, checkback)) return TRUE;
	if(!(increase_x== -1 && increase_y == +1))
		 if(checkpattern(x, y, -1, +1 , pattern, stone_color, checkback)) return TRUE;
	if(!(increase_x== -1 && increase_y == 0))
		 if(checkpattern(x, y, -1,  0 , pattern, stone_color, checkback)) return TRUE;
	return FALSE;
}

BOOL checkpattern(int x, int y, int increase_x, int increase_y , const char *pattern, int stone_color,int checkback)
//checkback은 기준점 보다 뒷쪽 몇번째 부터 검사하는지를 나타내는 값
{
	int i, length = int(strlen(pattern)), stone = 0;
	int correctpattern[10]={0,};//패턴을 실제돌에 맞게 조정해줌
	x += increase_x * checkback;//검색 시작 위치 조정
	y += increase_y * checkback;
	for(i = 0 ; i < length ;i++)
	{
		if(pattern[i] == '1')
			correctpattern[i] = stone_color;
		if(pattern[i] == '2' && stone_color == BLACK_STONE)
			correctpattern[i] = WHITE_STONE;
		if(pattern[i] == '2' && stone_color == WHITE_STONE)
			correctpattern[i] = BLACK_STONE;
	}

	for(i = 0 ;i < length ;i++){
		if( x + (i * increase_x) >= 0 && x + (i * increase_x) <= 18 && 
			y + (i * increase_y) >= 0 && y + (i * increase_y) <= 18 ) {
			if(omok[x + (i * increase_x)][y + (i * increase_y)] == correctpattern[i] ) {
				stone++;
			} 
			else break;
		}
	}
	if(length == stone) 
		return TRUE;
	return 0;
}

void AI()
{
	int x, y;
//	char buf[256];
	CEnemy.AI(omok, comp, 1, &x, &y);
	putstone(x, y, comp);
	//sprintf(buf, "attack score : %d, deffence score : %d", CEnemy.attack.score, CEnemy.defence.score);
	//MessageBox(hWnd, buf, "Debug", MB_OK);
}

