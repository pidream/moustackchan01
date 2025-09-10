#include "EEPROM.h"
#include <M5Unified.h> // M5Unifiedライブラリをプログラムで使用可能にします。
#include <M5GFX.h>
#include "mytypedef.h"
#include "parameter.h"

M5GFX lcd;
M5Canvas canvas(&lcd);


// グローバル変数（プログラム全体で使用する変数の定義をします。）
uint32_t ref_step = 0;
uint16_t max_speed=600;
t_sensor			sen_r, sen_l, sen_fr, sen_fl;	//センサ構造体


t_position		mypos;		//自己座標
t_wall			wall[MAZESIZE_X+1][MAZESIZE_Y+1];	//壁の情報を格納する構造体配列
//t_wall			wall_for_rom[MAZESIZE_X+1][MAZESIZE_Y+1];	//壁の情報を格納する構造体配列
direction_flug		now_direction[MAZESIZE_X+1][MAZESIZE_Y+1];	
unsigned short		 step_map[MAZESIZE_X+1][MAZESIZE_Y+1];	//歩数マップ
t_map_flug		map_flug[MAZESIZE_X+1][MAZESIZE_Y+1];
unsigned short map_stack_tail;
t_map_stack map_stack[2048];//0x800 0x00 ~ 0x7ff
char zentansaku=0;
bool flug_run_err=0;
short end_f_sensor;

//　割り込み内でアクセスするもの
volatile u_char  mode_case = 0;
volatile uint16_t l_front,r_front,l_side,r_side;
volatile uint16_t line_l,line_fl,line_fr,line_r;
volatile uint32_t cnt0 = 0;
volatile uint32_t cnt2 = 0;
volatile uint32_t left_cnt = 0;
volatile uint32_t right_cnt = 0;

volatile uint32_t total_step = 0;
volatile uint32_t diff_step = 0;
volatile uint32_t  ref_deff_step;

volatile double now_speed;
volatile double tar_speed=0;
volatile double st_accl=1;

volatile char runmode=0;
volatile double left_speed=0;
volatile double right_speed=0;

  double tmp_end_speed;
  bool break_flug=0;
  short tar_step;
  short stepsUsedForAcceleration;



// setup関数は起動時に一度だけ実行されます。
// 主に初期化処理を記述します。
void setup() {

  initAll();

}

// loop関数は起動している間ずっと繰り返し実行されます。
// センサーから値を取得したり、画面を書き換える動作等をおこないます。
void loop() {

  signed char run_mode = 0;
  bool state;
  signed char axpButton;
  
  now_speed = 0;

  cnt0=0;
  while(1){

    M5.update();

    //axpButton = 
    state =  M5.BtnA.wasPressed();
    if(state == true || M5.BtnPWR.wasClicked()){run_mode--;if(run_mode<0){run_mode=4;}canvas.fillScreen(BLACK);lcd.fillScreen(BLACK);}
    state =  M5.BtnC.wasPressed();
    if(state == true ){run_mode++;if(run_mode>4){run_mode=0;}canvas.fillScreen(BLACK);lcd.fillScreen(BLACK);}
    state =  M5.BtnB.wasPressed();
    if(state == true ||  M5.BtnPWR.wasHold()){canvas.fillScreen(BLACK);lcd.fillScreen(BLACK);break;}

  

    //M5.Power.setLed(128); //内臓LED（緑色）
    if(cnt0>200){
      cnt0=0;
    
    switch(run_mode){
      case 0:
        //左上が0
        canvas.fillCircle(53, 80, 7, WHITE);
        canvas.fillCircle(173, 80, 7, WHITE);
        canvas.fillRect(73, 140, 80, 5, WHITE);
        //canvas.pushSprite(&lcd, 7, 50);
        canvas.pushSprite(&lcd, random(0,14), 45+random(0,10) );

        break;
      case 1:
        //lcd.fillScreen(BLACK);
        lcd.setCursor(0, 0);              // 文字の描画座標（カーソル位置）を設定
        lcd.printf("%d\n",run_mode);
        lcd.printf("LEFT  F: %4d,  \n", line_fl);  // 画面に表示
        lcd.printf("RIGHT F: %4d,  \n", line_fr);  // 画面に表示
        lcd.printf("LEFT  S: %4d,  \n", line_l);   // 画面に表示
        lcd.printf("RIGHT S: %4d,  \n", line_r);   // 画面に表示
        lcd.printf("V_BAT: %3d,  \n",M5.Power.getBatteryLevel());
        break;

      case 2:
        lcd.setCursor(0, 0);              // 文字の描画座標（カーソル位置）を設定
        lcd.printf("%d show map\n",run_mode);
        load_map();
        view_map();

        
        break;

      case 3:
        lcd.setCursor(0, 0);              // 文字の描画座標（カーソル位置）を設定
        lcd.printf("%d Solve the maze\n",run_mode);
        lcd.printf("goal x= %d\n",GOAL_X);
        lcd.printf("goal y= %d\n",GOAL_Y);

        
        break;

      case 4:
        lcd.setCursor(0, 0);              // 文字の描画座標（カーソル位置）を設定
        lcd.printf("%d fast run\n",run_mode);
        lcd.printf("goal x= %d\n",GOAL_X);
        lcd.printf("goal y= %d\n",GOAL_Y);
        break;


      default:
        lcd.setCursor(50, 0);              // 文字の描画座標（カーソル位置）を設定
        lcd.printf("%d\n",run_mode);
        break;
    }//switch


    }
    
  }//while
  
  lcd.setCursor(0, 0);              // 文字の描画座標（カーソル位置）を設定
  lcd.printf("wait... \n");
  delay(500);
  lcd.fillScreen(BLACK);
  canvas.fillCircle(53, 80, 7, WHITE);
  canvas.fillCircle(173, 80, 7, WHITE);
  canvas.fillRect(73, 140, 80, 5, WHITE);
      
  switch(run_mode){
    case 0:
      lcd.fillScreen(BLACK);
      lcd.printf("loadmap \n");
      load_map();
      delay(500);
      break;

    case 1:
    case 2:
      digitalWrite(MOTOR_EN, 1);  // nsleepをHIGHでモータ励磁
      
      while(line_fr<280){
        canvas.pushSprite(&lcd, random(0,14), 45+random(0,10) );
        delay(30); 
      }
      lcd.fillScreen(BLACK);
      canvas.pushSprite(&lcd, 7, 50 );
      delay(1000);
      cnt0=0;

      max_speed=600;//600mm/s

      ref_step=total_step;//距離0点
      straight_for_search(0,SEARCH_SPEED); //壁尻当てから迷路中心までの距離
      straight_for_search(OSHIRI,SEARCH_SPEED); //壁尻当てから迷路中心までの距離
      ref_step=total_step;//距離0点
      straight_for_search(SECOND_HALF_SECTION + D06MM, 200);

      turn(right,1);

      straight_for_search(D75MM + FIRST_HALF_SECTION, 0);

      digitalWrite(MOTOR_EN, 0);  // nsleepをHIGHでモータ励磁

      break;

    case 3://探索
      
      digitalWrite(MOTOR_EN, 1);  // nsleepをHIGHでモータ励磁


      while(line_fr<280){
        canvas.pushSprite(&lcd, random(0,14), 45+random(0,10) );
        delay(30); 
      }

      C3:

      lcd.fillScreen(BLACK);
      canvas.pushSprite(&lcd, 7, 50 );
      delay(1000);
      cnt0=0;

      max_speed=600;

  		ref_step=total_step;//距離0点
			straight_for_search(0,SEARCH_SPEED); //壁尻当てから迷路中心までの距離
      straight_for_search(OSHIRI,SEARCH_SPEED); //壁尻当てから迷路中心までの距離
			ref_step=total_step;//距離0点

      search_adachi(GOAL_X, GOAL_Y);

      lcd.setCursor(0, 0); 
      lcd.fillScreen(BLACK);
      lcd.printf("goal \n");
      lcd.printf("V_BAT: %3d,  \n",M5.Power.getBatteryLevel());
      if(flug_run_err==1){lcd.printf("flug_run_err \n");}

      lcd.printf("savemap \n");
      save_map();

      delay(3000);
      lcd.fillScreen(BLACK);
      search_adachi(0, 0);


      lcd.fillScreen(BLACK);
      lcd.setCursor(0, 0); 
      lcd.printf("goal \n");
      lcd.printf("V_BAT: %3d,  \n",M5.Power.getBatteryLevel());
      if(flug_run_err==1){lcd.printf("flug_run_err \n");}
      lcd.printf("savemap \n");
      save_map();

      delay(3000);
      lcd.fillScreen(BLACK);


      rotate(1 , 2);//右回り 180度
      mypos.dir = north;
      digitalWrite(MOTOR_EN, 0);  // nsleepをHIGHでモータ励磁

      view_map();
      
      goto C5;

      while(1){;}
      break;

    case 4://最短
      load_map();
      
      fast_calc(GOAL_X, GOAL_Y);
      
      digitalWrite(MOTOR_EN, 1);  // nsleepをHIGHでモータ励磁


      while(line_fr<280){
        canvas.pushSprite(&lcd, random(0,14), 45+random(0,10) );
        delay(30); 
      }
      lcd.fillScreen(BLACK);
      canvas.pushSprite(&lcd, 7, 50 );
      delay(1000);
      cnt0=0;

      max_speed=600;

      fast_run(GOAL_X, GOAL_Y);

      digitalWrite(MOTOR_EN, 0);  // nsleepをHIGHでモータ励磁
    
      lcd.setCursor(0, 0); 
      lcd.fillScreen(BLACK);
      lcd.printf("goal \n");
      lcd.printf("V_BAT: %3d,  \n",M5.Power.getBatteryLevel());
      if(flug_run_err==1){lcd.printf("flug_run_err \n");}

      while(1){;}
      break;

    case 5:
      C5:
      delay(3000);

      digitalWrite(MOTOR_EN, 1);  // nsleepをHIGHでモータ励磁
      left_speed=-100;
      right_speed=-100;
      delay(1000);

      digitalWrite(MOTOR_EN, 0);  // nsleepをHIGHでモータ励磁
      left_speed=0;
      right_speed=0;
      delay(3000);

      lcd.fillScreen(BLACK);
      canvas.fillScreen(BLACK);
      canvas.fillCircle(53, 80, 7, WHITE);
      canvas.fillCircle(173, 80, 7, WHITE);
      canvas.fillRect(73, 140, 80, 5, WHITE);
      canvas.pushSprite(&lcd, random(0,14), 45+random(0,10) );
      delay(100);
      digitalWrite(MOTOR_EN, 1);  // nsleepをHIGHでモータ励磁
      goto C3;


      while(1){;}
      break;


  }//switch


  digitalWrite(MOTOR_EN, 0);  // nsleepをHIGHでモータ励磁

  run_mode=0;
  lcd.fillScreen(BLACK);
}



