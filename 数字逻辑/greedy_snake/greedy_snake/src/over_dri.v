module over_dri		//vga同步模块
(
	input clk,
	input sys_retn,
	output over_hs,		//列同步信号
	output over_vs,		//行同步信号
	output done_wire,		
	output [10:0] X_up,	
	output [10:0] y_up	
);


	reg [10:0] Count_hsync;		
	always @ (posedge clk or negedge sys_retn)
	begin
		if(!sys_retn)
			Count_hsync <= 11'd 0;
		else 
			 if(Count_hsync == 11'd 800)
				Count_hsync <= 11'd 0;
			 else	
				Count_hsync <= Count_hsync + 1'd 1;	
	end
	
	//**************************************************************
	reg [10:0] Count_vsync;		//行同步计数寄存器，628个像素就清零（时钟已经经过倍频，现在是一个时钟周期计算一个像素）
	always @ (posedge clk or negedge sys_retn)
	begin
		if(!sys_retn)
			Count_vsync <= 11'd 0;
		else 
			if(Count_vsync == 11'd 525)
				Count_vsync <= 11'd 0;
			else	if(Count_hsync == 11'd 800) 		//每当列像素达到1056（证明已经扫过一行了），行计数器加一~
						Count_vsync <= Count_vsync + 1'd 1;					
	end
	
	//**************************************************************
	reg r_Ready_sig;		//有效区域信号寄存器，800*600区域的标志位~
	always @ (posedge clk or negedge sys_retn)
	begin
		if(!sys_retn)
			r_Ready_sig <= 1'd 0;
		else	if((Count_hsync > (11'd 144 + 11'd200)) && (Count_hsync < (11'd 784 - 11'd184))		//这里是对区域进行判断：当216<列<1016,27<行<627时（即800*600），标志位为1，否则为0.
					 && (Count_vsync > (11'd 35 + 11'd93)) && (Count_vsync < (11'd 515 - 11'd133)))
					r_Ready_sig <= 1'd 1;
				else
					r_Ready_sig <= 1'd 0;		
	end
	
	assign done_wire = r_Ready_sig;		//连续赋值给over_Ready_sig
	
	//**************************************************************
	reg [10:0] rRow_add;		//行地址操作寄存器
	reg [10:0] rColumn_add;		//列地址操作寄存器
	always @ (posedge clk or negedge sys_retn)
	begin
		if(!sys_retn)
			begin
				rRow_add <= 11'd 0;
				rColumn_add <= 11'd 0;
			end
		else	
			begin
				rColumn_add <= (r_Ready_sig)?		Count_hsync - 11'd 144   :	 11'd 0;		
				rRow_add <= (r_Ready_sig)?	 Count_vsync - 11'd 27	 :	 11'd 0; 		
			end
	end
	
	assign X_up = rRow_add;
	assign y_up = rColumn_add;
	
	//**************************************************************
	reg r_hs;		//列信号
	reg r_vs;		//行信号
	always @ ( posedge clk or negedge sys_retn )
	begin
		if(!sys_retn)
			begin
				r_hs <= 1'd 0;
				r_vs <= 1'd 0;
			end
		else
			begin
				r_vs <= (Count_vsync <= 11'd 2)?		1'd 0	:	1'd 1;		
				r_hs <= (Count_hsync <= 11'd 96)? 	1'd 0 	:	1'd 1; 		
			end
	end
	
	assign over_hs = r_hs;
	assign over_vs = r_vs;
	
endmodule 