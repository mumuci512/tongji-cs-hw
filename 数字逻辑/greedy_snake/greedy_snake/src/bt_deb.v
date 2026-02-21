module bt_deb//按键消抖
(	
	input pclk,
	input sys_retn,
	
	input Left,//左按键
	input Right,//右按键
	input Up,//上按键
	input Down,//下按键
	
	output reg bt_left,
	output reg bt_right,
	output reg bt_up,
	output reg bt_down
);

parameter  Tmax= 32'd5_0000;
	reg [31:0] cnter;
	
	reg bt_left2;//左按键寄存器2，用来跟寄存比较的
	reg bt_right2;//右按键寄存器2
	reg bt_up2;//上按键寄存器2
	reg bt_down2;//下按键寄存器2
	
	always @ (posedge pclk or negedge sys_retn)
	begin
		if(!sys_retn)
		begin
			cnter <= 32'd0;
			
			bt_left <= 1'd0;
			bt_right <= 1'd0;
			bt_up <= 1'd0;
			bt_down <= 1'd0;
			
			bt_left2 <= 1'd0;
			bt_right2 <= 1'd0;
			bt_up2 <= 1'd0;
			bt_down2 <= 1'd0;						
		end
		else 
		begin
			if(cnter == Tmax)
			begin
				cnter <= 0;
				
				bt_left2 <= Left;
				bt_right2 <= Right;
				bt_up2 <= Up;
				bt_down2 <= Down;

				if((bt_left2 == 1'd1) && (Left == 1'd0))
					bt_left <= 1'd1;
				if((bt_right2 == 1'd1) && (Right == 1'd0))
					bt_right <= 1'd1;
				if((bt_up2 == 1'd1) && (Up == 1'd0))
					bt_up <= 1'd1;
				if((bt_down2 == 1'd1) && (Down == 1'd0))
					bt_down <= 1'd1;	
			end
			else
			begin
				cnter <= cnter + 32'd1;
				bt_left <= 1'd0;
				bt_right <= 1'd0;
				bt_up <= 1'd0;
				bt_down <= 1'd0;
			end
		end	
	end				

endmodule
