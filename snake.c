#include<Windows.h>
#include<tchar.h>
#include<stdlib.h>
#include<time.h>
#include<stdio.h>



#define ROW         24
#define COLUMN      32

HWND hwnd = NULL;

BYTE(*base)[32] = (BYTE(*)[32])0x01005340;



WNDPROC prevWndProc;

int move_table[ROW+1][COLUMN+1];
int dir[8] = {0, -1, 0, 1, 1, 0, -1, 0};
int head[2] = {1, 1};
int bait[2] = {ROW/2, COLUMN/2};
int currentDir[2] = {0, 1};
int snakeLen = 2;
int forbidDir[2] = {0, -1};


DWORD WINAPI update(LPVOID){
    while(true){
    		for(int i = 1; i <= ROW; i++){
			for(int j = 1; j <= COLUMN; j++){
				if(move_table[i][j])		
			   		base[i][j] = 0x8A;
				else
					base[i][j] = 0x0F;
			}
		}
		
		base[bait[0]][bait[1]] = 0x0E;
		InvalidateRect(hwnd, 0, FALSE);
		Sleep(50);
	}

}

void reset_game(){
	for(int i = 1; i <= ROW; i++)
		for(int j = 1; j <= COLUMN; j++)
			move_table[i][j] = 0;

	head[0] = 1;
	head[1] = 1;
	currentDir[0] = 0;
	currentDir[1] = 1;
	forbidDir[0] = -1*currentDir[0];
	forbidDir[1] = -1*currentDir[1];
	snakeLen = 2;
}

void default_move(){
	forbidDir[0] = -1*currentDir[0];
	forbidDir[1] = -1*currentDir[1];
	head[0] += currentDir[0];
	head[1] += currentDir[1];
	

	if(head[0] >= ROW || head[0] <= 0 || 
        head[1] >= COLUMN || head[1] <= 0 ||
		move_table[head[0]][head[1]] )
      {
	    reset_game();
       }

	move_table[head[0]][head[1]] = snakeLen + 1;

     if(head[0] == bait[0] && head[1] == bait[1]){
         while(true){
             int temp[2] = {rand()%(ROW-1)+1, rand()%(COLUMN-1)+1};
             if(!move_table[temp[0]][temp[1]]){
                 bait[0] = temp[0];
			 bait[1] = temp[1];
                  break;
              }
          }
          snakeLen += 1;
          head[0] += currentDir[0];
          head[1] += currentDir[1];
          move_table[head[0]][head[1]] = snakeLen + 1;
      }
	

	//update move table
	for(int i = 1; i < ROW; i++)
		for(int j = 1; j < COLUMN; j++)
			if(move_table[i][j])
				move_table[i][j]--;
				
}


LRESULT CALLBACK myNewWndProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{	
	
    *(int*)0x01005194 = 30;
    InvalidateRect(hwnd, 0, FALSE);

    switch(uMsg)
    {	  
        case WM_KEYDOWN: 
            switch (wParam) 
            { 
                case VK_LEFT: 
                    *(int*)0x01005194 = 2;
				if((forbidDir[0] != 0) || (forbidDir[1] != -1)){
					currentDir[0] = 0;
					currentDir[1] = -1;
				}
				break;
                case VK_RIGHT: 
                    *(int*)0x01005194 = 3;
				if((forbidDir[0] != 0) || (forbidDir[1] != 1)){
					currentDir[0] = 0;
					currentDir[1] = 1;
				}
				break; 
                case VK_UP: 
                    *(int*)0x01005194 = 0;
				if((forbidDir[0] != -1) || (forbidDir[1] != 0)){
					currentDir[0] = -1;
					currentDir[1] = 0;
				}
				break; 
                case VK_DOWN: 
                    *(int*)0x01005194 = 1;
				if((forbidDir[0] != 1) || (forbidDir[1] != 0)){
					currentDir[0] = 1;
					currentDir[1] = 0;
				}
				break;
			default:
				break;
		}
	   
    }

    return CallWindowProc(prevWndProc, hwnd, uMsg, wParam, lParam);
}


DWORD WINAPI controller(LPVOID){
	reset_game();
	while(true){
		default_move();
		Sleep(200);
	}
}



BOOL WINAPI DllMain(HINSTANCE hinstDll, DWORD dwReason, LPVOID lpvRevered) {
    switch (dwReason) {
    case DLL_PROCESS_ATTACH:
	   srand(time(NULL));
	   DisableThreadLibraryCalls(hinstDll);
        CreateThread(NULL, 0, update, NULL, 0, NULL);
	   CreateThread(NULL, 0, controller, NULL, 0, NULL);

        hwnd = FindWindow(NULL, "Minesweeper");
	   prevWndProc = (WNDPROC) SetWindowLongPtr(hwnd, GWL_WNDPROC, (LONG_PTR)&myNewWndProc);

        break;
    case DLL_THREAD_ATTACH:
        break;
    case DLL_THREAD_DETACH:
        break;
    case DLL_PROCESS_DETACH:
        break;
    }
    return TRUE;
}