/*

������̣�1��
����ҵ
��λ�ɻ���ս
2014��11��
������ 2014013432

*/

//#define DEBUGING  //��ӡ������Ϣ

#include "DigitPlane.h"

int WINAPI WinMain(HINSTANCE hInstance,
	HINSTANCE hPrevInstance,
	LPSTR lpCmdLine,
	int nCmdShow)
{
	WNDCLASSEX wcex;
	HWND hWnd;
	MSG msg;

	wcex.cbSize = sizeof(WNDCLASSEX);
	wcex.style = CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc = WndProc;
	wcex.cbClsExtra = 0;
	wcex.cbWndExtra = 0;
	wcex.hInstance = hInstance;
	wcex.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_APP_ICON));
	wcex.hCursor = LoadCursor(NULL, IDC_ARROW);
	wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
	wcex.lpszMenuName = NULL;
	wcex.lpszClassName = szWindowClass;
	wcex.hIconSm = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_APPLICATION));

	if (!RegisterClassEx(&wcex))
	{
		MessageBox(NULL,
			_T("Call to RegisterClassEx failed!"),
			_T("Mega Plane"),
			MB_OK);

		return 1;
	}

	// The parameters to CreateWindow explained:
	// szWindowClass: the name of the application
	// szTitle: the text that appears in the title bar
	// WS_OVERLAPPEDWINDOW: the type of window to create,~WS_THICKFRAME  fixed window size
	// CW_USEDEFAULT, CW_USEDEFAULT: initial position (x, y)
	// WNDWIDTH, WNDHEIGHT: initial size (width, length)
	// NULL: the parent of this window
	// NULL: this application does not have a menu bar
	// hInstance: the first parameter from WinMain
	// NULL: not used in this application
	hWnd = CreateWindow(
		szWindowClass,
		szTitle,
		WS_OVERLAPPEDWINDOW, //& ~WS_THICKFRAME,
		//CW_USEDEFAULT, CW_USEDEFAULT,
		20,20,
		O_WNDWIDTH + WNDWIDTH_PLUS, O_WNDHEIGHT + WNDHEIGHT_PLUS,
		NULL,
		NULL,
		hInstance,
		NULL
		);

	if (!hWnd)
	{
		MessageBox(NULL,
			_T("Call to CreateWindow failed!"),
			_T("Mega Plane"),
			MB_OK);

		return 1;
	}

	// The parameters to ShowWindow explained:
	// hWnd: the value returned from CreateWindow
	// nCmdShow: the fourth parameter from WinMain
	ShowWindow(hWnd,
		nCmdShow);
	UpdateWindow(hWnd);

	// Main message loop:
	while (GetMessage(&msg, NULL, 0, 0))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}

	return (int)msg.wParam;
}


LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	PAINTSTRUCT ps;
	HDC hdc;
	BITMAP bmp;
	POINT ptMouse;
	

	int i;

	switch (message)
	{
	case WM_CREATE:
		

		//����λͼ��Դ
		m_hBackgroundBmp = LoadBitmap(((LPCREATESTRUCT)lParam)->hInstance,
			MAKEINTRESOURCE(IDB_BACKGROUND));
		m_hHeroBmp = LoadBitmap(((LPCREATESTRUCT)lParam)->hInstance,
			MAKEINTRESOURCE(IDB_HERO));
		m_hHeroBulletBmp = LoadBitmap(((LPCREATESTRUCT)lParam)->hInstance,
			MAKEINTRESOURCE(IDB_HERO_BULLET));
		for (i = 0; i < ENEMY_TYPE_NUM; i++)
		{
			m_hEnemyBmp[i] = LoadBitmap(((LPCREATESTRUCT)lParam)->hInstance,
				MAKEINTRESOURCE(m_enemyBmpNames[i]));
		}
		m_hGameStatusBmp = LoadBitmap(((LPCREATESTRUCT)lParam)->hInstance,
			MAKEINTRESOURCE(IDB_GAME_STATUS));
		m_hToysBmp = LoadBitmap(((LPCREATESTRUCT)lParam)->hInstance,
			MAKEINTRESOURCE(IDB_TOYS));

		Initialize_Game();
		
		m_gameStatus.isPaused = FALSE;
		m_gameStatus.isStarting = TRUE;


		{
			//λͼ���
			GetObject(m_hGameStatusBmp, sizeof(BITMAP), &bmp);
			m_gameStatus.size.cx = bmp.bmWidth;
			m_gameStatus.size.cy = bmp.bmHeight / 2;
			m_gameStatus.hBmp = m_hGameStatusBmp;
		}

		srand((unsigned int)time(NULL));
		timer_i = 0;
		
		//��ʼ��������
		PlaySound(MAKEINTRESOURCE(IDR_WAVE1),GetModuleHandle(NULL),SND_LOOP|SND_ASYNC|SND_RESOURCE);

		break;
	case WM_SIZE:
		cxClient = LOWORD(lParam);
		cyClient = HIWORD(lParam);
		break;
	case WM_PAINT:
		
		hdc = BeginPaint(hWnd, &ps);
		//����
		Render(hdc);
		EndPaint(hWnd, &ps);
		break;
	case WM_KEYUP:
		switch (wParam)
		{
		case VK_LEFT:
		case VK_RIGHT:
		case VK_UP:
		case VK_DOWN:
			MOVE_HERO = 0;
			break;
		}
		break;
	case WM_KEYDOWN:
		switch (wParam)
		{
		case VK_LEFT:
		case VK_RIGHT:
		case VK_UP:
		case VK_DOWN:
			MOVE_HERO = wParam;
			break;

		case VK_SPACE:

			if(m_Planes[0].status == 0)
			{
				//Restart_Game
				Initialize_Game();
				
			}
			else
			{
				if(m_gameStatus.isStarting)
				{
					SetTimer(hWnd, TIMER, 50, NULL);
					m_gameStatus.isStarting = FALSE;
					InvalidateRect(hWnd, NULL, FALSE);
				}
				else 
					Pause_Game(hWnd);
			}
			break;
		case 'B':
			m_Toys[Toy_Count] = CreateToy(toyBLD);
			break;
		case 'D':
			m_Toys[Toy_Count] = CreateToy(toyDBL);
			break;
		case 'C':
			m_Toys[Toy_Count] = CreateToy(toyCLS);
			break;
		case 'R':
			m_Toys[Toy_Count] = CreateToy(toyRAD);
			break;
		case 'S':
			m_Toys[Toy_Count] = CreateToy(toySUB);
			break;
		case 'M'://����ģʽ�������ӵ���Чȫ��
			DBLBullet = TRUE;
			RADBullet = TRUE;
			SUBBullet = TRUE;
			break;
		default:
			break;
		}
		break;
	case WM_LBUTTONDOWN:
		ptMouse.x = LOWORD(lParam);
		ptMouse.y = HIWORD(lParam);

		//�������˼�������ͣͼ��
		if (Paused(ptMouse))
		{
			Pause_Game(hWnd);
		}
		break;
	case WM_TIMER:
		Run_Timer();
		InvalidateRect(hWnd, NULL, FALSE);
		break;
	case WM_DESTROY:
		PostQuitMessage(0);
		break;
	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
		break;
	}

	//������Դ��ռ���ڴ�
	//DeleteDC(hdc);
	return 0;
}

//�ɻ��ӵ��Ĵ�����ɾ��

//�����ɻ����������ͷɻ��Ĳ����ڴ����á�type���봴���ɻ������͡��л�����
Plane CreatePlane(LONG x, LONG y, int type)
{
	Plane plane;
	//λͼ��Դ��أ����޸�λͼ��Դ���˴�����Ӧ�޸�
	switch(type)
	{
	case planeHERO:
		plane.hBmp    = m_hHeroBmp;
		plane.size.cx = 102;
		plane.size.cy = 126;
		break;
	case 1:
		plane.hBmp    = m_hEnemyBmp[0];
		plane.size.cx = 34;
		plane.size.cy = 28;
		break;
	case 2:
		plane.hBmp    = m_hEnemyBmp[1];
		plane.size.cx = 70;
		plane.size.cy = 90;
		break;
	case 3:
		plane.hBmp    = m_hEnemyBmp[2];
		plane.size.cx = 108;
		plane.size.cy = 129;
		break;
	}


	//��Ϸ����
	switch(type)
	{
	case planeHERO:
		plane.speed   = 5;
		plane.full    = 5;
		plane.score   = 1;
		break;
	case 1:
		plane.speed   = 5;
		plane.full    = 1;
		plane.score   = 1;
		break;
	case 2:
		plane.speed   = 4;
		plane.full    = 3;
		plane.score   = 3;
		break;
	case 3:
		plane.speed   = 3;
		plane.full    = 6;
		plane.score   = 6;
		break;
	}
	switch(type)
	{
	case planeHERO://����Hero��ʱ���Դ����x,y
		plane.pos.x = WNDWIDTH /2 - plane.size.cx/2;
		plane.pos.y = WNDHEIGHT * 3 /4;
		break;
	default:
		plane.speed *= stage_mtplr(m_stage);
		y = -plane.size.cy;
		ConfirmNoLessThan(&x, -plane.size.cx );
		ConfirmNoMoreThan(&x, -plane.size.cx + WNDWIDTH);
		plane.pos.x = x;
		plane.pos.y = y;
		break;
	}
	plane.type = type;
	plane.status = plane.full;

	
	if (Plane_Count < PLANE_MAX_NUM) Plane_Count ++;
	plane.how = 0;
	return plane;
}
Toy CreateToy(int type)
{
	Toy toy;




	switch(type)
	{
	case toyDBL:
		
		break;
	case toyCLS:

		break;
	}
	toy.speed  = 10;
	toy.speed *= stage_mtplr(m_stage);
	toy.type = type;

	

	toy.hBmp = m_hToysBmp;
	toy.size.cx = Toy_BMP_sx;
	toy.size.cy = Toy_BMP_sy;
	toy.pos.x = GetRand(10, WNDWIDTH - toy.size.cx);
	toy.pos.y = 10;

	if (Toy_Count < TOY_MAX_NUM) Toy_Count ++;

	return toy;
}
VOID CreateBullet(Plane FatherPlane, int type, int special)//���һ������Ԥ��
{
	Bullet bullet;
	
	switch(type)
	{
	case 0:
		bullet.speed = -10;
		bullet.speed *= stage_mtplr(m_stage);
		bullet.pos.x = FatherPlane.pos.x + FatherPlane.size.cx / 2 - Bullet_BMP_sx / 2 + Bullet_BMP_sx * special;
		bullet.pos.y = FatherPlane.pos.y - Bullet_BMP_sy;
		break;
	case 1:
		bullet.speed = FatherPlane.speed * 2;
		bullet.pos.x = FatherPlane.pos.x + FatherPlane.size.cx / 2 - Bullet_BMP_sx / 2 + Bullet_BMP_sx * special;;
		bullet.pos.y = FatherPlane.pos.y + FatherPlane.size.cy ;
		break;
	}
	bullet.hBmp    = m_hHeroBulletBmp;
	bullet.size.cx = Bullet_BMP_sx;
	bullet.size.cy = Bullet_BMP_sy;

	bullet.vx = 0;
	if(special == 0)
		bullet.type  = type;
	else if(special == bullet_1_of_2|| special == bullet_2_of_2)
		bullet.type  = 2;
	else if(special == bullet_left)
		bullet.type  = 4, bullet.vx = -2.5;
	else if(special == bullet_right)
		bullet.type  = 3, bullet.vx = 2.5;
	else//SUB
	{
		bullet.type = 0;
		bullet.pos.y += 40;//����λͼ
	}

	if (Bullet_Count < BULLET_MAX_NUM) Bullet_Count ++;
	
	m_Bullets[Bullet_Count] = bullet;
}
VOID DestroyBullet(int iBullet)
{
	for(int i=iBullet+1; i<= Bullet_Count; i++)
	{
		m_Bullets[i-1] = m_Bullets[i];
	}
	Bullet_Count --;
}
VOID DestroyPlane(int iPlane)
{
	for(int i=iPlane+1; i<=Plane_Count; i++)
	{
		m_Planes[i-1] = m_Planes[i];
	}
	Plane_Count --;
}
VOID DestroyToy(int iToy)
{
	for(int i=iToy+1; i<=Toy_Count; i++)
	{
		m_Toys[i-1] = m_Toys[i];
	}
	Toy_Count --;
}

VOID Render(HDC hdc)
{
	TCHAR	szScore[50];
	HDC	hdcBmp, hdcBuffer;
	HBITMAP	cptBmp;
	int i, len;
	HFONT hFont; 

	cptBmp = CreateCompatibleBitmap(hdc, WNDWIDTH, WNDHEIGHT);
	hdcBmp = CreateCompatibleDC(hdc);
	hdcBuffer = CreateCompatibleDC(hdc);
	SelectObject(hdcBuffer, cptBmp);
	SelectObject(hdcBmp, m_hBackgroundBmp);
	
	//�ַ���ʾ׼��
	SetTextColor(hdcBuffer, WHITE);
	SetBkMode(hdcBuffer, TRANSPARENT);
	hFont = CreateFont
		(
		20, SEVEN_ZEROS, FONT_DEFAULT_PARAS,
		_T("΢���ź�")    //��������
		);
	SelectObject(hdcBuffer, hFont);


	if(m_gameStatus.isStarting)
	{		
		hFont = CreateFont
			(
			30, 0,    //�� ��
			30, 0,    //�ı���б ������б
			FW_HEAVY,    //����
			0,0,0,        //��б�壬���»��ߣ����л���
			FONT_DEFAULT_PARAS,
			_T("΢���ź�")    //��������
			);
		SelectObject(hdcBuffer, hFont);
		len = wsprintf(szScore, _T("��ӭ������λ�ɻ���ս��"));
		TextOut(hdcBuffer, 20, 200, szScore, len);

		hFont = CreateFont
			(
			20, SEVEN_ZEROS, FONT_DEFAULT_PARAS,
			_T("΢���ź�")    //��������
			);
		SelectObject(hdcBuffer, hFont);
		len = wsprintf(szScore, _T("�ո����ʼ����ͣ����������Ʒɻ���"));
		TextOut(hdcBuffer, 20, 260, szScore, len);
		goto Finishing;
	}


	//���Ʊ���������
	int ix, iy;
 	for(ix =0; ix < WNDWIDTH ; ix += BGWIDTH)
	{
		for(iy =0; iy < WNDHEIGHT ; iy += BGHEIGHT)
		{
			BitBlt(hdcBuffer, ix, iy - BGHEIGHT + backgound_i, BGWIDTH, BGHEIGHT,
				hdcBmp, 0, 0, SRCCOPY);
			BitBlt(hdcBuffer, ix, iy + backgound_i, BGWIDTH, BGHEIGHT,
				hdcBmp, 0, 0, SRCCOPY);
		}
	}

	//��ʾ����
	len = wsprintf(szScore, _T("SCORE: %d"), m_totalScore);
	TextOut(hdcBuffer, 30, 3, szScore, len);
	SetTextColor(hdcBuffer, GRAY(120));
	len = wsprintf(szScore, _T("STG: %02d"), m_stage);
	TextOut(hdcBuffer, 30, 20, szScore, len);
	SetTextColor(hdcBuffer, WHITE);

	//��Ϸ����
	if(m_Planes[0].status==0)
	{
		hFont = CreateFont
			(
			30, SEVEN_ZEROS,	FONT_DEFAULT_PARAS,
			_T("΢���ź�")    //��������
			);
		SelectObject(hdcBuffer, hFont);
		len = wsprintf(szScore, _T("�������У�"));
		SetTextColor(hdcBuffer, RED);
		TextOut(hdcBuffer, 20, 220, szScore, len);
		SetTextColor(hdcBuffer, WHITE);
		hFont = CreateFont
			(
			20, SEVEN_ZEROS, FONT_DEFAULT_PARAS,
			_T("΢���ź�")    //��������
			);
		SelectObject(hdcBuffer, hFont);
		len = wsprintf(szScore, _T("��Ϸ�������ؿ� %02d���÷� %d��"),m_stage, m_totalScore);
		TextOut(hdcBuffer, 20, 300, szScore, len);
		len = wsprintf(szScore, _T("���ո�����¿�ʼ��"));
		TextOut(hdcBuffer, 20, 330, szScore, len);
	}


	//���Ʒɻ�
	for (i = Plane_Count; i >= 0; i--)
	{
		SelectObject(hdcBmp, m_Planes[i].hBmp);
		TransparentBlt(
			hdcBuffer, m_Planes[i].pos.x, m_Planes[i].pos.y,
			m_Planes[i].size.cx, m_Planes[i].size.cy,
			hdcBmp, 0, m_Planes[i].size.cy * GetAniFromStatus(m_Planes[i].status), m_Planes[i].size.cx, m_Planes[i].size.cy,
			BLACK
			);
		//Ѫ��
		if(m_Planes[i].status >= - ANI_STEP)
		{
			int iblood;
			for(iblood=1; iblood<=m_Planes[i].status; iblood++)
				szScore[iblood-1]='1';
			for(; iblood<=m_Planes[i].full; iblood++)
				szScore[iblood-1]='0';
			szScore[iblood]=0;
			SetTextColor(hdcBuffer, GRAY(80));
			TextOut(hdcBuffer, m_Planes[i].pos.x, m_Planes[i].pos.y-20, szScore, m_Planes[i].full);
			SetTextColor(hdcBuffer, WHITE);
		}
		//�ӷ���Ϣ
		if(m_Planes[i].status <= 0 && m_Planes[i].status >= -2 *ANI_STEP && i != 0)
		{
			len = wsprintf(szScore, _T("+%d"),-m_Planes[i].score);
			SetTextColor(hdcBuffer, RGB(0,150,0));
			TextOut(hdcBuffer, m_Planes[i].pos.x + m_Planes[i].size.cx/2 - 8, m_Planes[i].pos.y -20, szScore, len);
			SetTextColor(hdcBuffer, WHITE);
		}

#ifdef DEBUGING
		{
			len = wsprintf(szScore, _T("id%d, bld%d"), i, m_Planes[i].status);
			TextOut(hdcBuffer, m_Planes[i].pos.x-10, m_Planes[i].pos.y-20, szScore, len);
		}
#endif		
	}

	//�����ӵ�
	for (i = 0; i <= Bullet_Count; i++)
	{
		SelectObject(hdcBmp, m_hHeroBulletBmp);
		TransparentBlt(
			hdcBuffer, m_Bullets[i].pos.x,  m_Bullets[i].pos.y,
			m_Bullets[i].size.cx, m_Bullets[i].size.cy,
			hdcBmp, 0, m_Bullets[i].size.cy * m_Bullets[i].type, m_Bullets[i].size.cx, m_Bullets[i].size.cy,
			BLACK
			);
#ifdef DEBUGING
		{
			len = wsprintf(szScore, _T("%d"), i);
			TextOut(hdcBuffer, m_Bullets[i].pos.x-5, m_Bullets[i].pos.y-5, szScore, len);
		}
#endif
	}


	//���Ƶ���
	for (i = 0; i <= Toy_Count; i++)
	{
		SelectObject(hdcBmp, m_hToysBmp);
		TransparentBlt(
			hdcBuffer, m_Toys[i].pos.x,  m_Toys[i].pos.y,
			m_Toys[i].size.cx, m_Toys[i].size.cy,
			hdcBmp, 0, m_Toys[i].size.cy * m_Toys[i].type, m_Toys[i].size.cx, m_Toys[i].size.cy,
			BLACK
			);
#ifdef DEBUGING
		{
			len = wsprintf(szScore, _T("%d"), i);
			TextOut(hdcBuffer, m_Toys[i].pos.x-5, m_Toys[i].pos.y-5, szScore, len);
		}
#endif
	}


	//����ͣ�����λͼ
	SelectObject(hdcBmp, m_gameStatus.hBmp);
	TransparentBlt(hdcBuffer, 0, 0, m_gameStatus.size.cx, m_gameStatus.size.cy,
		hdcBmp, 0, m_gameStatus.size.cy * m_gameStatus.isPaused,
		m_gameStatus.size.cx, m_gameStatus.size.cy, BLACK);
	
		
	//��ʾ������Ϣ
#ifdef DEBUGING

	{
		TCHAR	tempSTR[50];
		hFont = CreateFont
			(
			20, SEVEN_ZEROS, FONT_DEFAULT_PARAS,
			_T("΢���ź�")    //��������
			);
		SelectObject(hdcBuffer, hFont);
		len = wsprintfW(tempSTR, _T("timer_i: %d"), timer_i);
		TextOut(hdcBuffer, 30, 25 + 20, tempSTR, len);
		len = wsprintf(tempSTR, _T("Plane_Count: %d"), Plane_Count);
		TextOut(hdcBuffer, 30, 45 + 20, tempSTR, len);
		len = wsprintf(tempSTR, _T("Bullet_Count: %d"), Bullet_Count);
		TextOut(hdcBuffer, 30, 65 + 20, tempSTR, len);
		len = wsprintf(tempSTR, _T("Toy_Count: %d"), Toy_Count);
		TextOut(hdcBuffer, 30, 85 + 20, tempSTR, len);
	}
#endif
	
	Finishing:
	//������е���Ϣ���Ƶ���Ļ��
	BitBlt(hdc, 0, 0, WNDWIDTH, WNDHEIGHT, hdcBuffer, 0, 0, SRCCOPY);

	//������Դ��ռ���ڴ�
	DeleteObject(cptBmp);
	DeleteDC(hdcBuffer);
	DeleteDC(hdcBmp);
}

BOOL Paused(POINT ptMouse)
{
	RECT rPause;

	rPause.left = 0;
	rPause.top = 0;
	rPause.right = m_gameStatus.size.cx;
	rPause.bottom = m_gameStatus.size.cy;

	return PtInRect(&rPause, ptMouse);
}

VOID Pause_Game(HWND hWnd)
{
	if (!m_gameStatus.isPaused)
	{
		KillTimer(hWnd, TIMER);
		m_gameStatus.isPaused = TRUE;
	}
	else
	{
		SetTimer(hWnd, TIMER, 50, NULL);
		m_gameStatus.isPaused = FALSE;
	}
	InvalidateRect(hWnd, NULL, FALSE);
}
//�ж��ӵ��Ƿ���зɻ������жϽ���洢�ڽṹ���У���Ѫ
VOID Hit(int bullet_i, int plane_i)
{
	if(m_Planes[plane_i].status<=0)//����Ѿ������У�������������
	{
		return;
	}

	long x = m_Bullets[bullet_i].pos.x + (m_Bullets[bullet_i].size.cx)/2;
	long y = m_Bullets[bullet_i].pos.y;
	long top = m_Planes[plane_i].pos.y;
	long left = m_Planes[plane_i].pos.x;
	long right = left + m_Planes[plane_i].size.cx;
	long bottom = top + m_Planes[plane_i].size.cy;
	int bspeed = m_Bullets[bullet_i].speed;
	int bvx = m_Bullets[bullet_i].vx;
	int pspeed = m_Planes[plane_i].speed;
	int tempbool = 1;
	if(m_Bullets[bullet_i].type == bulletENEMY && plane_i == planeHERO)//���˵��ӵ����һ�
	{
		tempbool *= (x>=left && x<=right);
		tempbool *= (y+bspeed>=top && y-bspeed< bottom);
	}
	else if(m_Bullets[bullet_i].type != bulletENEMY && plane_i != planeHERO)//�ҵ��ӵ���л�
	{
		tempbool *= (x>=left && x<=right);
		tempbool *= (y-bspeed<=bottom);
	}
	else
	{
		return;
	}

	if(tempbool)//����
	{
		m_Bullets[bullet_i].type=-1;
		m_Planes[plane_i].status --;
	}

}

VOID HitHero(int plane_i)
{
	int tempbool=1;
	if(m_Planes[0].status<=0 || m_Planes[plane_i].status<=0)//����Ѿ������У�������������
	{
		return;
	}
	tempbool *= m_Planes[0].pos.x       < m_Planes[plane_i].pos.x + m_Planes[plane_i].size.cx - EPS;
	tempbool *= m_Planes[plane_i].pos.x < m_Planes[0].pos.x + m_Planes[0].size.cx - EPS;
	tempbool *= m_Planes[0].pos.y       < m_Planes[plane_i].pos.y + m_Planes[plane_i].size.cy - EPS;
	tempbool *= m_Planes[plane_i].pos.y < m_Planes[0].pos.y + m_Planes[0].size.cy - EPS;
	if(tempbool)//����
	{
		m_Planes[0].status = 0;
		m_Planes[plane_i].status = 0;
	}
}

VOID GetToy(int toy_i)
{
	int tempbool=1;
	if(m_Planes[0].status<=0)//���Hero�Ѿ������У�������������
	{
		return;
	}
	tempbool *= m_Planes[0].pos.x       < m_Toys[toy_i].pos.x + m_Toys[toy_i].size.cx - EPS_TOY;
	tempbool *= m_Toys[toy_i].pos.x < m_Planes[0].pos.x + m_Planes[0].size.cx - EPS_TOY;
	tempbool *= m_Planes[0].pos.y       < m_Toys[toy_i].pos.y + m_Toys[toy_i].size.cy - EPS_TOY;
	tempbool *= m_Toys[toy_i].pos.y < m_Planes[0].pos.y + m_Planes[0].size.cy - EPS_TOY;
	if(tempbool)//��������Ч����
	{
		switch(m_Toys[toy_i].type)
		{
		case toyCLS:
			//���ȫ���л�
			for (int ip = 1; ip <= Plane_Count; ip++)
				if(m_Planes[ip].status >= 0)
					m_Planes[ip].status = 0, m_Planes[ip].how = die_for_cls;
			break;
		case toyDBL:
			DBLBullet = TRUE;
			DBLBulletCDTimer = 0;
			break;
		case toyBLD:
			m_Planes[0].status = m_Planes[0].full;
			break;
		case toyRAD:
			RADBullet = TRUE;
			RADBulletCDTimer = 0;
			break;
		case toySUB: 
			SUBBullet = TRUE;
			SUBBulletCDTimer = 0;
			break;
		}
		m_Toys[toy_i].type = -1;
	}
}

VOID Run_Timer()
{
	//�ƶ�Hero
	if(m_gameStatus.isPaused == FALSE)
	{
		Move_Hero(MOVE_HERO);
	}

	int i;
	int ip, ib;
	timer_i ++;
	timer_i = timer_i % 48;
	backgound_i += 3;
	backgound_i %= BGHEIGHT;

	//�ж��һ��Ƿ񱻵л�����
	for (ip = 1; ip <= Plane_Count; ip++)
	{
		HitHero(ip);
	}


	//�ж��һ��Ƿ��ӵ�����
	if(m_Planes[0].status > 0)
	{

		for (ip = 0; ip <= Plane_Count; ip++)
		{
			for (ib = 0; ib <= Bullet_Count; ib++)
			{
				Hit(ib, ip);
			}
		}
	}


	//�ж��һ��Ƿ��õ���
	if(m_Planes[0].status > 0)
	{
		for (i = 0; i <= Toy_Count; i++)
		{
			GetToy(i);
		}
	}
	
	//����
	if(m_totalScore/20 >= m_stage)
	{
		m_stage++;
		m_Planes[Plane_Count] = CreatePlane(GetRand(10, WNDWIDTH), 10 , 3);
	}

	//����һ���µл�
	if (timer_i % NEW_ENEMY==0)
	{
			m_Planes[Plane_Count] = CreatePlane(GetRand(10, WNDWIDTH), 10 , GetRand(1,2));
	}

	//�л�λ��
	for (i = 1; i <= Plane_Count; i++)
	{
		m_Planes[i].pos.y += m_Planes[i].speed;
		if(m_Planes[i].type>=3)//BOSS���������յ��ƶ�·�ߣ�
		{
			if(m_Planes[i].status>0)
				if(timer_i % 16 ==0)
				{
					m_Planes[i].pos.y += GetRand(-2*BOSS_STEP, BOSS_STEP);
				}
				if(timer_i % 16 ==8)
				{
					m_Planes[i].pos.x += GetRand(-BOSS_STEP, BOSS_STEP);
				}
		}
	
	}
	//�л������µ��ӵ�
	if(timer_i % NEW_ENEMY_BULLET  == 0)
		for (i = 1; i <= Plane_Count; i++)
		{
			if(m_Planes[i].status > 0)
				if(GetRand(1,8)==1)//�ƻ��л������ӵ���ͬ����
					CreateBullet(m_Planes[i], 1, 0);
		}
	//�л��ɳ���Ļ
	for (i = 1; i <= Plane_Count; i++)
	{
		if (m_Planes[i].pos.y > WNDHEIGHT && WNDHEIGHT /*��С��ʱWM_SIZE�߶���0*/) DestroyPlane(i);
	}



	//�ӵ�λ��
	for (i = 0; i <= Bullet_Count; i++)
	{
		if (m_Bullets[i].pos.y <=0) DestroyBullet(i);
		if (m_Bullets[i].pos.y >=WNDHEIGHT) DestroyBullet(i);
		m_Bullets[i].pos.y += m_Bullets[i].speed;
		m_Bullets[i].pos.x += m_Bullets[i].vx;
	}

	//����λ��
	for (i = 0; i <= Toy_Count; i++)
	{
		if (m_Toys[i].pos.y <=0) DestroyToy(i);
		if (m_Toys[i].pos.y >=WNDHEIGHT) DestroyToy(i);
		m_Toys[i].pos.y = (m_Toys[i].pos.y + m_Toys[i].speed);
	}


	//��������
	if(timer_i % 4 ==0)
	{
		int rnd = GetRand(1, 5);
		if(rnd <= toyMAX);
			
	}
	
	

	//�һ������µ��ӵ�
	if(timer_i % NEW_HERO_BULLET  == 0)
	{
		if(!RADBullet && ! DBLBullet)
			CreateBullet(m_Planes[0], bulletHERO, 0);

		if(RADBullet)
		{
			CreateBullet(m_Planes[0], bulletHERO, bullet_left);
			CreateBullet(m_Planes[0], bulletHERO, bullet_right);
			CreateBullet(m_Planes[0], bulletHERO, 0);
			//��ʱ��	
			RADBulletCDTimer ++;
			if(RADBulletCDTimer > DBLBulletCDTime)
			{
				RADBullet = FALSE;
				RADBulletCDTimer = 0;
			}
		}
		if(DBLBullet)
		{
			CreateBullet(m_Planes[0], bulletHERO, bullet_2_of_2);
			CreateBullet(m_Planes[0], bulletHERO, bullet_1_of_2);
			//��ʱ��	
			DBLBulletCDTimer ++;
			if(DBLBulletCDTimer > DBLBulletCDTime)
			{
				DBLBullet = FALSE;
				DBLBulletCDTimer = 0;
			}
		}

		if(SUBBullet)
		{
			CreateBullet(m_Planes[0], bulletHERO, bullet_SUB_2);
			CreateBullet(m_Planes[0], bulletHERO, bullet_SUB_3);
			//��ʱ��	
			SUBBulletCDTimer ++;
			if(SUBBulletCDTimer > SUBBulletCDTime)
			{
				SUBBullet = FALSE;
				SUBBulletCDTimer = 0;
			}
		}

		
	}

	

	//�ӷ�
	for (ip = 1; ip <= Plane_Count; ip++)
	{
		if(m_Planes[ip].status == 0 && m_Planes[ip].score > 0 )
		{
			m_totalScore += m_Planes[ip].score;
			m_Planes[ip].score *= -1;
		}
	}

	//�����Ч�ķɻ�
	for (ip =1; ip <= Plane_Count; ip++)
	{
		if(m_Planes[ip].status <= -3 * ANI_STEP)
		{
			//�ڴ�ɻ������ʱ��������
			if(m_Planes[ip].type > 1 && m_Planes[ip].how !=die_for_cls)
			{
				
				//��Hero��ûѪ��ʱ���Ը���ĸ��ʲ��� toyBLD
				if(m_Planes[0].status == 1 && GetRand(0, 2)==0)
				{
					m_Toys[Toy_Count] = CreateToy(toyBLD);
				}
				else
				{
					m_Toys[Toy_Count] = CreateToy(GetRand(0, toyMAX));
				}
			}
			DestroyPlane(ip);
		}
	}
	

	//�����Ч���ӵ�
	for (ib = 0; ib <= Bullet_Count; ib++)
	{
		if(m_Bullets[ib].type==-1)	
		{
			DestroyBullet(ib);
		}
	}
	//�����Ч�ĵ���
	for (i = 0; i <= Toy_Count; i++)
	{
		if(m_Toys[i].type==-1)	
		{
			DestroyToy(i);
		}
	}

	//������֡
	{
		for (ip = 1; ip <= Plane_Count; ip++)
		{
			if(m_Planes[ip].status<=0)//����Ѿ�������
			{
				m_Planes[ip].status --;
			}
		}
	}

}

VOID Move_Hero(WPARAM key)
{
	long temp;
	switch(key)
	{

			case VK_LEFT:
				temp = m_Planes[0].pos.x - m_Planes[0].speed * STEP;
				ConfirmNoLessThan(&temp, -m_Planes[0].size.cx/2);
				m_Planes[0].pos.x = temp;
				break;
			case VK_RIGHT:
				temp = m_Planes[0].pos.x + m_Planes[0].speed * STEP;
				ConfirmNoMoreThan(&temp, -m_Planes[0].size.cx/2 + WNDWIDTH);
				m_Planes[0].pos.x = temp;
				break;
			case VK_UP:
				temp = m_Planes[0].pos.y - m_Planes[0].speed * STEP;
				ConfirmNoLessThan(&temp, 0);
				m_Planes[0].pos.y = temp;
				break;
			case VK_DOWN:
				temp = m_Planes[0].pos.y + m_Planes[0].speed * STEP;
				ConfirmNoMoreThan(&temp, WNDHEIGHT - m_Planes[0].size.cy/2);
				m_Planes[0].pos.y = temp;
				break;
	}
}

VOID Initialize_Game()
{
	m_Planes[0] = CreatePlane(NULL , NULL , planeHERO);
	Plane_Count = 0;
	Toy_Count = 0;
	Bullet_Count = 0;		
	m_totalScore = 0;
	m_stage = 1;
}