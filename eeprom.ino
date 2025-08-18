char ep_x,ep_y;
int ep_addr;
byte ep_data;


void save_map(void){
  ep_addr = 0;
  for (ep_x = 0; ep_x < MAZESIZE_X; ep_x++){
    for (ep_y = 0; ep_y < MAZESIZE_Y; ep_y++){
      // 
      EEPROM.write(ep_addr, (wall[ep_x][ep_y].north<<6 | wall[ep_x][ep_y].east<<4 | wall[ep_x][ep_y].south<<2 | wall[ep_x][ep_y].west) );
      ep_addr++;
    }
  }
  // See also the general purpose writeBytes() and readBytes() for BLOB in EEPROM library
  EEPROM.commit();
}

void load_map(void){
  ep_addr = 0;
  for (ep_x = 0; ep_x < MAZESIZE_X; ep_x++){
    for (ep_y = 0; ep_y < MAZESIZE_Y; ep_y++){
      ep_data = EEPROM.read(ep_addr);
      wall[ep_x][ep_y].north = ((ep_data >> 6)&0x03);
      wall[ep_x][ep_y].east  = ((ep_data >> 4)&0x03);
      wall[ep_x][ep_y].south = ((ep_data >> 2)&0x03);
      wall[ep_x][ep_y].west  = (ep_data&0x03);
      ep_addr++;
    }
  }

}

void view_map(void){

        canvas.fillScreen(BLACK);
        canvas.fillRect(0, 0, 226, 226, RED);
        canvas.fillRect(2, 2, 222, 222, DARKGREY);//左上が原点
        for(ep_x=0;ep_x<16;ep_x++){
          for(ep_y=0;ep_y<16;ep_y++){
            canvas.fillRect(2+ep_x*14, 2+ep_y*14, 12, 12, BLACK);
          }
        }

        for(ep_x=0;ep_x<16;ep_x++){
          for(ep_y=0;ep_y<16;ep_y++){
            //canvas.fillRect(2+ep_x*14, 2+ep_y*14, 12, 12, RED);
            //北
            if(wall[ep_x][ep_y].north==WALL  ){canvas.fillRect(2+ep_x*14, (15-ep_y)*14, 12, 2, RED);}
            if(wall[ep_x][ep_y].north==NOWALL){canvas.fillRect(2+ep_x*14, (15-ep_y)*14, 12, 2, BLACK);}
            //東
            if(wall[ep_x][ep_y].east ==WALL  ){canvas.fillRect((ep_x+1)*14, 2+(15-ep_y)*14, 2, 12, RED);}
            if(wall[ep_x][ep_y].east ==NOWALL){canvas.fillRect((ep_x+1)*14, 2+(15-ep_y)*14, 2, 12, BLACK);}
          }
        } 
        


        //canvas.fillRect(100, 100, 26, 26, BLACK);//ゴール

        canvas.fillRect(14, 212, 2, 12, YELLOW);//スタート右壁
        canvas.fillRect( 0, 212, 2, 12, YELLOW);//スタート左壁
        canvas.fillRect( 0, 224, 16, 2, YELLOW);//スタート後壁

        canvas.pushSprite(&lcd, 7, 50);
}