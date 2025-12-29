void init_maze(void)	//迷路情報の初期化
{
	char i,j;
	
	for(i = 0; i < MAZESIZE_X; i++)
	{
		for(j = 0; j < MAZESIZE_Y; j++)
		{
			wall[i][j].north = wall[i][j].east = wall[i][j].south = wall[i][j].west = UNKNOWN;	//迷路の全体がわからない事を設定する
		}
	}
	
	for(i = 0; i < MAZESIZE_X; i++)
	{
		wall[i][0].south = WALL;		//四方の壁を追加する(南)
		wall[i][MAZESIZE_Y-1].north = WALL;	//四方の壁を追加する(北)
		
	}
	
	for(j = 0; j < MAZESIZE_Y; j++)
	{
		wall[0][j].west = WALL;			//四方の壁を追加する(西)
		wall[MAZESIZE_X-1][j].east = WALL;	//四方の壁を追加する(東)
	}
	
	wall[0][0].east  = WALL; //スタート地点の壁を追加する
	wall[1][0].west = WALL;	 //スタート地点の壁を追加する
}



void init_map(char x, char y)
{
//迷路の歩数Mapを初期化する。全体を0xffff、引数の座標x,yは0で初期化する

	unsigned char i,j;
	
	if(zentansaku==0){ //通常探索
		map_stack_tail=0;
		for(i = 0; i < MAZESIZE_X; i++)		//迷路の大きさ分ループ(x座標)
		{
			for(j = 0; j < MAZESIZE_Y; j++)	//迷路の大きさ分ループ(y座標)
			{
				 step_map[i][j] = 0xffff;	//すべて0xffffで埋める
				now_direction[i][j].ew=0;
				now_direction[i][j].ns=0;
				now_direction[i][j].n =0;
				now_direction[i][j].s =0;
				now_direction[i][j].e =0;
				now_direction[i][j].w =0;
			}
		}
		
		
		 step_map[x][y] = 0;			//ゴール座標の歩数を０に設定
		
		map_stack[0].x = x;		//ゴール座標をスタックの最初に入れる
		map_stack[0].y = y;		//ゴール座標をスタックの最初に入れる
		map_stack_tail=1;
		
		#ifndef ONEONE
		if(!(x==0&&y==0)){  
			//ゴール座標の歩数を０に設定
			 step_map[x+1][y  ] = 0;
			 step_map[x  ][y+1] = 0;
			 step_map[x+1][y+1] = 0;
			//ゴール座標をスタックに入れる
			map_stack[1].x = x+1;map_stack[1].y = y;  
			map_stack[2].x = x  ;map_stack[2].y = y+1;
			map_stack[3].x = x+1;map_stack[3].y = y+1;
			map_stack_tail+=3;
		}
		#endif
  }

	now_direction[x][y].ew=1;
	now_direction[x][y].ns=1;
	now_direction[x][y].n =1;
	now_direction[x][y].s =1;
	now_direction[x][y].e =1;
	now_direction[x][y].w =1;
}

void make_map(char x, char y, unsigned char mask ,char fast_check)	//歩数マップを作成する
{
	//座標x,yをゴールとした歩数Mapを作成する。
	//maskの値(MASK_SEARCH or MASK_SECOND)によって、
	//探索用の歩数Mapを作るか、最短走行の歩数Mapを作るかが切り替わる
	//fast が 1(FAST_SEACH)なら歩数ベース 
	//        0(EXACT_SEACH)なら重みづけ最適化

	short i, j;
	unsigned short head = 0;

	unsigned char changeflu = 0;
	
	init_map(x, y);			//Mapを初期化する


	if(fast_check==FAST_SEACH){//探索中の時間のかからない処理、自分の足元まできたら探索うちきり
		while (head != map_stack_tail) { //head = tail なら更新する区画がない
			i = map_stack[head].x;//チェックする座標を呼び出す
			j = map_stack[head].y;
			if( step_map[i][j] == 0xffff){return;}
			head++;
			head&=0x07ff;

			if (j < MAZESIZE_Y - 1){				//範囲チェック
				if ((wall[i][j].north & mask) == NOWALL) {	//北の壁がなければ(maskの意味はstatic_parametersを参照)
					if ( step_map[i][j + 1] > ( step_map[i][j] + 1)) {	//まだ値が入っていなければ
						 step_map[i][j + 1] =  step_map[i][j] + 1;	//値を代入
						map_stack[map_stack_tail].x = i;
						map_stack[map_stack_tail].y = j + 1;
						map_stack_tail++;
						map_stack_tail&=0x07ff;//最大値制限 
					}
				}
			}

			if (i < MAZESIZE_X - 1){				//範囲チェック
				if ((wall[i][j].east & mask) == NOWALL) {	//東 壁がなければ
					if ( step_map[i + 1][j] > ( step_map[i][j] + 1)) {	//値が入っていなければ
						 step_map[i + 1][j] =  step_map[i][j] + 1;	//値を代入
						map_stack[map_stack_tail].x = i + 1;
						map_stack[map_stack_tail].y = j;
						map_stack_tail++;
						map_stack_tail&=0x07ff;//最大値制限 
					}
				}
			}

			if (j > 0){						//範囲チェック
				if ((wall[i][j].south & mask) == NOWALL) {	//南の壁がなければ
					if ( step_map[i][j - 1] > ( step_map[i][j] + 1)) {	//値が入っていなければ
						 step_map[i][j - 1] =  step_map[i][j] + 1;	//値を代入
						map_stack[map_stack_tail].x = i;
						map_stack[map_stack_tail].y = j - 1;
						map_stack_tail++;
						map_stack_tail&=0x07ff;//最大値制限 
					}
				}
			}

			if (i > 0){						//範囲チェック
				if ((wall[i][j].west & mask) == NOWALL) {	//西の壁がなければ
					if ( step_map[i - 1][j] > ( step_map[i][j] + 1)) {	//値が入っていなければ
						 step_map[i - 1][j] =  step_map[i][j] + 1;	//値を代入
						map_stack[map_stack_tail].x = i - 1;
						map_stack[map_stack_tail].y = j;
						map_stack_tail++;
						map_stack_tail&=0x07ff;//最大値制限 
					}
				}

			}
			//printf("%d \n", tail);
		

			
			
			if(mypos.x == i && mypos.y ==j){break;}//自分の位置までチェックし終えたら抜ける。
		
		}//whileの括弧閉じ
		
		
	}else{
		while (head != map_stack_tail) { //head = tail なら更新する区画がない
			i = map_stack[head].x;//チェックする座標を呼び出す
			j = map_stack[head].y;
			head++;
			head&=0x07ff;

			if (j < MAZESIZE_Y - 1)					//範囲チェック
			{
				if ((wall[i][j].north & mask) == NOWALL) {	//北の壁がなければ(maskの意味はstatic_parametersを参照)
					changeflu = 0;
					if ( step_map[i][j + 1] >  step_map[i][j]) { now_direction[i][j + 1].s = 1; }//一つ北の区間に南から来れるフラグを立てる
					if (now_direction[i][j].s == 1) { //現区画に南から来た
						if ( step_map[i][j + 1] > ( step_map[i][j] + WEIGHT_STRA)) {	//まだ値が入っていなければ
							 step_map[i][j + 1] =  step_map[i][j] + WEIGHT_STRA;	//値を代入
							map_stack[map_stack_tail].x = i;
							map_stack[map_stack_tail].y = j + 1;
							map_stack_tail++;
							map_stack_tail&=0x07ff;//最大値制限 
						}
					}
					else { //東西=1
						if(now_direction[i][j].e==1 && now_direction[i+1][j].s==1){ //東から来た
							if( step_map[i][j+1] > ( step_map[i][j] + WEIGHT_SLAN)){	//まだ値が入っていなければ
								 step_map[i][j+1] =  step_map[i][j] + WEIGHT_SLAN;	//値を代入
								map_stack[map_stack_tail].x = i;
								map_stack[map_stack_tail].y = j + 1;
								map_stack_tail++;
								map_stack_tail&=0x07ff;//最大値制限 
								changeflu = 1;
							}
							
						}
						if(now_direction[i][j].w==1 && now_direction[i-1][j].s==1){ //西から来た
							if( step_map[i][j+1] > ( step_map[i][j] + WEIGHT_SLAN)){	//まだ値が入っていなければ
								 step_map[i][j+1] =  step_map[i][j] + WEIGHT_SLAN;	//値を代入
								if(changeflu==0){
									map_stack[map_stack_tail].x = i;
									map_stack[map_stack_tail].y = j + 1;
									map_stack_tail++;
									map_stack_tail&=0x07ff;//最大値制限 
									changeflu = 1;
								}
							}
							
						}
						if (changeflu == 0 &&  step_map[i][j + 1] > ( step_map[i][j] + WEIGHT_TURN)) {	//まだ値が入っていなければ
							 step_map[i][j + 1] =  step_map[i][j] + WEIGHT_TURN;	//値を代入
							map_stack[map_stack_tail].x = i;
							map_stack[map_stack_tail].y = j + 1;
							map_stack_tail++;
							map_stack_tail&=0x07ff;//最大値制限 
						}
					}
				}
			}

			if (i < MAZESIZE_X - 1)					//範囲チェック
			{
				if ((wall[i][j].east & mask) == NOWALL) {	//東 壁がなければ
					changeflu = 0;
					if ( step_map[i + 1][j] >  step_map[i][j]) { now_direction[i + 1][j].w = 1; }//一つ東の区間に西から来れるフラグを立てる
					if (now_direction[i][j].w == 1) { //現区間に西から来れる場合
						if ( step_map[i + 1][j] > ( step_map[i][j] + WEIGHT_STRA)) {		//値が入っていなければ
							 step_map[i + 1][j] =  step_map[i][j] + WEIGHT_STRA;		//値を代入
							map_stack[map_stack_tail].x = i + 1;
							map_stack[map_stack_tail].y = j;
							map_stack_tail++;
							map_stack_tail&=0x07ff;//最大値制限 
						}
					}
					else { //北南=2
						if(now_direction[i][j].n==1 && now_direction[i][j+1].w==1){ //北から来た
							if( step_map[i+1][j] > ( step_map[i][j] + WEIGHT_SLAN)){	//まだ値が入っていなければ
								 step_map[i+1][j] =  step_map[i][j] + WEIGHT_SLAN;	//値を代入
								map_stack[map_stack_tail].x = i + 1;
								map_stack[map_stack_tail].y = j;
								map_stack_tail++;
								map_stack_tail&=0x07ff;//最大値制限 
								changeflu = 1;
							}
						}
						if(now_direction[i][j].s==1 && now_direction[i][j-1].w==1){ //南から来た
							if( step_map[i+1][j] > ( step_map[i][j] + WEIGHT_SLAN)){	//まだ値が入っていなければ
								 step_map[i+1][j] =  step_map[i][j] + WEIGHT_SLAN;	//値を代入
								if(changeflu == 0){
									map_stack[map_stack_tail].x = i + 1;
									map_stack[map_stack_tail].y = j;
									map_stack_tail++;
									map_stack_tail&=0x07ff;//最大値制限 
									changeflu = 1;
								}
							}
						}
						if (changeflu == 0 &&  step_map[i + 1][j] > ( step_map[i][j] + WEIGHT_TURN)) {		//値が入っていなければ
							 step_map[i + 1][j] =  step_map[i][j] + WEIGHT_TURN;		//値を代入
							map_stack[map_stack_tail].x = i + 1;
							map_stack[map_stack_tail].y = j;
							map_stack_tail++;
							map_stack_tail&=0x07ff;//最大値制限 
						}
					}
				}
			}

			if (j > 0)						//範囲チェック
			{
				if ((wall[i][j].south & mask) == NOWALL) {	//南の壁がなければ
					changeflu = 0;
					if ( step_map[i][j - 1] >  step_map[i][j]) { now_direction[i][j - 1].n = 1; }
					if (now_direction[i][j].n == 1) { //現区間に北から来れる場合
						if ( step_map[i][j - 1] > ( step_map[i][j] + WEIGHT_STRA)) {		//値が入っていなければ
							 step_map[i][j - 1] =  step_map[i][j] + WEIGHT_STRA;		//値を代入
							map_stack[map_stack_tail].x = i;
							map_stack[map_stack_tail].y = j - 1;
							map_stack_tail++;
							map_stack_tail&=0x07ff;//最大値制限 
						}
					}
					else { //東西=1
						if(now_direction[i][j].e==1 && now_direction[i+1][j].n==1){ //東から来た
							if( step_map[i][j-1] > ( step_map[i][j] + WEIGHT_SLAN)){	//まだ値が入っていなければ
								 step_map[i][j-1] =  step_map[i][j] + WEIGHT_SLAN;	//値を代入
								map_stack[map_stack_tail].x = i;
								map_stack[map_stack_tail].y = j - 1;
								map_stack_tail++;
								map_stack_tail&=0x07ff;//最大値制限 
								changeflu = 1;
							}
						}
						if(now_direction[i][j].w==1 && now_direction[i-1][j].n==1){ //西から来た
							if( step_map[i][j-1] > ( step_map[i][j] + WEIGHT_SLAN)){	//まだ値が入っていなければ
								 step_map[i][j-1] =  step_map[i][j] + WEIGHT_SLAN;	//値を代入
								if(changeflu ==0){
									map_stack[map_stack_tail].x = i;
									map_stack[map_stack_tail].y = j - 1;
									map_stack_tail++;
									map_stack_tail&=0x07ff;//最大値制限 
									changeflu = 1;
								}
							}
						}
						if (changeflu == 0 &&  step_map[i][j - 1] > ( step_map[i][j] + WEIGHT_TURN)) {		//値が入っていなければ
							 step_map[i][j - 1] =  step_map[i][j] + WEIGHT_TURN;		//値を代入
							map_stack[map_stack_tail].x = i;
							map_stack[map_stack_tail].y = j - 1;
							map_stack_tail++;
							map_stack_tail&=0x07ff;//最大値制限 
						}
					}
				}
			}

			if (i > 0)						//範囲チェック
			{
				if ((wall[i][j].west & mask) == NOWALL) {	//西の壁がなければ
					changeflu = 0;
					if ( step_map[i - 1][j] >  step_map[i][j]) { now_direction[i - 1][j].e = 1; }//一つ西の区間に東から来れるフラグを立てる
					if (now_direction[i][j].e == 1) { //現区間に東から来れる場合
						if ( step_map[i - 1][j] > ( step_map[i][j] + WEIGHT_STRA)) {		//値が入っていなければ
							 step_map[i - 1][j] =  step_map[i][j] + WEIGHT_STRA;		//値を代入
							map_stack[map_stack_tail].x = i - 1;
							map_stack[map_stack_tail].y = j;
							map_stack_tail++;
							map_stack_tail&=0x07ff;//最大値制限 
						}
					}
					else { //北南=2
						if(now_direction[i][j].n==1 && now_direction[i][j+1].e==1){ //北から来た
							if( step_map[i-1][j] > ( step_map[i][j] + WEIGHT_SLAN)){	//まだ値が入っていなければ
								 step_map[i-1][j] =  step_map[i][j] + WEIGHT_SLAN;	//値を代入
								map_stack[map_stack_tail].x = i - 1;
								map_stack[map_stack_tail].y = j;
								map_stack_tail++;
								map_stack_tail&=0x07ff;//最大値制限 
								changeflu = 1;
							}
						}
						if(now_direction[i][j].s==1 && now_direction[i][j-1].e==1){ //南から来た
							if( step_map[i-1][j] > ( step_map[i][j] + WEIGHT_SLAN)){	//まだ値が入っていなければ
								 step_map[i-1][j] =  step_map[i][j] + WEIGHT_SLAN;	//値を代入
								if(changeflu == 0){
									map_stack[map_stack_tail].x = i - 1;
									map_stack[map_stack_tail].y = j;
									map_stack_tail++;
									map_stack_tail&=0x07ff;//最大値制限 
									changeflu =1;
								}
							}
						}
						if (changeflu == 0 &&  step_map[i - 1][j] > ( step_map[i][j] + WEIGHT_TURN)) {		//値が入っていなければ
							 step_map[i - 1][j] =  step_map[i][j] + WEIGHT_TURN;		//値を代入
							map_stack[map_stack_tail].x = i - 1;
							map_stack[map_stack_tail].y = j;
							map_stack_tail++;
							map_stack_tail&=0x07ff;//最大値制限 
						}
					}
				}

			}
			//printf("%d \n", tail);
		
			
		
		}//whileの括弧閉じ
	}

}

void set_wall(char x, char y)	//壁情報を記録
{
//引数の座標x,yに壁情報を書き込む
	unsigned char n_write,s_write,e_write,w_write;
	
	//自分の方向に応じて書き込むデータを生成
	//CONV_SEN2WALL()はmacro.hを参照
	switch(mypos.dir){
		case north:	//北を向いている時
	
			if(wall[x][y].north != WALL){n_write = CONV_SEN2WALL(sen_fr.is_wall || sen_fl.is_wall);}else{n_write=WALL;}//前壁の有無を判断
			if(wall[x][y].east  != WALL){e_write = CONV_SEN2WALL(sen_r.is_wall);}else{e_write=WALL;}//右壁の有無を判断
			if(wall[x][y].west  != WALL){w_write = CONV_SEN2WALL(sen_l.is_wall);}else{w_write=WALL;}//左壁の有無を判断
			if(wall[x][y].south != WALL){s_write = NOWALL;	}else{s_write=WALL;}				//後ろ
			
			break;
			
		case east:	//東を向いているとき
		
			if(wall[x][y].east  != WALL){e_write = CONV_SEN2WALL(sen_fr.is_wall || sen_fl.is_wall);}else{e_write=WALL;}
			if(wall[x][y].south != WALL){s_write = CONV_SEN2WALL(sen_r.is_wall);	}else{s_write=WALL;}				
			if(wall[x][y].north != WALL){n_write = CONV_SEN2WALL(sen_l.is_wall);	}else{n_write=WALL;}			
			if(wall[x][y].west  != WALL){w_write = NOWALL;	}else{w_write=WALL;}					//後ろ
								
			break;
			
		case south:	//南を向いているとき
		
			if(wall[x][y].south != WALL){s_write = CONV_SEN2WALL(sen_fr.is_wall || sen_fl.is_wall);}else{s_write=WALL;}	
			if(wall[x][y].west  != WALL){w_write = CONV_SEN2WALL(sen_r.is_wall); }else{w_write=WALL;}
			if(wall[x][y].east  != WALL){e_write = CONV_SEN2WALL(sen_l.is_wall); }else{e_write=WALL;}
			if(wall[x][y].north != WALL){n_write = NOWALL;	}else{n_write=WALL;}					//後ろ

			break;
			
		case west:	//西を向いているとき
		
			if(wall[x][y].west  != WALL){w_write = CONV_SEN2WALL(sen_fr.is_wall || sen_fl.is_wall);	}else{w_write=WALL;}
			if(wall[x][y].north != WALL){n_write = CONV_SEN2WALL(sen_r.is_wall);	}else{n_write=WALL;}
			if(wall[x][y].south != WALL){s_write = CONV_SEN2WALL(sen_l.is_wall);	}else{s_write=WALL;}
			if(wall[x][y].east  != WALL){e_write = NOWALL;	}else{e_write=WALL;}					//後ろ
		
			break;
			
	}
	
	wall[x][y].north = n_write;	//実際に壁情報を書き込み
	wall[x][y].south = s_write;	//実際に壁情報を書き込み
	wall[x][y].east  = e_write;	//実際に壁情報を書き込み
	wall[x][y].west  = w_write;	//実際に壁情報を書き込み
	
	if(y < MAZESIZE_Y-1)	//範囲チェック
	{
		wall[x][y+1].south = n_write;	//反対側から見た壁を書き込み
	}
	
	if(x < MAZESIZE_X-1)	//範囲チェック
	{
		wall[x+1][y].west = e_write;	//反対側から見た壁を書き込み
	}
	
	if(y > 0)	//範囲チェック
	{
		wall[x][y-1].north = s_write;	//反対側から見た壁を書き込み
	}
	
	if(x > 0)	//範囲チェック
	{
		wall[x-1][y].east = w_write;	//反対側から見た壁を書き込み
	}
	
	map_flug[x][y].know=1;// 1:4面全て探索済み
	//if(zentansaku!=2){map_flug[x][y].ignore=1;}
}

void set_wall_F(char x, char y)	//前壁情報の再記録用
{
//引数の座標x,yに壁情報を書き込む
	unsigned char n_write,s_write,e_write,w_write;
	
	
	//自分の方向に応じて書き込むデータを生成
	//CONV_SEN2WALL()はmacro.hを参照
	switch(mypos.dir){
		case north:	//北を向いている時
			n_write = CONV_SEN2WALL(sen_fr.is_wall || sen_fl.is_wall);//壁の有無を判断
			if( step_map[x][y] != 0 && wall[x][y].north == WALL && n_write!=WALL){flug_run_err=1;break;}//あるはずの壁が無い→探索エラー
			wall[x][y].north = n_write;	//実際に壁情報を書き込み

			
			if(y < MAZESIZE_Y-1)	//範囲チェック
			{
				wall[x][y+1].south = n_write;	//反対側から見た壁を書き込み
			}
			
			break;
			
		case east:	//東を向いているとき
			e_write = CONV_SEN2WALL(sen_fr.is_wall || sen_fl.is_wall);
			if( step_map[x][y] != 0 && wall[x][y].east == WALL && e_write!=WALL){flug_run_err=1;break;}//あるはずの壁が無い→探索エラー
			wall[x][y].east  = e_write;	//実際に壁情報を書き込み
			
			
			if(x < MAZESIZE_X-1)	//範囲チェック
			{
				wall[x+1][y].west = e_write;	//反対側から見た壁を書き込み
			}

			break;
			
		case south:	//南を向いているとき
			s_write = CONV_SEN2WALL(sen_fr.is_wall || sen_fl.is_wall);
			if( step_map[x][y] != 0 && wall[x][y].south == WALL && s_write!= WALL){flug_run_err=1;break;}//あるはずの壁が無い→探索エラー	
			wall[x][y].south = s_write;	//実際に壁情報を書き込み
			

			if(y > 0)	//範囲チェック
			{
				wall[x][y-1].north = s_write;	//反対側から見た壁を書き込み
			}
			
			break;
			
		case west:	//西を向いているとき
			w_write = CONV_SEN2WALL(sen_fr.is_wall || sen_fl.is_wall);
			if( step_map[x][y] != 0 && wall[x][y].west == WALL && w_write != WALL){flug_run_err=1;break;}//あるはずの壁が無い→探索エラー
			wall[x][y].west  = w_write;	//実際に壁情報を書き込み
			
			
			
			if(x > 0)	//範囲チェック
			{
				wall[x-1][y].east = w_write;	//反対側から見た壁を書き込み
			}
			break;
			
	}
	

	
}



bool is_unknown(char x, char y)	//指定された区画が未探索か否かを判断する関数 未探索:true　探索済:false
{
	//座標x,yが未探索区間か否かを調べる UNKOWN の中身は"2"
	
	if(map_flug[x][y].know==1){return false;}//探索済
	
	if((wall[x][y].north == UNKNOWN) || (wall[x][y].east == UNKNOWN) || (wall[x][y].south == UNKNOWN) || (wall[x][y].west == UNKNOWN))
	{			//どこかの壁情報が不明のままであれば
		return true;	//未探索
	}
	else
	{	
		map_flug[x][y].know=1;
		return false;	//探索済
	}
}



char get_priority(char x, char y, t_direction dir)	//そのマスの情報から、優先度を算出する
{
	//座標x,yと、向いている方角dirから優先度を算出する
	
	//未探索が一番優先度が高い.(4)
	//それに加え、自分の向きと、行きたい方向から、
	//前(2)横(1)後(0)の優先度を付加する。

	char priority;	//優先度を記録する変数
	
	priority = 0;
	
	if(mypos.dir == dir)				//行きたい方向が現在の進行方向と同じ場合
	{
		priority = 2;
	}
	else if( ((4+mypos.dir-dir)%4) == 2)		//行きたい方向が現在の進行方向と逆の場合
	{
		priority = 0;
	}
	else						//それ以外(左右どちらか)の場合
	{
		priority = 1;
	}
	
	
	if(is_unknown(x,y) == true)
	{
		priority += 4;				//未探索の場合優先度をさらに付加
	}
	
	return priority;				//優先度を返す
	
}


char get_nextdir(unsigned char mask, t_direction *dir)	
{
	//ゴール座標x,yに向かう場合、今どちらに行くべきかを判断する。
	//探索、最短の切り替えのためのmaskを指定、dirは方角を示す
	char priority,tmp_priority;		//最小の値を探すために使用する変数
	unsigned short little;
	static t_direction old_dir[2];

	little =  step_map[mypos.x][mypos.y];			//仮で今いる場所の重さを入れておく


	priority = 0;					//優先度の初期値は0

	//maskの意味はstatic_parameter.hを参照
	if( (wall[mypos.x][mypos.y].north & mask) == NOWALL)			//北に壁がなければ
	{
		tmp_priority = get_priority(mypos.x, mypos.y + 1, north);	//優先度を算出
		if( step_map[mypos.x][mypos.y+1] < little)				//一番歩数が小さい方向を見つける
		{
			little =  step_map[mypos.x][mypos.y+1];			//ひとまず北が歩数が小さい事にする
			*dir = north;						//方向を保存
			priority = tmp_priority;				//優先度を保存
		}
		else if( step_map[mypos.x][mypos.y+1] == little)			//歩数が同じ場合は優先度から判断する
		{
			//if(priority < tmp_priority )				//優先度を評価
			//{
				*dir = north;					//方向を更新
				priority = tmp_priority;			//優先度を保存
			//}
		}
	}
	
	if( (wall[mypos.x][mypos.y].east & mask) == NOWALL)			//東に壁がなければ
	{
		tmp_priority = get_priority(mypos.x + 1, mypos.y, east);	//優先度を算出
		if( step_map[mypos.x + 1][mypos.y] < little)				//一番歩数が小さい方向を見つける
		{
			little =  step_map[mypos.x+1][mypos.y];			//ひとまず東が歩数が小さい事にする
			*dir = east;						//方向を保存
			priority = tmp_priority;				//優先度を保存
		}
		else if( step_map[mypos.x + 1][mypos.y] == little)			//歩数が同じ場合、優先度から判断
		{
			if(priority == tmp_priority && old_dir[1]==east)//同一歩数の場合は2つ前と同じ向きに進む⇒斜め
			{
				*dir = east;
			}
			else if(priority < tmp_priority)			//優先度を評価
			{
				*dir = east;					//方向を保存
				priority = tmp_priority;			//優先度を保存
			}
		}
	}
	
	if( (wall[mypos.x][mypos.y].south & mask) == NOWALL)			//南に壁がなければ
	{
		tmp_priority = get_priority(mypos.x, mypos.y - 1, south);	//優先度を算出
		if( step_map[mypos.x][mypos.y - 1] < little)				//一番歩数が小さい方向を見つける
		{
			little =  step_map[mypos.x][mypos.y-1];			//ひとまず南が歩数が小さい事にする
			*dir = south;						//方向を保存
			priority = tmp_priority;				//優先度を保存
		}
		else if( step_map[mypos.x][mypos.y - 1] == little)			//歩数が同じ場合、優先度で評価
		{
			if(priority == tmp_priority && old_dir[1]==south)
			{
				*dir = south;	
			}
			else if(priority < tmp_priority)			//優先度を評価
			{
				*dir = south;					//方向を保存
				priority = tmp_priority;			//優先度を保存
			}
		}
	}
	
	if( (wall[mypos.x][mypos.y].west & mask) == NOWALL)			//西に壁がなければ
	{
		tmp_priority = get_priority(mypos.x - 1, mypos.y, west);	//優先度を算出
		if( step_map[mypos.x-1][mypos.y] < little)				//一番歩数が小さい方向を見つける
		{
			little =  step_map[mypos.x-1][mypos.y];			//西が歩数が小さい
			*dir = west;						//方向を保存
			priority = tmp_priority;				//優先度を保存
		}
		else if( step_map[mypos.x - 1][mypos.y] == little)			//歩数が同じ場合、優先度で評価
		{
			if(priority == tmp_priority && old_dir[1]==west)
			{
				*dir = west;
			}
			else if(priority < tmp_priority)			//優先度を評価
			{			
				*dir = west;					//方向を保存
				priority = tmp_priority;			//優先度を保存
			}
		}
	}
	

	//_LED(0);
	old_dir[1] = old_dir[0];
	old_dir[0] = (*dir);
	return ( (char)( ( 4 + *dir - mypos.dir) % 4 ) );			//どっちに向かうべきかを返す。
										//演算の意味はmytyedef.h内のenum宣言から。
	
}



void search_adachi(char gx, char gy)
{
//引数gx,gyに向かって足立法で迷路を探索する
	t_direction glob_nextdir,no_use;					//次に向かう方向を記録する変数

	signed char tmp_rotation_direc;
	signed char tmp_ofset_flug = 0;

	char known_st_cnt=0;
	char check_stop_flug,check_stop_left,check_stop_right;

	
	
	make_map(gx,gy,MASK_SEARCH,FAST_SEACH);				//歩数Map生成
	if( step_map[mypos.x][mypos.y]==0xffff){
    digitalWrite(MOTOR_EN, 0);  // nsleepをHIGHでモータ励磁
    lcd.printf("map_break \n");
    return;
  }
	switch(get_nextdir(MASK_SEARCH,&glob_nextdir))		//次に行く方向を戻り値とする関数を呼ぶ
	{
		case front:
			end_f_sensor =REF_SEN_FT;
			ref_step = total_step;//距離0点
			straight_for_search(SECOND_HALF_SECTION,SEARCH_SPEED);		//半区画進む
			ref_step = total_step;//距離0点
			break;
		
		case right:
			rotate(right,1);					//右に曲がって
			end_f_sensor =REF_SEN_FT;
			straight_for_search(SECOND_HALF_SECTION,SEARCH_SPEED);		//半区画進む
			ref_step = total_step;//距離0点
			break;
		
		case left:
			rotate(left,1);						//左に曲がって
			end_f_sensor =REF_SEN_FT;
			straight_for_search(SECOND_HALF_SECTION,SEARCH_SPEED);		//半区画進む
			ref_step = total_step;//距離0点
			break;
		
		case rear:
			rotate(right,2);				//180ターン
			end_f_sensor =REF_SEN_FT;
			straight_for_search(SECOND_HALF_SECTION,SEARCH_SPEED);		//半区画進む
			ref_step = total_step;//距離0点
			break;
	}

		//con_wall.enable = true;					//壁制御を有効にする
		ref_step = total_step;//距離0点				//進んだ距離カウント用変数をリセット
	
		mypos.dir = glob_nextdir;				//方向を更新


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

	ref_step = total_step;//距離0点
	//while((mypos.x != gx) || (mypos.y != gy) ){	//ゴール座標（1点）に到達するまで繰り返す
  while( step_map[mypos.x][mypos.y]!=0 ){			//ゴール区間に入るまで繰り返す
	//while( step_map[mypos.x][mypos.y]!=0 || is_unknown(mypos.x,mypos.y)==true ){	//未探索のゴールがなくなるまで繰り返す。||zentansaku!=0
	  //	ゴールではない                      未探索
		
		if(flug_run_err==1){break;}
		//_LED(2);
		//if(gtimaer==120000 || gtimaer==180000 || gtimaer==240000 || gtimaer==300000 || gtimaer==330000|| gtimaer==360000){save_map_flug=1;}
		//	     2分	     3分		4分		   5分		    5分半		6分
		//straight_for_search(MM2LENGTH(10),SEARCH_SPEED);
		
		tmp_rotation_direc = 0;
		//if(line_r>(cal_cen_r+10)){tmp_rotation_direc = -1;}//右に近い→左周り 17->5mm
		//if(line_l>(cal_cen_l+10)){tmp_rotation_direc =  1;}//左に近い→右回り
		//if(line_r>TH_SEN_R && line_l<TH_SEN_L){tmp_rotation_direc =  1;}//右壁しか無い→右回り
		//if(line_l>TH_SEN_L && line_r<TH_SEN_R){tmp_rotation_direc = -1;}//左壁しかない→左回り
		
		cnt2 = 0;


		set_wall(mypos.x,mypos.y);					//壁をセット
		make_map(gx,gy,MASK_SEARCH,FAST_SEACH);				//歩数Map生成
		
    //delay(500);
		
		//if( step_map[mypos.x][mypos.y]==0xffff){zentansaku=2;break;}//荒探索完了、最短経路上未探索に切り替え
		//if(zentansaku==1 && gtimaer>_6MINS){zentansaku=2;break;}//探索制限時間 5分後に最短経路上未探索に切り替え
		//if(zentansaku==2 && gtimaer>_7MINS){break;}//7分で探索打ち切り

	  if(step_map[mypos.x][mypos.y] == 0xffff){//MAP破壊(目標地点までが塞がっている。)
		  digitalWrite(MOTOR_EN, 0);  // nsleepをHIGHでモータ励磁
      lcd.setCursor(0, 0); 
      lcd.fillScreen(BLACK);
      lcd.printf("map_break \n");
		  while(1){
			  delay(1);
		  }
    }
		switch(get_nextdir(MASK_SEARCH,&glob_nextdir))		//次に行く方向を戻り値とする関数を呼ぶ
		{
			case front:
				//eda_kari();//枝刈り処理を回す

				if(mypos.dir == north && is_unknown(mypos.x, mypos.y+1)==false && wall[mypos.x][mypos.y+1].north == NOWALL){	//北向き && 1区間先も既知区間
					known_st_cnt=0;
					while(mypos.y<=29){
						mypos.y++;//仮想的に一歩進める
						if(get_nextdir(MASK_SECOND,&no_use)==front && is_unknown(mypos.x, mypos.y+1)==false){
							known_st_cnt++;
						   }else{
							mypos.y--;//一歩戻す(後ろで+1するので、調整)
						   	break;
						   }
					}
					end_f_sensor = REF_SEN_FT;
					straight(SECTION*(known_st_cnt+1),SEARCH_SPEED);//直線まとめて進む
					
				}else if(mypos.dir == east && is_unknown(mypos.x+1, mypos.y)==false && wall[mypos.x+1][mypos.y].east == NOWALL){	//東向き && 1区間先も既知区間
					known_st_cnt=0;
					while(mypos.x<=29){
						mypos.x++;//仮想的に一歩進める
						if(get_nextdir(MASK_SECOND,&no_use)==front && is_unknown(mypos.x+1, mypos.y)==false){
							known_st_cnt++;
						   }else{
							mypos.x--;//一歩戻す(後ろで+1するので、調整)
						   	break;
						   }
					}
					end_f_sensor = REF_SEN_FT;
					straight(SECTION*(known_st_cnt+1),SEARCH_SPEED);//直線まとめて進む
					
				}else if(mypos.dir == south && is_unknown(mypos.x, mypos.y-1)==false && wall[mypos.x][mypos.y-1].south == NOWALL){	//南向き && 1区間先も既知区間
					known_st_cnt=0;
					while(mypos.y>=2){
						mypos.y--;//仮想的に一歩進める
						if(get_nextdir(MASK_SECOND,&no_use)==front && is_unknown(mypos.x, mypos.y-1)==false){
							known_st_cnt++;
						   }else{
							mypos.y++;//一歩戻す(後ろで+1するので、調整)
						   	break;
						   }
					}
					end_f_sensor = REF_SEN_FT;
					straight(SECTION*(known_st_cnt+1),SEARCH_SPEED);//直線まとめて進む
					
				}else if(mypos.dir == west && is_unknown(mypos.x-1, mypos.y)==false && wall[mypos.x-1][mypos.y].west == NOWALL){	//西向き && 1区間先も既知区間
					known_st_cnt=0;
					while(mypos.x>=2){
						mypos.x--;//仮想的に一歩進める
						if(get_nextdir(MASK_SECOND,&no_use)==front && is_unknown(mypos.x-1, mypos.y)==false){
							known_st_cnt++;
						   }else{
							mypos.x++;//一歩戻す(後ろで+1するので、調整)
						   	break;
						   }
					}
					end_f_sensor = REF_SEN_FT;
					straight(SECTION*(known_st_cnt+1),SEARCH_SPEED);//直線まとめて進む
					
				}else{

					
					
					end_f_sensor = REF_SEN_FT;
					straight_for_search(MM2LENGTH(20),SEARCH_SPEED);
					set_wall_F(mypos.x,mypos.y);		//前壁再確認
					
					if(sen_fr.is_wall || sen_fl.is_wall){	//	前壁があったら（前壁を読み損ねていた場合）
						straight_for_search(FIRST_HALF_SECTION,0);	//半区画進んで
						ref_step = total_step;//距離0点
						glob_nextdir=(t_direction)((glob_nextdir+2)%4);
						goto janpstop;
					}
					
					end_f_sensor = REF_SEN_FT;
					straight_for_search(SECTION,SEARCH_SPEED);	//一区画進む
					ref_step = total_step;//距離0点
					


				}
				tmp_ofset_flug=0;
				break;
				
			case right:
          straight_for_search(D33MM, SLAM_SPEED);//06+27=33
        	if((line_fl<line_fr+10 && line_fl>line_fr-10) || (sen_fl.is_wall == false && sen_fr.is_wall == false) ){
						if(line_l>REF_SEN_L	+15){tmp_ofset_flug=-1;}//左壁に近い→右にずれる
        		turn(right,1);
        		if(tmp_ofset_flug==1){ref_step=total_step + D15MM;}
        		delay(100);
        		end_f_sensor = REF_SEN_FT;
        		straight_for_search(D42MM, SLAM_SPEED);//75-33=42
        		ref_step = total_step;//距離0点
        		tmp_ofset_flug=0;
					}else{
						straight_for_search(FIRST_HALF_SECTION,0);		//半区画進んで
          	if(line_l>REF_SEN_L	+15){tmp_ofset_flug=-1;}//左壁に近い→右にずれる
						rotate(right,1);					//右に曲がって
						//end_f_sensor =END_SEN_FT;
          	if(tmp_ofset_flug==1){ref_step=total_step + D15MM;}
						straight_for_search(SECOND_HALF_SECTION,SEARCH_SPEED);		//半区画進む
						ref_step = total_step;//距離0点
					
          	tmp_ofset_flug=0;
					}
				break;
			
			case left:
			    straight_for_search(D33MM, SLAM_SPEED);
					if((line_fl<line_fr+10 && line_fl>line_fr-10) || (sen_fl.is_wall == false && sen_fr.is_wall == false) ){//前壁との角度誤差がない場合（と前壁がない場合）にだけスラロームする
        		if(line_r>REF_SEN_R	+15){tmp_ofset_flug=1;}//右壁に近い→左にずれる
        		turn(left,1);
        		if(tmp_ofset_flug==1){ref_step=total_step + D15MM;}
        		delay(100);
        		end_f_sensor = REF_SEN_FT;
        		straight_for_search(D42MM, SLAM_SPEED);
        		ref_step = total_step;//距離0点
        		tmp_ofset_flug=0;						
					}else{
          	straight_for_search(FIRST_HALF_SECTION,0);		//半区画進んで
          	if(line_r>REF_SEN_R	+15){tmp_ofset_flug=1;}//右壁に近い→左にずれる
						rotate(left,1);					//右に曲がって
						//end_f_sensor =END_SEN_FT;
         	 if(tmp_ofset_flug==1){ref_step=total_step + D15MM;}
						straight_for_search(SECOND_HALF_SECTION,SEARCH_SPEED);		//半区画進む
						ref_step = total_step;//距離0点

          	tmp_ofset_flug=0;
					}
				break;
				
			
			case rear:
        //end_f_sensor =END_SEN_FT;
				straight_for_search(FIRST_HALF_SECTION,0);		//半区画進んで

				janpstop:
				
				if(tmp_rotation_direc == -1){
					rotate(left,2);
				}else if(tmp_rotation_direc == 1){
					rotate(right,2);
				}else{
					rotate(left,2);				//180ターン
				}
        if(line_r>REF_SEN_R	+10){tmp_ofset_flug= 1;}//右壁に近い→左にずれる
        if(line_l>REF_SEN_L	+10){tmp_ofset_flug=-1;}//左壁に近い→右にずれる
				
				
				ref_step = total_step;//距離0点
				//end_f_sensor =END_SEN_FT;
				straight_for_search(SECOND_HALF_SECTION,SEARCH_SPEED);		//半区画進む
				ref_step = total_step;//距離0点
				break;
		}

	
		mypos.dir = glob_nextdir;					//方向を更新
		
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
		



	} //while
	

		//半区間すすんで止まる。
		set_wall(mypos.x,mypos.y);		//壁をセット
		if(sen_fl.is_wall ==false && sen_fr.is_wall ==false){//前壁が無い場合
			ref_step = total_step;//距離0点
			//straight_for_search(MM2LENGTH(10),SEARCH_SPEED);
			//set_wall_F(mypos.x,mypos.y);			//前壁再確認
			//end_f_sensor = END_SEN_FT;
			straight_for_search(FIRST_HALF_SECTION,0);	//ゴール.半区画進む
		}else{
			ref_step = total_step;//距離0点
			//end_f_sensor = END_SEN_FT;
			straight_for_search(FIRST_HALF_SECTION,0);	//ゴール.半区画進む
	  }


	ref_step = total_step;//距離0
}



