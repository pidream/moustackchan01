void straight_for_search(long len, double end_speed)	//直線走行、探索用にリセットは外部に任せる
{
  //short set_pulse_period;


  runmode=1;

  if(now_speed<50){
    //canvas.fillCircle(53, 80, 7, WHITE);
    //canvas.fillCircle(173, 80, 7, WHITE);
    //canvas.fillRect(73, 140, 80, 5, WHITE);
    //canvas.setPivot(canvas.width()/2.0, canvas.height()/2.0);
    canvas.pushRotateZoom(120, 130, 0, 1.0, 1.0);//画面上に描写を書き出す
  }
  
  if(end_speed<0.1){tmp_end_speed=70;}else{tmp_end_speed = end_speed;}


  if(now_speed<0.1){
    tar_speed=100;
    now_speed=100;
    delay(100);
  }

  noInterrupts(); // 排他制御開始
  tar_speed=SEARCH_SPEED;// mm/s
  st_accl=ST_ACCL;
  interrupts(); // 排他制御終了

  while(ref_step>total_step){}

  while(now_speed<end_speed || len-(total_step-ref_step)-D30MM > (((now_speed*now_speed)-(end_speed*end_speed))/(2000.0*ST_DE_ACCL))*1.971467682 ){//59=30mm
    if( (total_step-ref_step) > len){break;}
    if(line_fl + line_fr >= end_f_sensor){break;}

    delay(1);
  }

  noInterrupts(); // 排他制御開始
  tar_speed=tmp_end_speed;
  st_accl=-ST_DE_ACCL;
  interrupts(); // 排他制御終了

  if(end_speed<now_speed && end_speed<50){
    canvas.fillRect(73, 140, 80, 5, BLACK);
    canvas.fillRect(88, 130, 50, 25, WHITE);
    canvas.pushRotateZoom(120, 130, 0, 1.0, 1.0);//画面上に描写を書き出す
    break_flug=1;
  }

  while((total_step-ref_step)  < len){
    if(line_fl + line_fr >= end_f_sensor){tar_speed = end_speed;break;}//前壁で距離あわせ
    delay(1);
  }

  #if 1
	if((line_fl + line_fr) < end_f_sensor && (line_fl + line_fr) > (end_f_sensor-40) && (sen_fr.is_wall || sen_fl.is_wall) ){//前壁があったら
		while(1){
			//if(speed_tgt <= r_tar_speed){st_accl = 0;}
			if(line_fl + line_fr >= end_f_sensor){st_accl =0; tar_speed = end_speed;break;}//前壁で距離あわせ
			//if(cnt1>1000){break;}
      delay(1);
		}
	}
  #endif

  if(end_speed<0.1){
    st_accl=0;
    tar_speed=0;
    now_speed=0;
    runmode=0;
  }
  
  end_f_sensor = END_SEN_FT;//区画中心

  if(break_flug==1){//メモリ上に口を書き直す
    canvas.fillRect(88, 130, 50, 25, BLACK);
    canvas.fillRect(73, 140, 80, 5, WHITE);
    //canvas.pushRotateZoom(120, 180, 0, 1.0, 1.0);
  }

}

void straight(long len, double end_speed)
{


  runmode=1;

  if(now_speed<50){
    //canvas.fillCircle(53, 80, 7, WHITE);
    //canvas.fillCircle(173, 80, 7, WHITE);
    //canvas.fillRect(73, 140, 80, 5, WHITE);
    //canvas.setPivot(canvas.width()/2.0, canvas.height()/2.0);
    canvas.pushRotateZoom(120, 130, 0, 1.0, 1.0);
  }
  
  if(end_speed<0.1){tmp_end_speed=70;}else{tmp_end_speed = end_speed;}


  if(now_speed<0.1){
    tar_speed=100;
    now_speed=100;
    delay(100);
  }

  noInterrupts(); // 排他制御開始
  tar_speed=max_speed;// mm/s
  st_accl=ST_ACCL;
  interrupts(); // 排他制御終了


  while(now_speed<end_speed || len-(total_step-ref_step)-D30MM > (((now_speed*now_speed)-(end_speed*end_speed))/(2000.0*ST_DE_ACCL))*1.971467682){//59=30mm
    if( (total_step-ref_step) > len){break;}
    if(line_fl + line_fr >= end_f_sensor){break;}

    delay(1);
  }

  noInterrupts(); // 排他制御開始
  tar_speed=tmp_end_speed;
  st_accl=-ST_DE_ACCL;
  interrupts(); // 排他制御終了

  if(end_speed<now_speed && end_speed<50){
    canvas.fillRect(73, 140, 80, 5, BLACK);
    canvas.fillRect(88, 130, 50, 25, WHITE);
    canvas.pushRotateZoom(120, 130, 0, 1.0, 1.0);
    break_flug=1;
  }

  while((total_step-ref_step)  < len){
    if(line_fl + line_fr >= end_f_sensor){tar_speed = end_speed;break;}//前壁で距離あわせ
    delay(1);
  }

  #if 1
	if((line_fl + line_fr) < end_f_sensor && (line_fl + line_fr) > (end_f_sensor-20) && (sen_fr.is_wall || sen_fl.is_wall) ){//前壁があったら
		while(1){
			//if(speed_tgt <= r_tar_speed){st_accl = 0;}
			if(line_fl + line_fr >= end_f_sensor){st_accl =0; tar_speed = end_speed;break;}//前壁で距離あわせ
			//if(cnt1>1000){break;}
      delay(1);
		}
	}
  #endif

  if(end_speed<0.1){
    st_accl=0;
    tar_speed=0;
    now_speed=0;
    runmode=0;
  }
  
  end_f_sensor = END_SEN_FT;

  if(break_flug==1){
    canvas.fillRect(88, 130, 50, 25, BLACK);
    canvas.fillRect(73, 140, 80, 5, WHITE);
    //canvas.pushRotateZoom(120, 180, 0, 1.0, 1.0);
  }
  ref_step=total_step;
}

/* 
  超信地旋回
*/
void rotate(char dir ,char times)
{
  

  runmode=2;
  st_accl=0;
  tar_speed=0;
  now_speed=0;
  left_speed =0;
  right_speed=0;

  delay(100);

  tar_step = 72*times;


  canvas.fillCircle(53, 80, 7, WHITE);
  canvas.fillCircle(173, 80, 7, WHITE);
  canvas.fillRect(73, 140, 80, 5, WHITE);
  canvas.setPivot(canvas.width()/2.0, canvas.height()/2.0);
  if(dir==left){
    canvas.pushRotateZoom(120, 160, -5, 1.0, 1.0);
    delay(50);
    canvas.pushRotateZoom(120, 160, -20, 1.0, 1.0);

    left_speed =-100;
    right_speed=100;

  }else{
    canvas.pushRotateZoom(120, 160, 5, 1.0, 1.0);
    delay(50);
    canvas.pushRotateZoom(120, 160, 20, 1.0, 1.0);

    left_speed =100;
    right_speed=-100;

  }
  
  ref_step=total_step;
  while(total_step-ref_step < tar_step){continue;}

  left_speed =0;
  right_speed=0;

  //delay(200);
  if(dir==left){
    canvas.pushRotateZoom(120, 160, -5, 1.0, 1.0);
    delay(100);
    canvas.pushRotateZoom(120, 160,  0, 1.0, 1.0);
  }else{
    canvas.pushRotateZoom(120, 160, 5, 1.0, 1.0);
    delay(100);
    canvas.pushRotateZoom(120, 160,  0, 1.0, 1.0);
  }

  ref_step=total_step;
}

/*
  スラローム旋回
*/
void turn(char dir,char times)
{

  st_accl=0;
  tar_step = 72;//90°分のステップ差  

  if(times==0){return;}
  ref_deff_step = diff_step;
  if(dir==left){
    runmode=3;
    noInterrupts(); // 排他制御開始
    tar_speed=300;
    st_accl=1;
    interrupts(); // 排他制御終了

    while( now_speed<tar_speed){continue;}

    stepsUsedForAcceleration = (diff_step)-ref_deff_step;//加速に使ったステップ数を記録
    while( (diff_step)< (ref_deff_step + (tar_step - stepsUsedForAcceleration))){continue;}

    noInterrupts(); // 排他制御開始
    tar_speed=205;
    st_accl=-1;
    interrupts(); // 排他制御終了

    while( (diff_step)< (ref_deff_step + tar_step)){continue;}

  }else{
    runmode=4;
    noInterrupts(); // 排他制御開始
    tar_speed=300;
    st_accl=1;
    interrupts(); // 排他制御終了

    while( now_speed<tar_speed){continue;}

    stepsUsedForAcceleration = (diff_step)-ref_deff_step;//加速に使ったステップ数を記録
    while( (diff_step)> (ref_deff_step - (tar_step + stepsUsedForAcceleration) )){continue;}

    noInterrupts(); // 排他制御開始
    tar_speed=205;
    st_accl=-1;
    interrupts(); // 排他制御終了

    while((diff_step)> (ref_deff_step - tar_step)){continue;}

  }

  runmode=1;
  noInterrupts(); // 排他制御開始
  st_accl=0;
  tar_speed=200;
  left_speed =200;
  right_speed=200;
  interrupts(); // 排他制御終了

  ref_step=total_step;
}