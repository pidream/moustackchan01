#include "EEPROM.h"
#include <M5Unified.h> // M5Unifiedライブラリをプログラムで使用可能にします。
#include <M5GFX.h>
#include "mytypedef.h"
#include "parameter.h"
#include "glob_var.h"

M5GFX lcd;
M5Canvas canvas(&lcd);


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
  
  now_speed = 0;

  cnt0=0;
  while(1){
    //delay(1);
    M5.update();

    state =  M5.BtnA.wasPressed();
    if(state == true || M5.BtnPWR.wasClicked()){run_mode--;if(run_mode<0){run_mode=5;}lcd.fillScreen(BLACK);}
    state =  M5.BtnC.wasPressed();
    if(state == true ){run_mode++;if(run_mode>5){run_mode=0;}lcd.fillScreen(BLACK);}
    state =  M5.BtnB.wasPressed();
    if(state == true ||  M5.BtnPWR.wasHold()){canvas.fillScreen(BLACK);break;}

    

    //M5.Power.setLed(128); //内臓LED（緑色）
    if(cnt0>200){
      cnt0=0;
    
    switch(run_mode){
      case 0:
        //左上が0
        canvas.fillScreen(BLACK);
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

      case 5:
        lcd.setCursor(0, 0);              // 文字の描画座標（カーソル位置）を設定
        lcd.printf("%d slam run\n",run_mode);
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

    case 1://テスト走行
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

      max_speed=350;//600mm/s

      ref_step=total_step;//距離0点
      straight_for_search(0,SEARCH_SPEED); //ダミーの直進処理。何故か最初の直進が無視されるため
      straight_for_search(OSHIRI,SEARCH_SPEED); //壁尻当てから迷路中心までの距離
      ref_step=total_step;//距離0点
      straight_for_search(SECOND_HALF_SECTION + D33MM, 0);//06+27=33
      digitalWrite(MOTOR_EN, 0);  // nsleepをHIGHでモータ励磁
      delay(10);
      lcd.printf("LEFT  F: %4d,  \n", line_fl);  // 画面に表示
      lcd.printf("RIGHT F: %4d,  \n", line_fr);  // 画面に表示
      

      while(1){;}

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
			straight_for_search(0,SEARCH_SPEED); //ダミーの直進処理。何故か最初の直進が無視されるため
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
      
      goto C99;

      while(1){;}
      break;

    case 4://最短
      load_map();
      
      fast_calc(GOAL_X, GOAL_Y);
      
      digitalWrite(MOTOR_EN, 1);  // nsleepをHIGHでモータ励磁


      while(line_fr<280){
        canvas.pushSprite(&lcd, random(0,14), 45+random(0,10) );
        delay(30); 
        fotce_sr_en = false;
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

    case 5://最短
      load_map();
      
      fast_calc(GOAL_X, GOAL_Y);
      
      digitalWrite(MOTOR_EN, 1);  // nsleepをHIGHでモータ励磁


      while(line_fr<280){
        canvas.pushSprite(&lcd, random(0,14), 45+random(0,10) );
        delay(30); 
        fotce_sr_en = true;
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




    case 99://オードスタート処置、重ね探索を繰り返す
      C99:
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



