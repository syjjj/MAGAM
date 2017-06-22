// include the basic windows header files and the Direct3D header file
#include <windows.h>
#include <windowsx.h>
#include <d3d9.h>
#include <d3dx9.h>
#include <iostream>

// define the screen resolution and keyboard macros
#define SCREEN_WIDTH  480
#define SCREEN_HEIGHT 640
#define KEY_DOWN(vk_code) ((GetAsyncKeyState(vk_code) & 0x8000) ? 1 : 0)
#define KEY_UP(vk_code) ((GetAsyncKeyState(vk_code) & 0x8000) ? 0 : 1)

#define ENEMY_NUM 10 
#define BULLET_NUM 20 

// include the Direct3D Library file
#pragma comment (lib, "d3d9.lib")
#pragma comment (lib, "d3dx9.lib")

// global declarations
LPDIRECT3D9 d3d;    // the pointer to our Direct3D interface
LPDIRECT3DDEVICE9 d3ddev;    // the pointer to the device class
LPD3DXSPRITE d3dspt;    // the pointer to our Direct3D Sprite interface



						// sprite declarations
LPDIRECT3DTEXTURE9 sprite;    // the pointer to the sprite
LPDIRECT3DTEXTURE9 sprite_hero; 
LPDIRECT3DTEXTURE9 sprite_hero_break;
LPDIRECT3DTEXTURE9 sprite_enemy;
LPDIRECT3DTEXTURE9 sprite_enemy2;
LPDIRECT3DTEXTURE9 sprite_enemy3;
LPDIRECT3DTEXTURE9 sprite_bullet;
LPDIRECT3DTEXTURE9 sprite_bullet2;
LPDIRECT3DTEXTURE9 sprite_bullet3;

//
LPDIRECT3DTEXTURE9 sprite_back;
LPDIRECT3DTEXTURE9 sprite_backright;
LPDIRECT3DTEXTURE9 sprite_backbottom;
LPDIRECT3DTEXTURE9 sprite_title;
LPDIRECT3DTEXTURE9 sprite_titlemungu;

LPDIRECT3DTEXTURE9 sprite_ending;
LPDIRECT3DTEXTURE9 sprite_endmungu;
//

LPDIRECT3DTEXTURE9 crash1;
LPDIRECT3DTEXTURE9 crash2;
LPDIRECT3DTEXTURE9 item;
									 // function prototypes
void initD3D(HWND hWnd);    // sets up and initializes Direct3D
void render_frame(void);    // renders a single frame
void cleanD3D(void);		// closes Direct3D and releases memory

void init_game(void);
void do_game_logic(void);
bool sphere_collision_check(float x0, float y0, float size0, float x1, float y1, float size1);

//scene
bool title = true;
bool play = false;
bool gameover = false;
bool death = false;

//
int power = 1;

// the WindowProc function prototype
LRESULT CALLBACK WindowProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

using namespace std;


enum { MOVE_UP, MOVE_DOWN, MOVE_LEFT, MOVE_RIGHT };


//기본 클래스 
class entity {

public:
	float x_pos;
	float y_pos;
	int status;
	int HP;

};


bool sphere_collision_check(float x0, float y0, float size0, float x1, float y1, float size1)
{

	if ((x0 - x1)*(x0 - x1) + (y0 - y1)*(y0 - y1) < (size0 + size1) * (size0 + size1))
		return true;
	else
		return false;

}

//아이템
class Item :public entity {

public:
	bool ishow;
	void move();
	void init(float x, float y);
};

void Item::init(float x, float y)
{

	x_pos = x;
	y_pos = y;

}


void Item::move()
{
	y_pos += 3;

}


//주인공 클래스 
class Hero :public entity {

public:

	bool hshow;
	int hitcount;

	void move(int i);
	void init(float x, float y);
	bool check_collision(float x, float y);


};

void Hero::init(float x, float y)
{
	hitcount = 0;
	x_pos = x;
	y_pos = y;


}

void Hero::move(int i)
{
	switch (i)
	{
	case MOVE_UP:
		y_pos -= 6;
		break;

	case MOVE_DOWN:
		y_pos += 6;
		break;


	case MOVE_LEFT:
		x_pos -= 6;
		break;


	case MOVE_RIGHT:
		x_pos += 6;
		break;

	}

	if (x_pos < 0)
		x_pos = 0;
	if (x_pos > 360)
		x_pos = 360;
	if (y_pos < 0)
		y_pos = 0;
	if (y_pos > 550)
		y_pos = 550;
}

bool Hero::check_collision(float x, float y)
{

	//충돌 처리 시 
	if (sphere_collision_check(x_pos, y_pos, 22, x, y, 18) == true)
	{
		hshow = false;
		return true;

	}
	else {

		return false;
	}
}



// 적 클래스 
class Enemy :public entity {

public:

	bool eShow;

	void fire();
	void init(float x, float y);
	void move();

	void active();
	void hide();
};

void Enemy::init(float x, float y)
{

	x_pos = x;
	y_pos = y;

}


void Enemy::move()
{
	y_pos += 2;

}

void Enemy::active()
{
	eShow = true;

}

void Enemy::hide()
{
	eShow = false;

}





// 총알 클래스 
class Bullet :public entity {

public:
	bool bShow;

	void init(float x, float y);
	void move();
	bool show();
	void hide();
	void active();
	bool check_collision(float x, float y);


};


bool Bullet::check_collision(float x, float y)
{
		//충돌 처리 시 
		if (sphere_collision_check(x_pos, y_pos, 32, x, y, 32) == true)
		{
			bShow = false;
			return true;

		}
		else {

			return false;
		}
}




void Bullet::init(float x, float y)
{
	x_pos = x;
	y_pos = y;

}



bool Bullet::show()
{
	return bShow;

}


void Bullet::active()
{
	bShow = true;

}



void Bullet::move()
{
		y_pos -= 15;
}

void Bullet::hide()
{
	bShow = false;

}






//객체 생성 
Hero hero;
Enemy enemy[ENEMY_NUM];
Bullet bullet[BULLET_NUM];


// the entry point for any Windows program
int WINAPI WinMain(HINSTANCE hInstance,
	HINSTANCE hPrevInstance,
	LPSTR lpCmdLine,
	int nCmdShow)
{
	HWND hWnd;
	WNDCLASSEX wc;

	ZeroMemory(&wc, sizeof(WNDCLASSEX));

	wc.cbSize = sizeof(WNDCLASSEX);
	wc.style = CS_HREDRAW | CS_VREDRAW;
	wc.lpfnWndProc = (WNDPROC)WindowProc;
	wc.hInstance = hInstance;
	wc.hCursor = LoadCursor(NULL, IDC_ARROW);
	wc.lpszClassName = L"WindowClass";

	RegisterClassEx(&wc);

	hWnd = CreateWindowEx(NULL, L"WindowClass", L"MAGAM",
		WS_EX_TOPMOST | WS_BORDER, 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT,
		NULL, NULL, hInstance, NULL);

	ShowWindow(hWnd, nCmdShow);

	// set up and initialize Direct3D
	initD3D(hWnd);


	//게임 오브젝트 초기화 
	init_game();

	// enter the main loop:

	MSG msg;

	while (TRUE)
	{
		DWORD starting_point = GetTickCount();

		if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{
			if (msg.message == WM_QUIT)
				break;

			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}

		do_game_logic();

		render_frame();

		// check the 'escape' key
		if (KEY_DOWN(VK_ESCAPE))
			PostMessage(hWnd, WM_DESTROY, 0, 0);




		while ((GetTickCount() - starting_point) < 25);
	}

	// clean up DirectX and COM
	cleanD3D();

	return msg.wParam;
}


// this is the main message handler for the program
LRESULT CALLBACK WindowProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
	case WM_DESTROY:
	{
		PostQuitMessage(0);
		return 0;
	} break;
	}

	return DefWindowProc(hWnd, message, wParam, lParam);
}


// this function initializes and prepares Direct3D for use
void initD3D(HWND hWnd)
{
	d3d = Direct3DCreate9(D3D_SDK_VERSION);

	D3DPRESENT_PARAMETERS d3dpp;

	ZeroMemory(&d3dpp, sizeof(d3dpp));
	d3dpp.Windowed = TRUE;
	d3dpp.SwapEffect = D3DSWAPEFFECT_DISCARD;
	d3dpp.hDeviceWindow = hWnd;
	d3dpp.BackBufferFormat = D3DFMT_X8R8G8B8;
	d3dpp.BackBufferWidth = SCREEN_WIDTH;
	d3dpp.BackBufferHeight = SCREEN_HEIGHT;


	// create a device class using this information and the info from the d3dpp stuct
	d3d->CreateDevice(D3DADAPTER_DEFAULT,
		D3DDEVTYPE_HAL,
		hWnd,
		D3DCREATE_SOFTWARE_VERTEXPROCESSING,
		&d3dpp,
		&d3ddev);

	D3DXCreateSprite(d3ddev, &d3dspt);    // create the Direct3D Sprite object

	D3DXCreateTextureFromFileEx(d3ddev,    // the device pointer
		L"Panel3.png",    // the file name
		D3DX_DEFAULT,    // default width
		D3DX_DEFAULT,    // default height
		D3DX_DEFAULT,    // no mip mapping
		NULL,    // regular usage
		D3DFMT_A8R8G8B8,    // 32-bit pixels with alpha
		D3DPOOL_MANAGED,    // typical memory handling
		D3DX_DEFAULT,    // no filtering
		D3DX_DEFAULT,    // no mip filtering
		D3DCOLOR_XRGB(255, 0, 255),    // the hot-pink color key
		NULL,    // no image info struct
		NULL,    // not using 256 colors
		&sprite);    // load to sprite

	D3DXCreateTextureFromFileEx(d3ddev,    // the device pointer
		L"Resource/back.png",    // the file name
		480,    // default width
		640,    // default height
		D3DX_DEFAULT,    // no mip mapping
		NULL,    // regular usage
		D3DFMT_A8R8G8B8,    // 32-bit pixels with alpha
		D3DPOOL_MANAGED,    // typical memory handling
		D3DX_DEFAULT,    // no filtering
		D3DX_DEFAULT,    // no mip filtering
		D3DCOLOR_XRGB(255, 0, 255),    // the hot-pink color key
		NULL,    // no image info struct
		NULL,    // not using 256 colors
		&sprite_back);    // load to sprite

	D3DXCreateTextureFromFileEx(d3ddev,    // the device pointer
		L"Resource/startmungu.png",    // the file name
		281,    // default width
		24,    // default height
		D3DX_DEFAULT,    // no mip mapping
		NULL,    // regular usage
		D3DFMT_A8R8G8B8,    // 32-bit pixels with alpha
		D3DPOOL_MANAGED,    // typical memory handling
		D3DX_DEFAULT,    // no filtering
		D3DX_DEFAULT,    // no mip filtering
		D3DCOLOR_XRGB(255, 0, 255),    // the hot-pink color key
		NULL,    // no image info struct
		NULL,    // not using 256 colors
		&sprite_titlemungu);    // load to sprite

	D3DXCreateTextureFromFileEx(d3ddev,    // the device pointer
		L"Resource/gameend.png",    // the file name
		247,    // default width
		25,    // default height
		D3DX_DEFAULT,    // no mip mapping
		NULL,    // regular usage
		D3DFMT_A8R8G8B8,    // 32-bit pixels with alpha
		D3DPOOL_MANAGED,    // typical memory handling
		D3DX_DEFAULT,    // no filtering
		D3DX_DEFAULT,    // no mip filtering
		D3DCOLOR_XRGB(255, 0, 255),    // the hot-pink color key
		NULL,    // no image info struct
		NULL,    // not using 256 colors
		&sprite_ending);    // load to sprite

	D3DXCreateTextureFromFileEx(d3ddev,    // the device pointer
		L"Resource/endmungu.png",    // the file name
		177,    // default width
		44,    // default height
		D3DX_DEFAULT,    // no mip mapping
		NULL,    // regular usage
		D3DFMT_A8R8G8B8,    // 32-bit pixels with alpha
		D3DPOOL_MANAGED,    // typical memory handling
		D3DX_DEFAULT,    // no filtering
		D3DX_DEFAULT,    // no mip filtering
		D3DCOLOR_XRGB(255, 0, 255),    // the hot-pink color key
		NULL,    // no image info struct
		NULL,    // not using 256 colors
		&sprite_endmungu);    // load to sprite

	D3DXCreateTextureFromFileEx(d3ddev,    // the device pointer
		L"Resource/backright.png",    // the file name
		72,    // default width
		640,    // default height
		D3DX_DEFAULT,    // no mip mapping
		NULL,    // regular usage
		D3DFMT_A8R8G8B8,    // 32-bit pixels with alpha
		D3DPOOL_MANAGED,    // typical memory handling
		D3DX_DEFAULT,    // no filtering
		D3DX_DEFAULT,    // no mip filtering
		D3DCOLOR_XRGB(255, 0, 255),    // the hot-pink color key
		NULL,    // no image info struct
		NULL,    // not using 256 colors
		&sprite_backright);    // load to sprite

	D3DXCreateTextureFromFileEx(d3ddev,    // the device pointer
		L"Resource/backbottom.png",    // the file name
		480,    // default width
		49,    // default height
		D3DX_DEFAULT,    // no mip mapping
		NULL,    // regular usage
		D3DFMT_A8R8G8B8,    // 32-bit pixels with alpha
		D3DPOOL_MANAGED,    // typical memory handling
		D3DX_DEFAULT,    // no filtering
		D3DX_DEFAULT,    // no mip filtering
		D3DCOLOR_XRGB(255, 0, 255),    // the hot-pink color key
		NULL,    // no image info struct
		NULL,    // not using 256 colors
		&sprite_backbottom);    // load to sprite

	D3DXCreateTextureFromFileEx(d3ddev,    // the device pointer
		L"Resource/title.png",    // the file name
		245,    // default width
		45,    // default height
		D3DX_DEFAULT,    // no mip mapping
		NULL,    // regular usage
		D3DFMT_A8R8G8B8,    // 32-bit pixels with alpha
		D3DPOOL_MANAGED,    // typical memory handling
		D3DX_DEFAULT,    // no filtering
		D3DX_DEFAULT,    // no mip filtering
		D3DCOLOR_XRGB(255, 0, 255),    // the hot-pink color key
		NULL,    // no image info struct
		NULL,    // not using 256 colors
		&sprite_title);    // load to sprite
	///////////////////////////////////////////////////

	D3DXCreateTextureFromFileEx(d3ddev,    // the device pointer
		L"Resource/player.png",    // the file name
		45,    // default width
		35,    // default height
		D3DX_DEFAULT,    // no mip mapping
		NULL,    // regular usage
		D3DFMT_A8R8G8B8,    // 32-bit pixels with alpha
		D3DPOOL_MANAGED,    // typical memory handling
		D3DX_DEFAULT,    // no filtering
		D3DX_DEFAULT,    // no mip filtering
		D3DCOLOR_XRGB(255, 0, 255),    // the hot-pink color key
		NULL,    // no image info struct
		NULL,    // not using 256 colors
		&sprite_hero);    // load to sprite

	D3DXCreateTextureFromFileEx(d3ddev,    // the device pointer
		L"Resource/player_death.png",    // the file name
		45,    // default width
		35,    // default height
		D3DX_DEFAULT,    // no mip mapping
		NULL,    // regular usage
		D3DFMT_A8R8G8B8,    // 32-bit pixels with alpha
		D3DPOOL_MANAGED,    // typical memory handling
		D3DX_DEFAULT,    // no filtering
		D3DX_DEFAULT,    // no mip filtering
		D3DCOLOR_XRGB(255, 0, 255),    // the hot-pink color key
		NULL,    // no image info struct
		NULL,    // not using 256 colors
		&sprite_hero_break);    // load to sprite

	D3DXCreateTextureFromFileEx(d3ddev,    // the device pointer
		L"Resource/enemy1.png",    // the file name
		65,    // default width
		65,    // default height
		D3DX_DEFAULT,    // no mip mapping
		NULL,    // regular usage
		D3DFMT_A8R8G8B8,    // 32-bit pixels with alpha
		D3DPOOL_MANAGED,    // typical memory handling
		D3DX_DEFAULT,    // no filtering
		D3DX_DEFAULT,    // no mip filtering
		D3DCOLOR_XRGB(255, 0, 255),    // the hot-pink color key
		NULL,    // no image info struct
		NULL,    // not using 256 colors
		&sprite_enemy);    // load to sprite

	D3DXCreateTextureFromFileEx(d3ddev,    // the device pointer
		L"Resource/enemy2.png",    // the file name
		65,    // default width
		65,    // default height
		D3DX_DEFAULT,    // no mip mapping
		NULL,    // regular usage
		D3DFMT_A8R8G8B8,    // 32-bit pixels with alpha
		D3DPOOL_MANAGED,    // typical memory handling
		D3DX_DEFAULT,    // no filtering
		D3DX_DEFAULT,    // no mip filtering
		D3DCOLOR_XRGB(255, 0, 255),    // the hot-pink color key
		NULL,    // no image info struct
		NULL,    // not using 256 colors
		&sprite_enemy2);    // load to sprite

	D3DXCreateTextureFromFileEx(d3ddev,    // the device pointer
		L"Resource/enemy3.png",    // the file name
		65,    // default width
		65,    // default height
		D3DX_DEFAULT,    // no mip mapping
		NULL,    // regular usage
		D3DFMT_A8R8G8B8,    // 32-bit pixels with alpha
		D3DPOOL_MANAGED,    // typical memory handling
		D3DX_DEFAULT,    // no filtering
		D3DX_DEFAULT,    // no mip filtering
		D3DCOLOR_XRGB(255, 0, 255),    // the hot-pink color key
		NULL,    // no image info struct
		NULL,    // not using 256 colors
		&sprite_enemy3);    // load to sprite

	D3DXCreateTextureFromFileEx(d3ddev,    // the device pointer
		L"Resource/attack01.png",    // the file name
		5,    // default width
		25,    // default height
		D3DX_DEFAULT,    // no mip mapping
		NULL,    // regular usage
		D3DFMT_A8R8G8B8,    // 32-bit pixels with alpha
		D3DPOOL_MANAGED,    // typical memory handling
		D3DX_DEFAULT,    // no filtering
		D3DX_DEFAULT,    // no mip filtering
		D3DCOLOR_XRGB(255, 0, 255),    // the hot-pink color key
		NULL,    // no image info struct
		NULL,    // not using 256 colors
		&sprite_bullet);    // load to sprite

	D3DXCreateTextureFromFileEx(d3ddev,    // the device pointer
		L"Resource/attack02.png",    // the file name
		15,    // default width
		25,    // default height
		D3DX_DEFAULT,    // no mip mapping
		NULL,    // regular usage
		D3DFMT_A8R8G8B8,    // 32-bit pixels with alpha
		D3DPOOL_MANAGED,    // typical memory handling
		D3DX_DEFAULT,    // no filtering
		D3DX_DEFAULT,    // no mip filtering
		D3DCOLOR_XRGB(255, 0, 255),    // the hot-pink color key
		NULL,    // no image info struct
		NULL,    // not using 256 colors
		&sprite_bullet2);    // load to sprite

	D3DXCreateTextureFromFileEx(d3ddev,    // the device pointer
		L"Resource/attack03.png",    // the file name
		25,    // default width
		25,    // default height
		D3DX_DEFAULT,    // no mip mapping
		NULL,    // regular usage
		D3DFMT_A8R8G8B8,    // 32-bit pixels with alpha
		D3DPOOL_MANAGED,    // typical memory handling
		D3DX_DEFAULT,    // no filtering
		D3DX_DEFAULT,    // no mip filtering
		D3DCOLOR_XRGB(255, 0, 255),    // the hot-pink color key
		NULL,    // no image info struct
		NULL,    // not using 256 colors
		&sprite_bullet3);    // load to sprite

	D3DXCreateTextureFromFileEx(d3ddev,    // the device pointer
		L"Resource/crash1.png",    // the file name
		65,    // default width
		65,    // default height
		D3DX_DEFAULT,    // no mip mapping
		NULL,    // regular usage
		D3DFMT_A8R8G8B8,    // 32-bit pixels with alpha
		D3DPOOL_MANAGED,    // typical memory handling
		D3DX_DEFAULT,    // no filtering
		D3DX_DEFAULT,    // no mip filtering
		D3DCOLOR_XRGB(255, 0, 255),    // the hot-pink color key
		NULL,    // no image info struct
		NULL,    // not using 256 colors
		&crash1);    // load to sprite

	D3DXCreateTextureFromFileEx(d3ddev,    // the device pointer
		L"Resource/crash2.png",    // the file name
		65,    // default width
		65,    // default height
		D3DX_DEFAULT,    // no mip mapping
		NULL,    // regular usage
		D3DFMT_A8R8G8B8,    // 32-bit pixels with alpha
		D3DPOOL_MANAGED,    // typical memory handling
		D3DX_DEFAULT,    // no filtering
		D3DX_DEFAULT,    // no mip filtering
		D3DCOLOR_XRGB(255, 0, 255),    // the hot-pink color key
		NULL,    // no image info struct
		NULL,    // not using 256 colors
		&crash2);    // load to sprite

	D3DXCreateTextureFromFileEx(d3ddev,    // the device pointer
		L"Resource/powerup.png",    // the file name
		25,    // default width
		20,    // default height
		D3DX_DEFAULT,    // no mip mapping
		NULL,    // regular usage
		D3DFMT_A8R8G8B8,    // 32-bit pixels with alpha
		D3DPOOL_MANAGED,    // typical memory handling
		D3DX_DEFAULT,    // no filtering
		D3DX_DEFAULT,    // no mip filtering
		D3DCOLOR_XRGB(255, 0, 255),    // the hot-pink color key
		NULL,    // no image info struct
		NULL,    // not using 256 colors
		&item);    // load to sprite

	return;
}


void init_game(void)
{
	death = false;
	hero.hitcount = 0;

	//객체 초기화 
	hero.init(210, 500);

	//적들 초기화 
	for (int i = 0; i<ENEMY_NUM; i++)
	{

		enemy[i].init((float)(rand() % 400), rand() % 200 - 300);
	}

	//총알 초기화 
	for (int i = 0; i < BULLET_NUM; i++)
	{
		bullet[i].init(hero.x_pos, hero.y_pos);
	}

}


void do_game_logic(void)
{

	//주인공 처리 
	if (KEY_DOWN(VK_UP))
		hero.move(MOVE_UP);

	if (KEY_DOWN(VK_DOWN))
		hero.move(MOVE_DOWN);

	if (KEY_DOWN(VK_LEFT))
		hero.move(MOVE_LEFT);

	if (KEY_DOWN(VK_RIGHT))
		hero.move(MOVE_RIGHT);

	for (int i = 0; i < ENEMY_NUM; i++)
	{
		if (hero.check_collision(enemy[i].x_pos, enemy[i].y_pos) == true)
		{

			hero.hitcount++;
			if (hero.hitcount == 1)
			{
				hero.y_pos += 70;


			}
			if (hero.hitcount == 2)
			{
				hero.y_pos += 70;

				death = true;
				
				play = false;

				gameover = true;

			}
		}
	}

	//적들 처리 
	for (int i = 0; i<ENEMY_NUM; i++)
	{
		if (enemy[i].y_pos > 640)
		{
			enemy[i].init((float)(rand() % 300), rand() % 200 - 350);
			
		}
		else
			enemy[i].move();
	}


	//총알 처리 
	for (int i = 0; i < BULLET_NUM; i++)
	{
	if (bullet[i].show() == false)
	{
		if (KEY_DOWN(VK_SPACE))
		{
			bullet[i].active();
			bullet[i].init(hero.x_pos+20, hero.y_pos-10);
		}
	}


	if (bullet[i].show() == true)
	{
		if (bullet[i].y_pos < -70)
			bullet[i].hide();
		else
			bullet[i].move();

		//충돌 처리 
		for (int j = 0; j < ENEMY_NUM; j++)
		{
			if (bullet[i].check_collision(enemy[j].x_pos, enemy[j].y_pos) == true)
			{
				enemy[j].init((float)(rand() % 300), rand() % 200 - 300);

				bullet[i].hide();
			}
		}


	}
	}


}

// this is the function used to render a single frame
void render_frame(void)
{
	// clear the window to a deep blue
	d3ddev->Clear(0, NULL, D3DCLEAR_TARGET, D3DCOLOR_XRGB(0, 0, 0), 1.0f, 0);

	d3ddev->BeginScene();    // begins the 3D scene

	d3dspt->Begin(D3DXSPRITE_ALPHABLEND);    // // begin sprite drawing with transparency

											 //UI 창 렌더링 


											 
											 //static int frame = 21;    // start the program on the final frame
											 //if(KEY_DOWN(VK_SPACE)) frame=0;     // when the space key is pressed, start at frame 0
											 //if(frame < 21) frame++;     // if we aren't on the last frame, go to the next frame

											 //// calculate the x-position
											 //int xpos = frame * 182 + 1;

											 //RECT part;
											 //SetRect(&part, xpos, 0, xpos + 181, 128);
											 //D3DXVECTOR3 center(0.0f, 0.0f, 0.0f);    // center at the upper-left corner
											 //D3DXVECTOR3 position(150.0f, 50.0f, 0.0f);    // position at 50, 50 with no depth
											 //d3dspt->Draw(sprite, &part, &center, &position, D3DCOLOR_ARGB(127, 255, 255, 255));

	if (title == true)
	{
		//title
		RECT part;
		SetRect(&part, 0, 0, 480, 640);
		D3DXVECTOR3 center(0.0f, 0.0f, 0.0f);    // center at the upper-left corner
		D3DXVECTOR3 position(0.0f, 0.0f, 0.0f);    // position at 50, 50 with no depth
		d3dspt->Draw(sprite_back, &part, &center, &position, D3DCOLOR_ARGB(255, 255, 255, 255));

		RECT part1;
		SetRect(&part1, 0, 0, 245, 45);
		D3DXVECTOR3 center1(0.0f, 0.0f, 0.0f);    // center at the upper-left corner
		D3DXVECTOR3 position1(120, 240, 0.0f);    // position at 50, 50 with no depth
		d3dspt->Draw(sprite_title, &part1, &center1, &position1, D3DCOLOR_ARGB(255, 255, 255, 255));

		RECT part2;
		SetRect(&part2, 0, 0, 281, 24);
		D3DXVECTOR3 center2(0.0f, 0.0f, 0.0f);    // center at the upper-left corner
		D3DXVECTOR3 position2(100, 550, 0.0f);    // position at 50, 50 with no depth	
		d3dspt->Draw(sprite_titlemungu, &part2, &center2, &position2, D3DCOLOR_ARGB(255, 255, 255, 255));


		if (KEY_DOWN(VK_SPACE))
		{
			title = false;
			play = true;
		}
	}

	if (play == true)
	{

		//주인공 
		RECT part;
		SetRect(&part, 0, 0, 45, 35);
		D3DXVECTOR3 center(0.0f, 0.0f, 0.0f);    // center at the upper-left corner
		D3DXVECTOR3 position(hero.x_pos, hero.y_pos, 0.0f);    // position at 50, 50 with no depth
		if (hero.hitcount > 0)
			d3dspt->Draw(sprite_hero_break, &part, &center, &position, D3DCOLOR_ARGB(255, 255, 255, 255));
		else
			d3dspt->Draw(sprite_hero, &part, &center, &position, D3DCOLOR_ARGB(255, 255, 255, 255));

		////총알 
		if (death == false)
		{
			for (int i = 0; i < BULLET_NUM; i++)
			{
				if (bullet[i].bShow == true)
				{
					RECT part1;
					SetRect(&part1, 0, 0, 5, 25);
					D3DXVECTOR3 center1(0.0f, 0.0f, 0.0f);    // center at the upper-left corner
					D3DXVECTOR3 position1(bullet[i].x_pos, bullet[i].y_pos, 0.0f);    // position at 50, 50 with no depth
					d3dspt->Draw(sprite_bullet, &part1, &center1, &position1, D3DCOLOR_ARGB(255, 255, 255, 255));
				}
			}
		}
		////에네미 
		RECT part2;
		SetRect(&part2, 0, 0, 65, 65);
		D3DXVECTOR3 center2(0.0f, 0.0f, 0.0f);    // center at the upper-left corner
		int num[ENEMY_NUM];
		for (int i = 0; i < ENEMY_NUM; i++)
		{

			D3DXVECTOR3 position2(enemy[i].x_pos, enemy[i].y_pos, 0.0f);    // position at 50, 50 with no depth
			num[i]= rand();
			switch (num[i]%3)
			{
			case 0:
				d3dspt->Draw(sprite_enemy, &part2, &center2, &position2, D3DCOLOR_ARGB(255, 255, 255, 255));
				break;
			case 1:
				d3dspt->Draw(sprite_enemy2, &part2, &center2, &position2, D3DCOLOR_ARGB(255, 255, 255, 255));
				break;
			case 2:
				d3dspt->Draw(sprite_enemy3, &part2, &center2, &position2, D3DCOLOR_ARGB(255, 255, 255, 255));
				break;
			}
		}

		RECT part4;
		SetRect(&part4, 0, 0, 72, 640);
		D3DXVECTOR3 center4(0.0f, 0.0f, 0.0f);    // center at the upper-left corner
		D3DXVECTOR3 position4(408, 0.0f, 0.0f);    // position at 50, 50 with no depth
		d3dspt->Draw(sprite_backright, &part4, &center4, &position4, D3DCOLOR_ARGB(255, 255, 255, 255));
		
		RECT part5;
		SetRect(&part5, 0, 0, 480, 49);
		D3DXVECTOR3 center5(0.0f, 0.0f, 0.0f);    // center at the upper-left corner
		D3DXVECTOR3 position5(0.0f, 591, 0.0f);    // position at 50, 50 with no depth
		d3dspt->Draw(sprite_backbottom, &part5, &center5, &position5, D3DCOLOR_ARGB(255, 255, 255, 255));

	}
	
	if (gameover== true)
	{
		RECT part;
		SetRect(&part, 0, 0, 247, 25);
		D3DXVECTOR3 center(0.0f, 0.0f, 0.0f);    // center at the upper-left corner
		D3DXVECTOR3 position(130, 250, 0.0f);    // position at 50, 50 with no depth
		d3dspt->Draw(sprite_ending, &part, &center, &position, D3DCOLOR_ARGB(255, 255, 255, 255));

		RECT part2;
		SetRect(&part2, 0, 0, 177, 44);
		D3DXVECTOR3 center2(0.0f, 0.0f, 0.0f);    // center at the upper-left corner
		D3DXVECTOR3 position2(150, 400, 0.0f);    // position at 50, 50 with no depth
		d3dspt->Draw(sprite_endmungu, &part2, &center2, &position2, D3DCOLOR_ARGB(255, 255, 255, 255));

		if (KEY_DOWN(VK_SPACE))
		{
			gameover = false;
			play = true;

			init_game();
		}
	}

	d3dspt->End();    // end sprite drawing

	d3ddev->EndScene();    // ends the 3D scene

	d3ddev->Present(NULL, NULL, NULL, NULL);

	return;
}


// this is the function that cleans up Direct3D and COM
void cleanD3D(void)
{
	sprite->Release();
	d3ddev->Release();
	d3d->Release();

	//객체 해제 
	sprite_hero->Release();
	sprite_enemy->Release();
	sprite_bullet->Release();

	return;
}
