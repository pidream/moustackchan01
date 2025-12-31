typedef struct
{
	unsigned char north:2;	//北の壁情報
	unsigned char east:2;	//東の壁情報
	unsigned char south:2;	//南の壁情報
	unsigned char west:2;	//西の壁情報
}t_wall;			//壁情報を格納する構造体(ビットフィールド)

typedef struct
{
	unsigned char know:2;	
	unsigned char goal:2;	
	unsigned char ignore:2;	
	unsigned char rsv_b:2;	
}t_map_flug;	//座標のフラグ管理

typedef enum
{
	north=0,
	east=1,
	south=2,
	west=3,
}t_direction;

typedef struct
{
	short x;
	short y;
	t_direction dir;
}t_position;

typedef struct {
	unsigned char x;
	unsigned char y;
}t_map_stack;

typedef struct  {
	unsigned char h:1;
	unsigned char g:1;
	unsigned char w:1;
	unsigned char s:1;
	unsigned char e:1;
	unsigned char n:1;
	unsigned char ns:1;
	unsigned char ew:1;
} direction_flug;

typedef struct
{
	//short value;		//現在の値
	//short p_value;		//１mS過去の値
	//short error;		//value - ref
	//short ref;		//リファレンス値
	//short th_wall;		//壁があるか否かの閾値
	//short th_control;	//制御をかけるか否かの閾値
	bool is_wall;		//壁があるか無いか ( true = 壁あり false = 壁なし )
	//t_bool is_control;	//制御に使うか否か
}t_sensor;	

typedef enum
{
	front=0,		//前
	right=1,		//右
	rear=2,			//後
	left=3,			//左
	unknown,		//方向不明
}t_local_dir;	//自分から見た方向を示す列挙型

typedef struct
{
	unsigned short step:6;		//step
	unsigned short ter_speed:10;	//目標速度
	
	unsigned char now_speed_T:8; 	//目標速度との速度差
	unsigned char now_speed_D:8;	//左右速度差
	
	unsigned char ter_gyro:8;	//目標角速度
	unsigned char now_gyro:8;	//現在の角速度
	
	unsigned char sensor_l:8;	//
	unsigned char sensor_fl:8;	//
	
	unsigned char sensor_fr:8;	//
	unsigned char sensor_r:8;	//
	
	unsigned short batt:7;	//
	unsigned short resve:9;	//
}t_log;			//LOG構造体