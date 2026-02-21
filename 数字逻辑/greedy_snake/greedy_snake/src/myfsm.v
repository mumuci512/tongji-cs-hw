module myfsm
(
	input pclk,
	input sys_retn,
	input bt_left,//左按键
	input bt_right,//右按键
	input bt_up,//上按键
	input bt_down,//下按键

	output reg  [2:0] cur_st,
	
	input infire,	
	input inwall,
	input inbody
		
);
	parameter   IDLE = 3'b001;
	parameter   GAMING = 3'b010;
	parameter   ST_DONE = 3'b100;

	always @ (posedge pclk or negedge sys_retn)
	begin
		if(!sys_retn)
			cur_st <= IDLE;
		else begin
			case(cur_st)//选择游戏状态
				IDLE:		  
				begin	
						if(bt_down)	
									cur_st <= GAMING;
	
						else
									cur_st <= IDLE;//这是为了避免生成锁存器
					 
				end
				GAMING:begin
						if(inwall | inbody | infire) 	
								cur_st <= ST_DONE;
						
						else
								cur_st <= GAMING;
				end

	
				ST_DONE: cur_st <= ST_DONE;
	
			endcase
		end
	end
	
	endmodule
	
