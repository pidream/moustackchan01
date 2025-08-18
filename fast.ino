unsigned char run_pat[200];

void fast_calc(char x, char y)
{
//引数の座標x,yに向かって最短走行する

	t_direction glob_nextdir;
	short straight_count=0;
	char harf_sect=0;
  short runsect=0;
    

	harf_sect=1; //スタート直後のフラグ
	make_map(x,y,MASK_SECOND,EXACT_SEACH);		//重みづけ歩数Map生成
  if( step_map[0][0] == 0xffff){lcd.printf("mapbreak \n");delay(3000);return;}
	//lcd.printf("x:%d,y:%d,w:%d \n",mypos.x,mypos.y,step_map[mypos.x][mypos.y]);

	while(step_map[mypos.x][mypos.y]!=0){
  
		switch(get_nextdir(MASK_SECOND,&glob_nextdir))	//次に行く方向を戻り値とする関数を呼ぶ
		{
			case front:				//直線をまとめて走るようにする
				straight_count++;
				break;
			
			case right:
				if(straight_count>=2 || harf_sect==1 ){
					run_pat[runsect]=(straight_count-1)*2+harf_sect;
					harf_sect=0;
					runsect++;
				}

				run_pat[runsect]=70;
				runsect++;
				
				
				straight_count = 1;//走る直線の距離をリセット
				break;
			
			case left:
				if(straight_count>=2 || harf_sect==1 ){
					run_pat[runsect]=(straight_count-1)*2+harf_sect;
					harf_sect=0;
					runsect++;
				}
				
				run_pat[runsect]=80;
				runsect++;
				
				straight_count = 1;//走る直線の距離をリセット
				break;
			
			case rear:
				break;
		}
	
		mypos.dir = glob_nextdir;	//自分の向きを修正
		
		//向いた方向によって自分の座標を更新する
		switch(mypos.dir)
		{
			case north:
				mypos.y++;	//北を向いた時はY座標を増やす
				break;
				
			case east:
				mypos.x++;	//東を向いた時はX座標を増やす
				break;
				
			case south:
				mypos.y--;	//南を向いた時はY座標を減らす
				break;
			
			case west:
				mypos.x--;	//西を向いたときはX座標を減らす
				break;

		}

    //lcd.printf("x:%d,y:%d,w:%d \n",mypos.x,mypos.y,step_map[mypos.x][mypos.y]);
	}

	//ゴールで1歩進む補正 ここから
	switch(mypos.dir)
	{
		case north:
			mypos.y++;	//北を向いた時はY座標を増やす
			break;
			
		case east:
			mypos.x++;	//東を向いた時はX座標を増やす
			break;
			
		case south:
			mypos.y--;	//南を向いた時はY座標を減らす
			break;
		
		case west:
			mypos.x--;	//西を向いたときはX座標を減らす
			break;
	}
	//ここまで
	
	 
	//最後の直線消化
  run_pat[runsect]=(straight_count-1)*2;
	
	#ifndef ONEONE
		run_pat[runsect]+=2;//ゴールで進む補正
	#endif
	runsect++;
	
	//止める
	run_pat[runsect]=255;
	
  
}

/*
	fast_run
	引数はターン速度
	1:50cm/s
	2:
	3:
*/
void fast_run(char x, char y)
{
	short runsect=0;
	unsigned tempnow=0;
  signed char tmp_ofset_flug = 0;
  
  

	ref_step = total_step;//距離0
	
	while(1){
	switch(run_pat[runsect]){
		case 70://右ターン
        #if 0
        straight_for_search(D06MM, 200);
        if(line_l>REF_SEN_L	+15){tmp_ofset_flug=-1;}//左壁に近い→右にずれる
        turn(right,1);
        if(tmp_ofset_flug==1){ref_step=total_step + D15MM;}
        delay(100);
        end_f_sensor = REF_SEN_FT;
        straight_for_search(D75MM, 200);
        ref_step = total_step;//距離0点
        tmp_ofset_flug=0;
        #else

          straight_for_search(FIRST_HALF_SECTION,0);		//半区画進んで
          if(line_l>REF_SEN_L	+15){tmp_ofset_flug=-1;}//左壁に近い→右にずれる
					rotate(right,1);					//右に曲がって
					//end_f_sensor =END_SEN_FT;
          if(tmp_ofset_flug==1){ref_step=total_step + D15MM;}
					straight_for_search(SECOND_HALF_SECTION,SEARCH_SPEED);		//半区画進む
					ref_step = total_step;//距離0点
          tmp_ofset_flug=0;
				#endif

			break;
			
		case 80://左ターン
        #if 0
        straight_for_search(D06MM, 200);
        if(line_r>REF_SEN_R	+15){tmp_ofset_flug=1;}//右壁に近い→左にずれる
        turn(left,1);
        if(tmp_ofset_flug==1){ref_step=total_step + D15MM;}
        delay(100);
        end_f_sensor = REF_SEN_FT;
        straight_for_search(D75MM, 200);
        ref_step = total_step;//距離0点
        tmp_ofset_flug=0;
        #else

          straight_for_search(FIRST_HALF_SECTION,0);		//半区画進んで
          if(line_r>REF_SEN_R	+15){tmp_ofset_flug=1;}//右壁に近い→左にずれる
					rotate(left,1);					//右に曲がって
					//end_f_sensor =END_SEN_FT;
          if(tmp_ofset_flug==1){ref_step=total_step + D15MM;}
					straight_for_search(SECOND_HALF_SECTION,SEARCH_SPEED);		//半区画進む
					ref_step = total_step;//距離0点
          tmp_ofset_flug=0;
				#endif

			break;
					
		case 254://スキップ
			break;
			
		case 255://停止
			return;//関数を抜ける
			break;



		default://直線
			tempnow=run_pat[runsect];
			while(run_pat[runsect+1]==254){runsect++;}
			
				if(tempnow%2){//スタート直後の距離補正
					turn(left,0);
          straight(0,SEARCH_SPEED); //ダミー
          straight(SECOND_HALF_SECTION+OSHIRI,SEARCH_SPEED);
          ref_step = total_step;//距離0
        }
					
				switch(run_pat[runsect+1]){
					case 255://次はゴール
						end_f_sensor = END_SEN_FT;
						straight( (tempnow/2)*SECTION + FIRST_HALF_SECTION, 0);
						break;

					default://次はターン
						end_f_sensor = REF_SEN_FT;
						straight((tempnow/2)*SECTION,SEARCH_SPEED);
            tmp_ofset_flug=0;
						break;
				}//switch

			break;

	}//switch
	runsect++;
	}//while

}
