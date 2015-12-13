#include <windows.h>
#include <stdlib.h>
#include <string.h>
#include <tchar.h>
#include "resource.h"
#include <time.h>
#include <math.h>

//PlaySound
//#include <Mmsystem.h>
#pragma comment(lib, "WINMM.LIB")

#define TIMER           1
int timer_i;
int backgound_i = 0;


#define WNDWIDTH        cxClient
#define WNDHEIGHT       cyClient
#define O_WNDWIDTH      380
#define O_WNDHEIGHT     650
#define WNDWIDTH_PLUS   0//18
#define WNDHEIGHT_PLUS  0//45
int cxClient = O_WNDWIDTH, cyClient = O_WNDHEIGHT;







//��Ϸ����
//�ɻ��ӵ����ٶ�Ѫ����Create������
#define ANI_STEP        6 //�������µ�֡Ƶ
#define STEP            3 //��Ӧ����������ƶ�����
int MOVE_HERO = 0;
#define BOSS_STEP       20

#define EPS             30//�ж��һ������е��ݲ�
#define EPS_TOY         0


#define NEW_HERO_BULLET  (new_enemy_interval(m_stage) / 2)//8
#define NEW_ENEMY_BULLET 12
#define NEW_ENEMY        new_enemy_interval(m_stage) //16

//��ɫ
#define WHITE           RGB(255,255,255)
#define BLACK           0
#define GRAY(x)         RGB(x,x,x)//�Ҷ�ֵ
#define RED             RGB(188,0,0)

#define FONT_DEFAULT_PARAS GB2312_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH | FF_DONTCARE
#define SEVEN_ZEROS 0,0,0,0,0,0,0

//�ɻ�
typedef struct
{
	HBITMAP	hBmp;
	POINT	pos;
	SIZE	size;
	int     speed;
	int		type;
	int     status;//��Ѫ������֡��/��ʱ�����ֱ����GetBlood��GetAniFromStatusȡ��
	int     full;  //��Ѫֵ
	int     score; //���еļӷ֣�-1��ʾ�ӷ��ѽ���
	int     how;//��¼�������������ֵ��ʾ��toyCLS����
}Plane;

#define die_for_cls 1
#define planeHERO 0
#define planeBOSS 3
#define PLANE_MAX_NUM   20 //�л��������
Plane m_Planes[PLANE_MAX_NUM + 2];
int Plane_Count;//����±�




//����
typedef struct
{
	HBITMAP	hBmp;
	POINT	pos;
	SIZE	size;
	int     speed;
	int		type;
}Toy;
//Toy.type �� toy.bmp �е�λ�ö�Ӧ

//���߹�����ȴʱ�䣬����������
#define COMMONBulletCDTime (15 + 1 * m_stage)

#define toyCLS    0

#define toyDBL    1    
	BOOL DBLBullet = FALSE;
	#define DBLBulletCDTime COMMONBulletCDTime
	int DBLBulletCDTimer;

#define toyBLD    2

#define toyRAD    3
	BOOL RADBullet = FALSE;
	#define RADBulletCDTime COMMONBulletCDTime
	int RADBulletCDTimer;

#define toySUB    4
	BOOL SUBBullet = FALSE;
	#define SUBBulletCDTime COMMONBulletCDTime
	int SUBBulletCDTimer;

#define toyMAX    4

#define TOY_MAX_NUM     50  //�����������
int Toy_Count;//����±�
Toy m_Toys[TOY_MAX_NUM+1];


//�ӵ�
typedef struct
{
	HBITMAP	hBmp;
	POINT	pos;
	SIZE	size;
	int	    speed;
	long    vx;
	int		type;
}Bullet;
//Bullet.type
#define bulletHERO  0 
#define bulletENEMY 1
#define BULLET_MAX_NUM  200 //�ӵ��������
Bullet m_Bullets[BULLET_MAX_NUM+1];
int Bullet_Count;//����±�
#define bullet_1_of_2 -1
#define bullet_2_of_2 1
#define bullet_left   -2
#define bullet_right  2
#define bullet_SUB_1  -4
#define bullet_SUB_2  -3
#define bullet_SUB_3  3
#define bullet_SUB_4  4

//��Ϸ״̬�ṹ��
typedef struct
{
	HBITMAP	hBmp;
	POINT   pos;
	SIZE	size;
	BOOL	isPaused;
	BOOL    isStarting;
	BOOL    isOver;
}GameStaus;


static TCHAR szWindowClass[] = _T("win32app");
static TCHAR szTitle[] = _T("��λ�ɻ���ս - ������");

//λͼ��Դ���
#define ENEMY_TYPE_NUM  3
HBITMAP m_hBackgroundBmp;
HBITMAP m_hHeroBmp;
HBITMAP m_hHeroBulletBmp;
HBITMAP	m_hEnemyBmp[ENEMY_TYPE_NUM];
HBITMAP m_hGameStatusBmp;
HBITMAP m_hToysBmp;
int	m_enemyBmpNames[] = { IDB_ENEMY_SMALL, IDB_ENEMY_MIDDLE, IDB_ENEMY_BIG };
//λͼ�ĳ���
#define Bullet_BMP_sx   10
#define Bullet_BMP_sy   23
#define Toy_BMP_sx      23
#define Toy_BMP_sy      34
#define BGWIDTH         377
#define BGHEIGHT        540




//��Ϸ״̬���ܷ���
GameStaus m_gameStatus;
int m_totalScore;
int m_stage;



//ȫ�ֺ���

//�������
LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);

//�ɻ��ӵ��Ĵ�����ɾ��
Plane CreatePlane(LONG x, LONG y, int type);
VOID CreateBullet(Plane FatherPlane, int type, int special);
VOID DestroyBullet(int iBullet);
VOID DestroyPlane(int iPlane);
Toy CreateToy(int type);
VOID DestroyToy(int iToy);


//��WinProc����ĺ���
VOID Render(HDC hdc);
VOID Hit(int bullet_i, int plane_i);
VOID HitHero(int plane_i);
VOID GetToy(int toy_i);
VOID Run_Timer();
VOID Move_Hero(WPARAM key);
VOID Initialize_Game();

//��ͣ
BOOL Paused(POINT);
VOID Pause_Game(HWND hWnd);

//������ȡ��ת����С����
int GetRand(int min, int max)//�����������������
{
	int i = (int)min + (rand() / (double)(RAND_MAX+1)) * ( max - min +1);
	return i;
}
VOID ConfirmNoLessThan(long * x, long standard)
{
	if(*x < standard)
		*x = standard;
}
VOID ConfirmNoMoreThan(long * x, long standard)
{
	if(*x > standard)
		*x = standard;
}
int GetAniFromStatus(int status)
{

	if(status >  0            ) return 0;
	if(status >= -1 * ANI_STEP) return 1;
	if(status >= -2 * ANI_STEP) return 2;
	if(status <  -2 * ANI_STEP) return 3;
	return 4;
}
int GetBlood(int i)
{
	if(i<=0) return 0;
	return i;
}
double stage_mtplr(int stage)//ȫ���ٶ�����ϵ��
{
	return min(sqrt(stage+0.0), 10);
}

int new_enemy_interval(int stage)
{
	int a[5] = {16,12,8,6,4};
	if(stage/4 <= 4)
		return a[stage/4];
	return 2;
}