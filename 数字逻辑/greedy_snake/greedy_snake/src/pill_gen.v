module pill_gen
(
	input pclk,//50MHZ的时钟
	input sys_retn,//全局复位
	
	input [5:0] fist_x,//蛇头的X坐标
	input [5:0] fist_y,//蛇头的Y坐标
	
	output reg [5:0] pill_x,//苹果的X坐标
	output reg [4:0] pill_y,//苹果的Y坐标

	output reg growup//蛇的身体增长信号
);


	reg [31:0] piill_cnt;
	reg [10:0] rdata;//高6位为X坐标 低5位为Y坐标
	
	always@(posedge pclk)
	begin	
		rdata <= rdata + 11'd842;  
	end												
	
	always@(posedge pclk or negedge sys_retn)
	begin
		if(!sys_retn)
		begin
			piill_cnt <= 32'd0;
			
			pill_x <= 6'd24;
			pill_y <= 5'd12;
			
			growup <= 1'd0;//0表示不增加身体节数
		end
		else	if(piill_cnt == 32'd249999)//0.5秒计时
				begin
					piill_cnt <= 32'd0;
					if((pill_x == fist_x) && (pill_y == fist_y))
					begin
						growup <= 1'd1;
						pill_x <= (rdata[10:5]> 6'd38)?		(rdata[10:5] - 6'd25)		:		((rdata[10:5] == 6'd0)?		6'd1		:		rdata[10:5]);
						pill_y <= (rdata[4:0] > 5'd28)?		(rdata[4:0] - 5'd3)		:		((rdata[4:0] == 5'd0)?		5'd1		:		rdata[4:0]);
					end   
					else
						growup <= 1'd0;
				end
		else
			piill_cnt <= piill_cnt + 32'd1;
	end
endmodule
