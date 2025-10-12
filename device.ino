const unsigned char log_table[101]={
  0,
  0, 30, 48, 60, 70, 78, 85, 90, 95,100,
104,108,111,115,118,120,123,126,128,130,
132,134,136,138,140,141,143,145,146,148,
149,151,152,153,154,156,157,158,159,160,
161,162,163,164,165,166,167,168,169,170,
171,172,172,173,174,175,176,176,177,178,
179,179,180,181,181,182,183,183,184,185,
185,186,186,187,188,188,189,189,190,190,
191,191,192,192,193,193,194,194,195,195,
196,196,197,197,198,198,199,199,200,200
};

volatile short wall_trace_speed=0;
volatile short wall_trace_step=0;
volatile short left_set,right_set,left_set_buf,right_set_buf;
volatile uint32_t right_step = 0;
volatile uint32_t left_step = 0;
short r_side_black,l_side_black,r_front_black,l_front_black;

hw_timer_t *g_timer0 = NULL;
portMUX_TYPE timerMux = portMUX_INITIALIZER_UNLOCKED;
void IRAM_ATTR onTimer() {  //0.2ms周期
  //noInterrupts(); // 排他制御開始
  portENTER_CRITICAL_ISR(&timerMux);
  left_cnt++;
  if(left_cnt==left_set-1){
    digitalWrite(STEP_A,1);
    left_step++;
  }else if(left_cnt>=left_set){
    left_cnt=0;
    digitalWrite(STEP_A,0);
  }
  if(left_cnt<left_set-1){left_set=left_set_buf;}

  right_cnt++;
  if(right_cnt==right_set-1){
    digitalWrite(STEP_B,1);
    right_step++;
  }else if(right_cnt>=right_set){
    right_cnt=0;
    digitalWrite(STEP_B,0);
  }
  if(right_cnt<right_set-1){right_set=right_set_buf;}

  total_step = right_step+left_step;
  diff_step = right_step - left_step;

  mode_case++;
  if(mode_case>4){mode_case=0;}


  switch(mode_case){
    case 0:
      digitalWrite(CH_SEL, 0);//CH_SEL 0=左前 右横
      break;

    case 1:
      r_side_black = analogRead(36);
      l_front_black = analogRead(35);
      digitalWrite(IR_LED, 1);//IR_LED:ON
      delayMicroseconds(10);//10us待ち
      r_side = r_side_black-analogRead(36);
      l_front = l_front_black-analogRead(35);
      
      //analogReadMilliVolts()
      digitalWrite(IR_LED, 0);//IR_LED:OFF
      break;

    case 2:
      digitalWrite(CH_SEL, 1);//CH_SEL 1=右前 左横
      break;

    case 3:
      l_side_black = analogRead(36);
      r_front_black = analogRead(35);
      digitalWrite(IR_LED, 1);//IR_LED:ON
      delayMicroseconds(10);//10us待ち
      l_side = l_side_black-analogRead(36);
      r_front = r_front_black-analogRead(35);
      digitalWrite(IR_LED, 0);//IR_LED:OFF
      break;
  }
  

  if(mode_case == 4){// ///////////////////////////////// ここから 1ms毎に実行
    cnt0++;

	//max - 4096(2^12)
	if(r_side<100){
		line_r =log_table[r_side];
	}else if(r_side<198){
		line_r =log_table[r_side/2]+30;
	}else if(r_side<394){
		line_r =log_table[r_side/4]+60;	
	}else if(r_side<899){
		line_r =log_table[r_side/9]+95;	
	}else if(r_side<1973){
		line_r =log_table[r_side/20]+130;		
	}else{
		line_r =log_table[r_side/46]+166;	
	}
  

  if(r_front<100){
		line_fr =log_table[r_front];
	}else if(r_front<198){
		line_fr =log_table[r_front/2]+30;
	}else if(r_front<394){
		line_fr =log_table[r_front/4]+60;	
	}else if(r_front<899){
		line_fr =log_table[r_front/9]+95;	
	}else if(r_front<1973){
		line_fr =log_table[r_front/20]+130;		
	}else{
		line_fr =log_table[r_front/46]+166;	
	}
  //line_fr=line_fr+10;
  
  if(l_front<100){
		line_fl =log_table[l_front];
	}else if(l_front<198){
		line_fl =log_table[l_front/2]+30;
	}else if(l_front<394){
		line_fl =log_table[l_front/4]+60;	
	}else if(l_front<899){
		line_fl =log_table[l_front/9]+95;	
	}else if(l_front<1973){
		line_fl =log_table[l_front/20]+130;		
	}else{
		line_fl =log_table[l_front/46]+166;	
	}
  line_fl=(double)line_fl*0.84+70;

  if(l_side<100){
		line_l =log_table[l_side];
	}else if(l_side<198){
		line_l =log_table[l_side/2]+30;
	}else if(l_side<394){
		line_l =log_table[r_side/4]+60;	
	}else if(l_side<899){
		line_l =log_table[l_side/9]+95;	
	}else if(l_side<1973){
		line_l =log_table[l_side/20]+130;		
	}else{
		line_l =log_table[l_side/46]+166;	
	}


	//壁有無判定|| (line_l>TH_SEN_L-30||line_r>cal_cen_l+30)|| (line_r>TH_SEN_R-30||line_l>cal_cen_l+30)
	if(line_l > TH_SEN_L ){sen_l.is_wall  = true;}else{sen_l.is_wall  = false;}
	if(line_r > TH_SEN_R ){sen_r.is_wall  = true;}else{sen_r.is_wall  = false;}	
	if(line_fl > TH_SEN_FL && line_fr > (TH_SEN_FR-20)){sen_fl.is_wall = true;}else{sen_fl.is_wall = false;}
 	if(line_fr > TH_SEN_FR && line_fl > (TH_SEN_FL-20)){sen_fr.is_wall = true;}else{sen_fr.is_wall = false;}



    //加速度計算
    if(st_accl>0 && now_speed<tar_speed){
      now_speed=now_speed+st_accl;
      if(now_speed>=tar_speed){now_speed=tar_speed;st_accl=0;}
    }
    if(st_accl<0 && now_speed>tar_speed){
      now_speed=now_speed+st_accl;
      if(now_speed<=tar_speed){now_speed=tar_speed;st_accl=0;}
    }


    //壁トレース
    wall_trace_speed=0;
    //wall_trace_step=0;
    switch(runmode){
      case 1:
        if(line_fl>REF_SEN_FL || line_fr>REF_SEN_FR){//前壁に近い
          if(line_fl>line_fr+5){wall_trace_speed=-10;}//左側が前にある→左回転
          if(line_fr>line_fl+5){wall_trace_speed= 10;}//右側が前にある→右回転  

        }else{
          if(line_l>REF_SEN_L	+KABE_TR_TH){wall_trace_speed= 5;}//左壁に近い→右回転
          if(line_r>REF_SEN_R	+KABE_TR_TH){wall_trace_speed=-5;}//右壁に近い→左回転
        }

        //モータへの出力設定
        left_speed =now_speed + wall_trace_speed;
        right_speed=now_speed - wall_trace_speed;
        break;

      case 2://超信地旋回
        //モータへの出力設定を直接指示している
        break;

      case 3://左ターン
        right_speed=now_speed;
        left_speed =400-right_speed;
        break;

      case 4://右ターン
        left_speed=now_speed;
        right_speed =400-left_speed;
        break;

      default://静止
        //モータへの出力設定
        left_speed =0;
        right_speed=0;
        break;

    }


    //モータ速度→ステップ周期に変換
    if(left_speed > 0.1){
      digitalWrite(DIR_L, 0);  //0で前進
      left_set_buf = 5023.275753/left_speed;

    }else if(left_speed < -0.1){
      digitalWrite(DIR_L, 1);  //1で後進
      left_set_buf = 5023.275753/(-left_speed);

    }else{
      left_set = 0xfff0;
      left_set_buf = 0xfff0;
      left_cnt =0;
      digitalWrite(STEP_A,0);
    }

    if(right_speed>0.1){
      digitalWrite(DIR_R, 0);  //0で前進
      right_set_buf= 5023.275753/right_speed;

    }else if(right_speed<-0.1){
      digitalWrite(DIR_R, 1);  //1で後進
      right_set_buf= 5023.275753/(-right_speed);

    }else{
      right_set=0xfff0;
      right_set_buf = 0xfff0;
      right_cnt=0;
      digitalWrite(STEP_B,0);
    }

    //直進時のトレースはパルス周期を直接変化させる(パルス周期を伸ばす→回転速度が遅くなる)
    if(left_set_buf==right_set_buf && wall_trace_speed!=0){
      if(wall_trace_speed>0){//右回転したい
        right_set_buf = right_set_buf + 1;//右の回転を遅くする 
      }else{
        left_set_buf = left_set_buf + 1;
      }
    }


  }

  portEXIT_CRITICAL_ISR(&timerMux);
  //interrupts(); // 排他制御終了
}


void initAll(void)
{
  auto cfg = M5.config();       // M5Stack初期設定用の構造体を代入
  // configを設定する場合はここで設定
  cfg.output_power = false;
  cfg.internal_imu = false;
  cfg.internal_rtc = false;
  cfg.internal_spk = false;
  cfg.internal_mic = false;
  M5.begin(cfg);                 // M5デバイスの初期化

  M5.Power.begin(); //バッテリ電圧監視に必要

  //IO設定
  pinMode(STEP_B,OUTPUT);//STEP_B
  pinMode(DIR_R,OUTPUT);//DIR_R
  pinMode(DIR_L,OUTPUT);//DIR_L
  pinMode(STEP_A,OUTPUT);//STEP_A
  pinMode(MOTOR_EN,OUTPUT);//nsleep
  pinMode(CH_SEL,OUTPUT);//CH_SEL
  pinMode(IR_LED,OUTPUT);//LED_ON
  pinMode(35, ANALOG);//右前or左前
  pinMode(36, ANALOG);//右横or左前
  
  digitalWrite(MOTOR_EN, LOW);  // nsleepをHIGHでモータ励磁
  digitalWrite(DIR_L, 0);  //0で前進
  digitalWrite(DIR_R, 0);  //0で前進

  digitalWrite(IR_LED, 0);//LED_ON

  //ステッピングモータの初期パルス周期
  //回転しないように最大値にしておく
  left_set =0xfff0;
  right_set=0xfff0;


  //portENTER_CRITICAL(&timerMux);
  // スプライトの作成
  lcd.begin();
  lcd.fillScreen(TFT_BLACK);
  canvas.setColorDepth(8);
  canvas.createSprite(226, 226);
  //portEXIT_CRITICAL(&timerMux);

  lcd.setRotation(0);
  lcd.setTextSize(2);               // テキストサイズを変更
  lcd.setTextFont(2);
  //M5.Lcd.print("Hello World!!");       // 画面にHello World!!と1行表示

  

  Serial.println("Hello M5stack!!");         // シリアルモニターにHello World!!と1行表示

  

  if (!EEPROM.begin(EEPROM_SIZE))
  {
    Serial.println("failed to initialise EEPROM"); delay(1000000);
  }
  #if 0
  Serial.println(" bytes read from Flash . Values are:");
  delay(100);
  for (int i = 0; i < EEPROM_SIZE; i++)
  {
    Serial.print(byte(EEPROM.read(i))); Serial.print(" ");
    if(i%16==15){Serial.println();delay(100);}
  }
  Serial.println();
  //Serial.println("writing random n. in memory");
  #endif

  init_maze();

  // タイマ作成
  g_timer0 = timerBegin(1000000); // 1MHzカウント
  // タイマ割り込みサービス・ルーチン onTimer を登録
  timerAttachInterrupt(g_timer0, &onTimer);
  // 割り込みタイミング(ms)の設定
  timerAlarm(g_timer0, 200, true,0); //1MHz*200 = 0.2ms毎の呼びだし
  // タイマ有効化
  timerStart(g_timer0);

}