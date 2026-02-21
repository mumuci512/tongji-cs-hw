module moving//蛇的运动情况控制模块
(
	input pclk,//50M时钟
	input sys_retn,//全局复位信号
	
	input [31:0] PTMAX,

	
	input bt_GOLEFT,//左按键
	input bt_GORIGHT,//右按键
	input bt_GOUP,//上按键
	input bt_GODOWN,//下按键
	
	output reg [1:0] snake_st,
	input [9:0] Pixel_x,
	input [9:0] Pixel_y,
	
	output [5:0] fist_x,
	output [5:0] fist_y,
	
	input growGOUP,
	
	input [2:0] cur_st,//3种游戏状态，IDLE：001；GAMING：010；end：100；
	
	output reg inSANEK_ELSE,//撞到身体信号
	output reg inFOURWALL//撞到墙信号

);

	localparam GOUP   = 2'b00;
	localparam GODOWN = 2'b01;
	localparam GOLEFT = 2'b10;
	localparam GORIGHT = 2'b11;
	
	
	localparam NOTHING = 2'b00;
	localparam SANEK_FISRT = 2'b01;
	localparam SANEK_ELSE = 2'b10;
	localparam FOURWALL = 2'b11;
	

	localparam   ST_DONE = 3'b100;


	reg [6:0] howlong;
	
	reg [31:0] time_cnt;
	
	wire [1:0] go_fw;
	reg [1:0] go_fwr;//寄存方向的寄存器
	reg [1:0] gonext = 2'b11 ;//下一个方向的寄存器

	assign go_fw = go_fwr;
	
	
	reg go_fwGOLEFT;//方向左
	reg go_fwGORIGHT;//方向右
	reg go_fwGOUP;//方向上
	reg go_fwGODOWN;//方向下
	
	reg [5:0] SANEK_ELSE_x [15:0];//这种格式是二维数组的意思，地址为0~15，每个地址为6bits
	reg [5:0] SANEK_ELSE_y [15:0];
	reg [15:0] show_ll;//这用来控制蛇体亮暗，1为亮，0为暗
	
	
	assign fist_x = SANEK_ELSE_x[0];//SANEK_ELSE_x，SANEK_ELSE_y表示一整条蛇身体各节的格坐标
	assign fist_y = SANEK_ELSE_y[0];
	
	
	always @ (posedge pclk or negedge sys_retn)
	begin
		if(!sys_retn)
			go_fwr <= GORIGHT;//默认一出来就是向右前进
		else
			go_fwr <= gonext;
	end


	
	always @ (posedge pclk or negedge sys_retn)
	begin
		if(!sys_retn)
		begin
			time_cnt <= 32'd0;
		
			SANEK_ELSE_x[0] <= 6'd10;//蛇头默认出现的X坐标是10
			SANEK_ELSE_y[0] <= 6'd5;//蛇头默认出现的Y坐标是5
			
			SANEK_ELSE_x[1] <= 6'd9;//第一节身体的X坐标是9
			SANEK_ELSE_y[1] <= 6'd5;//第一节身体的Y坐标还是5
			
			SANEK_ELSE_x[2] <= 6'd8;//第二节身体的X坐标是8
			SANEK_ELSE_y[2] <= 6'd5;//第二节身体的Y坐标还是5
			
	
			SANEK_ELSE_x[3] <= 6'd0;
			SANEK_ELSE_y[3] <= 6'd0;
			
			SANEK_ELSE_x[4] <= 6'd0;
			SANEK_ELSE_y[4] <= 6'd0;
			
			SANEK_ELSE_x[5] <= 6'd0;
			SANEK_ELSE_y[5] <= 6'd0;
			
			SANEK_ELSE_x[6] <= 6'd0;
			SANEK_ELSE_y[6] <= 6'd0;
			
			SANEK_ELSE_x[7] <= 6'd0;
			SANEK_ELSE_y[7] <= 6'd0;
			
			SANEK_ELSE_x[8] <= 6'd0;
			SANEK_ELSE_y[8] <= 6'd0;
			
			SANEK_ELSE_x[9] <= 6'd0;
			SANEK_ELSE_y[9] <= 6'd0;
			
			SANEK_ELSE_x[10] <= 6'd0;
			SANEK_ELSE_y[10] <= 6'd0;
			
			SANEK_ELSE_x[11] <= 6'd0;
			SANEK_ELSE_y[11] <= 6'd0;
			
			SANEK_ELSE_x[12] <= 6'd0;
			SANEK_ELSE_y[12] <= 6'd0;
			
			SANEK_ELSE_x[13] <= 6'd0;
			SANEK_ELSE_y[13] <= 6'd0;
			
			SANEK_ELSE_x[14] <= 6'd0;
			SANEK_ELSE_y[14] <= 6'd0;
			
			SANEK_ELSE_x[15] <= 6'd0;
			SANEK_ELSE_y[15] <= 6'd0;
			
			inFOURWALL <= 1'd0;
			inSANEK_ELSE <= 1'd0;
		end
		else	if(time_cnt == PTMAX) //0.02us*12'500'000 = 0.25s，每秒移动四次
				begin
					time_cnt <= 32'd0;
					
					if(cur_st == ST_DONE)
					begin
						SANEK_ELSE_x[0] <= 6'd9;
						SANEK_ELSE_y[0] <= 6'd6; 
						
						SANEK_ELSE_x[1] <= 6'd8;
						SANEK_ELSE_y[1] <= 6'd6;
						
						SANEK_ELSE_x[2] <= 6'd7;
						SANEK_ELSE_y[2] <= 6'd6;
						

						SANEK_ELSE_x[3] <= 6'd0;
						SANEK_ELSE_y[3] <= 6'd0;
						
						SANEK_ELSE_x[4] <= 6'd0;
						SANEK_ELSE_y[4] <= 6'd0;
						
						SANEK_ELSE_x[5] <= 6'd0;
						SANEK_ELSE_y[5] <= 6'd0;
						
						SANEK_ELSE_x[6] <= 6'd0;
						SANEK_ELSE_y[6] <= 6'd0;
						
						SANEK_ELSE_x[7] <= 6'd0;
						SANEK_ELSE_y[7] <= 6'd0;
						
						SANEK_ELSE_x[8] <= 6'd0;
						SANEK_ELSE_y[8] <= 6'd0;
						
						SANEK_ELSE_x[9] <= 6'd0;
						SANEK_ELSE_y[9] <= 6'd0;
						
						SANEK_ELSE_x[10] <= 6'd0;
						SANEK_ELSE_y[10] <= 6'd0;
						
						SANEK_ELSE_x[11] <= 6'd0;
						SANEK_ELSE_y[11] <= 6'd0;
						
						SANEK_ELSE_x[12] <= 6'd0;
						SANEK_ELSE_y[12] <= 6'd0;
						
						SANEK_ELSE_x[13] <= 6'd0;
						SANEK_ELSE_y[13] <= 6'd0;
						
						SANEK_ELSE_x[14] <= 6'd0;
						SANEK_ELSE_y[14] <= 6'd0;
						
						SANEK_ELSE_x[15] <= 6'd0;
						SANEK_ELSE_y[15] <= 6'd0;
						
						inFOURWALL <= 1'd0;
						inSANEK_ELSE <= 1'd0;

					end
					else
					begin//撞墙有四种情况，上下左右，撞到上，Y = 1；撞到下边，Y = 28；撞到左边，X = 1；撞到右边，X = 38；画个图就知道了~
						if( ((go_fw == GOUP) && (SANEK_ELSE_y[0] == 6'd1)) | 
							((go_fw == GODOWN) && (SANEK_ELSE_y[0] == 6'd28)) | 
							((go_fw == GOLEFT) && (SANEK_ELSE_x[0] == 6'd1)) | 
							((go_fw == GORIGHT) && (SANEK_ELSE_x[0] == 6'd38))|
	
							(   ( (SANEK_ELSE_x[0] >=16) && (SANEK_ELSE_x[0] < 24) ) && ( (SANEK_ELSE_y[0] >= 14) && (SANEK_ELSE_y[0] < 16))  )
							)
							//16 24 x     // 
							//14 16 y     // 
							inFOURWALL <= 1'd1;
						else if(((SANEK_ELSE_y[0] == SANEK_ELSE_y[1]) && (SANEK_ELSE_x[0] == SANEK_ELSE_x[1]) && (show_ll[1] == 1'd1)) | 
								((SANEK_ELSE_y[0] == SANEK_ELSE_y[2]) && (SANEK_ELSE_x[0] == SANEK_ELSE_x[2]) && (show_ll[2] == 1'd1)) | 
								((SANEK_ELSE_y[0] == SANEK_ELSE_y[3]) && (SANEK_ELSE_x[0] == SANEK_ELSE_x[3]) && (show_ll[3] == 1'd1)) | 
								((SANEK_ELSE_y[0] == SANEK_ELSE_y[4]) && (SANEK_ELSE_x[0] == SANEK_ELSE_x[4]) && (show_ll[4] == 1'd1)) | 
								((SANEK_ELSE_y[0] == SANEK_ELSE_y[5]) && (SANEK_ELSE_x[0] == SANEK_ELSE_x[5]) && (show_ll[5] == 1'd1)) | 
								((SANEK_ELSE_y[0] == SANEK_ELSE_y[6]) && (SANEK_ELSE_x[0] == SANEK_ELSE_x[6]) && (show_ll[6] == 1'd1)) | 
								((SANEK_ELSE_y[0] == SANEK_ELSE_y[7]) && (SANEK_ELSE_x[0] == SANEK_ELSE_x[7]) && (show_ll[7] == 1'd1)) | 
								((SANEK_ELSE_y[0] == SANEK_ELSE_y[8]) && (SANEK_ELSE_x[0] == SANEK_ELSE_x[8]) && (show_ll[8] == 1'd1)) | 
								((SANEK_ELSE_y[0] == SANEK_ELSE_y[9]) && (SANEK_ELSE_x[0] == SANEK_ELSE_x[9]) && (show_ll[9] == 1'd1)) | 
								((SANEK_ELSE_y[0] == SANEK_ELSE_y[10]) && (SANEK_ELSE_x[0] == SANEK_ELSE_x[10]) && (show_ll[10] == 1'd1)) | 
								((SANEK_ELSE_y[0] == SANEK_ELSE_y[11]) && (SANEK_ELSE_x[0] == SANEK_ELSE_x[11]) && (show_ll[11] == 1'd1)) | 
								((SANEK_ELSE_y[0] == SANEK_ELSE_y[12]) && (SANEK_ELSE_x[0] == SANEK_ELSE_x[12]) && (show_ll[12] == 1'd1)) | 
								((SANEK_ELSE_y[0] == SANEK_ELSE_y[13]) && (SANEK_ELSE_x[0] == SANEK_ELSE_x[13]) && (show_ll[13] == 1'd1)) | 
								((SANEK_ELSE_y[0] == SANEK_ELSE_y[14]) && (SANEK_ELSE_x[0] == SANEK_ELSE_x[14]) && (show_ll[14] == 1'd1)) | 
								((SANEK_ELSE_y[0] == SANEK_ELSE_y[15]) && (SANEK_ELSE_x[0] == SANEK_ELSE_x[15]) && (show_ll[15] == 1'd1)))
								
								inSANEK_ELSE <= 1'd1;
						else
						begin
							SANEK_ELSE_x[1] <= SANEK_ELSE_x[0];
							SANEK_ELSE_y[1] <= SANEK_ELSE_y[0];
							
							SANEK_ELSE_x[2] <= SANEK_ELSE_x[1];
							SANEK_ELSE_y[2] <= SANEK_ELSE_y[1];
							
							SANEK_ELSE_x[3] <= SANEK_ELSE_x[2];
							SANEK_ELSE_y[3] <= SANEK_ELSE_y[2];
							
							SANEK_ELSE_x[4] <= SANEK_ELSE_x[3];
							SANEK_ELSE_y[4] <= SANEK_ELSE_y[3];
							
							SANEK_ELSE_x[5] <= SANEK_ELSE_x[4];
							SANEK_ELSE_y[5] <= SANEK_ELSE_y[4];
							
							SANEK_ELSE_x[6] <= SANEK_ELSE_x[5];
							SANEK_ELSE_y[6] <= SANEK_ELSE_y[5];
							
							SANEK_ELSE_x[7] <= SANEK_ELSE_x[6];
							SANEK_ELSE_y[7] <= SANEK_ELSE_y[6];
							
							SANEK_ELSE_x[8] <= SANEK_ELSE_x[7];
							SANEK_ELSE_y[8] <= SANEK_ELSE_y[7];
							
							SANEK_ELSE_x[9] <= SANEK_ELSE_x[8];
							SANEK_ELSE_y[9] <= SANEK_ELSE_y[8];
							
							SANEK_ELSE_x[10] <= SANEK_ELSE_x[9];
							SANEK_ELSE_y[10] <= SANEK_ELSE_y[9];
							
							SANEK_ELSE_x[11] <= SANEK_ELSE_x[10];
							SANEK_ELSE_y[11] <= SANEK_ELSE_y[10];
							
							SANEK_ELSE_x[12] <= SANEK_ELSE_x[11];
							SANEK_ELSE_y[12] <= SANEK_ELSE_y[11];
							
							SANEK_ELSE_x[13] <= SANEK_ELSE_x[12];
							SANEK_ELSE_y[13] <= SANEK_ELSE_y[12];
							
							SANEK_ELSE_x[14] <= SANEK_ELSE_x[13];
							SANEK_ELSE_y[14] <= SANEK_ELSE_y[13];
							
							SANEK_ELSE_x[15] <= SANEK_ELSE_x[14];
							SANEK_ELSE_y[15] <= SANEK_ELSE_y[14];
							
							case(go_fw)						
								GOUP:
									begin
									if(SANEK_ELSE_y[0] == 6'd1)
											inFOURWALL <= 1'd1;
										else
											SANEK_ELSE_y[0] <= SANEK_ELSE_y[0] - 6'd1;
									end
								
								GODOWN:
									begin
										if(SANEK_ELSE_y[0] == 6'd28)
											inFOURWALL <= 1'd1;
										else
											SANEK_ELSE_y[0] <= SANEK_ELSE_y[0] + 6'd1;
									end
								GOLEFT:
									begin
										if(SANEK_ELSE_x[0] == 6'd1)
											inFOURWALL <= 1'd1;
										else
											SANEK_ELSE_x[0] <= SANEK_ELSE_x[0] - 6'd1;
									end
								
								GORIGHT:
									begin
										if(SANEK_ELSE_x[0] == 6'd38)
											inFOURWALL <= 1'd1;
										else
											SANEK_ELSE_x[0] <= SANEK_ELSE_x[0] + 6'd1;
									end
							endcase																	
						end					
					end
			end 
		else
			time_cnt <= time_cnt + 32'd1;
	end

	always @ (posedge pclk)//给四个按键赋值
	begin
		if(bt_GOLEFT == 1'd1)
				go_fwGOLEFT <= 1'd1;
		else if(bt_GORIGHT == 1'd1)
				go_fwGORIGHT <= 1'd1;
		else if(bt_GOUP == 1'd1)
				go_fwGOUP <= 1'd1;
		else if(bt_GODOWN == 1'd1)
				go_fwGODOWN <= 1'd1;			
		else 
		begin
			go_fwGOLEFT <= 1'd0;
			go_fwGORIGHT <= 1'd0;
			go_fwGOUP <= 1'd0;
			go_fwGODOWN <= 1'd0;
		end
	end

	always @ (*)
	begin   
		
		case(go_fw)
			GOUP:
				begin
					if(go_fwGOLEFT)
						gonext = GOLEFT;
					else if(go_fwGORIGHT)
						gonext = GORIGHT;
					else
						gonext = GOUP;
				end
			
			GODOWN:
				begin
					if(go_fwGOLEFT)
						gonext = GOLEFT;
					else if(go_fwGORIGHT)
						gonext = GORIGHT;
					else
						gonext = GODOWN;
				end		
			GOLEFT:
				begin
					if(go_fwGOUP)
						gonext = GOUP;
					else if(go_fwGODOWN)
						gonext = GODOWN;
					else
						gonext = GOLEFT;
				end
			
			GORIGHT:
				begin
					if(go_fwGOUP)
						gonext = GOUP;
					else if(go_fwGODOWN)
						gonext = GODOWN;
					else
						gonext = GORIGHT;
				end		
		endcase
	end


	reg getit;//吃苹果状态
	
	always @ (posedge pclk or negedge sys_retn)
	begin
		if(!sys_retn)
		begin
			show_ll <= 16'd7;
			howlong <= 7'd3;
			getit <= 1'd0;
		end		
		else if(cur_st == ST_DONE)
				begin
					howlong <= 7'd3;
					getit <= 1'd0;
					show_ll <= 16'd7;
				end 
		else				
		begin//判断蛇头与苹果坐标是否重合
			case(getit)
				1'd0	:
					begin
						if(growGOUP)
						begin
							howlong <= howlong + 7'd1;
							show_ll[howlong] <= 1'b1;
							getit <= 1'd1;
						end
						else
							show_ll[howlong] <= show_ll[howlong];
					end
				1'd1	:
					begin
						if(!growGOUP)//等待身体增长信号为0
							getit <= 0;
						else
							getit <= getit;
					end
			endcase
		end
	end
	//0.........40 x
	//..........	
	//0........30
	parameter C_FOURWALL_X  = 20 ;
	parameter C_FOURWALL_X_WD  = 4 ;  //4	
	parameter C_FOURWALL_Y  = 15 ;	
	parameter C_FOURWALL_Y_WD  = 1 ;	//2 
	//16 24 x
	//14 16 y
	always @ (*)//注意，这里是电平触发，所以用的是阻塞赋值“=”
	begin
		if((Pixel_x >= 10'd0) && (Pixel_x < 10'd640) && (Pixel_y >= 10'd0) && (Pixel_y < 10'd480))//如果扫描像素在目标框640*480内
		begin
			if( Pixel_x[9:4] == 6'd0 |
				Pixel_y[9:4] == 6'd0 | 
				Pixel_x[9:4] == 6'd39 | 
				Pixel_y[9:4] == 6'd29  
				//|((Pixel_x[9:4] >= C_FOURWALL_X-C_FOURWALL_X_WD) && (Pixel_x[9:4] < C_FOURWALL_X+C_FOURWALL_X_WD) && (Pixel_y[9:4] >= C_FOURWALL_Y-C_FOURWALL_Y_WD) && (Pixel_y[9:4] <  C_FOURWALL_Y+C_FOURWALL_Y_WD) )  
				)
				
				snake_st = FOURWALL;//扫描对象是墙
			else if(Pixel_x[9:4] == SANEK_ELSE_x[0] && Pixel_y[9:4] == SANEK_ELSE_y[0] && show_ll[0] == 1'd1)
					snake_st = 	SANEK_FISRT	;
			else if(((Pixel_x[9:4] == SANEK_ELSE_x[1]) && (Pixel_y[9:4] == SANEK_ELSE_y[1]) && (show_ll[1] == 1'd1)) | 
					 ((Pixel_x[9:4] == SANEK_ELSE_x[2]) && (Pixel_y[9:4] == SANEK_ELSE_y[2]) && (show_ll[2] == 1'd1)) | 
					 ((Pixel_x[9:4] == SANEK_ELSE_x[3]) && (Pixel_y[9:4] == SANEK_ELSE_y[3]) && (show_ll[3] == 1'd1)) | 
					 ((Pixel_x[9:4] == SANEK_ELSE_x[4]) && (Pixel_y[9:4] == SANEK_ELSE_y[4]) && (show_ll[4] == 1'd1)) | 
					 ((Pixel_x[9:4] == SANEK_ELSE_x[5]) && (Pixel_y[9:4] == SANEK_ELSE_y[5]) && (show_ll[5] == 1'd1)) | 
					 ((Pixel_x[9:4] == SANEK_ELSE_x[6]) && (Pixel_y[9:4] == SANEK_ELSE_y[6]) && (show_ll[6] == 1'd1)) | 
					 ((Pixel_x[9:4] == SANEK_ELSE_x[7]) && (Pixel_y[9:4] == SANEK_ELSE_y[7]) && (show_ll[8] == 1'd1)) | 
					 ((Pixel_x[9:4] == SANEK_ELSE_x[8]) && (Pixel_y[9:4] == SANEK_ELSE_y[8]) && (show_ll[8] == 1'd1)) | 
					 ((Pixel_x[9:4] == SANEK_ELSE_x[9]) && (Pixel_y[9:4] == SANEK_ELSE_y[9]) && (show_ll[9] == 1'd1)) | 
					 ((Pixel_x[9:4] == SANEK_ELSE_x[10]) && (Pixel_y[9:4] == SANEK_ELSE_y[10]) && (show_ll[10] == 1'd1)) | 
					 ((Pixel_x[9:4] == SANEK_ELSE_x[11]) && (Pixel_y[9:4] == SANEK_ELSE_y[11]) && (show_ll[11] == 1'd1)) | 
					 ((Pixel_x[9:4] == SANEK_ELSE_x[12]) && (Pixel_y[9:4] == SANEK_ELSE_y[12]) && (show_ll[12] == 1'd1)) | 
					 ((Pixel_x[9:4] == SANEK_ELSE_x[13]) && (Pixel_y[9:4] == SANEK_ELSE_y[13]) && (show_ll[13] == 1'd1)) | 
					 ((Pixel_x[9:4] == SANEK_ELSE_x[14]) && (Pixel_y[9:4] == SANEK_ELSE_y[14]) && (show_ll[14] == 1'd1)) | 
					 ((Pixel_x[9:4] == SANEK_ELSE_x[15]) && (Pixel_y[9:4] == SANEK_ELSE_y[15]) && (show_ll[15] == 1'd1)))
									
					snake_st = 	SANEK_ELSE ;
			else 
					snake_st = NOTHING;
		end
		else
			snake_st = NOTHING;
	end
	
endmodule
