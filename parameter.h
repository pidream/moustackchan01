//ゴール座標
#define GOAL_X	3				//○ゴール座標(x)
#define GOAL_Y	3				//○ゴール座標(y)
#define ONEONE				//1x1ゴール設定

//速度・距離
#define ST_ACCL 1.0 //直線加速度 1=1m/ss
#define ST_DE_ACCL 1.5 //直線加速度 1=1m/ss
#define SEARCH_SPEED 400 // m/s
#define SECTION 358 //180mm=355
#define FIRST_HALF_SECTION 124
#define SECOND_HALF_SECTION 234 //96+21.41
//#define HALF_SECTION 177 //90mm
#define OSHIRI 124 //62.59mm 尻あてから迷路中心まで
#define D15MM 30
#define D30MM 60
#define D06MM 12
#define D75MM 149

//センサ 位置の基準
//横壁
#define REF_SEN_R	320			//マウスを迷路中央に置いた時の横センサの値
#define REF_SEN_L	320			//マウスを迷路中央に置いた時の横センサの値
#define KABE_TR_TH 15

//前壁
#define REF_SEN_FR	280			//マウスを尻あて時の前センサの値
#define REF_SEN_FL	280			//マウスを尻あて時の前センサの値

#define END_SEN_FR	327			//マウスを迷路中央に置いた時の前センサの値	45mm
#define END_SEN_FL	327			//マウスを迷路中央に置いた時の前センサの値

#define REF_SEN_FT	(REF_SEN_FR+REF_SEN_FL)	//柱位置
#define END_SEN_FT	(END_SEN_FR+END_SEN_FL)	//迷路中央



//センサ 壁の有無
#define TH_SEN_L 200
#define TH_SEN_R 200
#define TH_SEN_FL 200
#define TH_SEN_FR 200

//重みづけ
#define WEIGHT_STRA 1
#define WEIGHT_SLAN 5
#define WEIGHT_TURN 5

//迷路関連パラメータ
#define MAZESIZE_X	(16)		//迷路の大きさ(MAZESIZE_X * MAZESIZE_Y)迷路
#define MAZESIZE_Y	(16)		//迷路の大きさ(MAZESIZE_X * MAZESIZE_Y)迷路

#define MASK_SEARCH	0x01		//探索走行用マスク値.壁情報とこの値のAND値が０（NOWALL）なら壁なしor未探索区間
#define MASK_SECOND	0x03		//最短走行用マスク値.壁情報とこの値のAND値が０（NOWALL）なら壁なし
#define UNKNOWN	2			//10 壁があるかないか判らない状態の場合の値
#define NOWALL	0			//00 壁がないばあいの値
#define WALL	1			//01 壁がある場合の値
#define VWALL	3			//11 仮想壁の値
#define FAST_SEACH 1
#define EXACT_SEACH 0

//IO
#define MOTOR_EN 0
#define STEP_B 13
#define STEP_A 27
#define DIR_L 19
#define DIR_R 14
#define CH_SEL 25
#define IR_LED 26

#define EEPROM_SIZE 256

//マクロ
#define MM2LENGTH(s)	((short)(s)*(2) )	//距離(mm)からパルス数に変更
#define CONV_SEN2WALL(w) ((w) ? WALL : NOWALL)			//センサ情報から壁情報へ変換