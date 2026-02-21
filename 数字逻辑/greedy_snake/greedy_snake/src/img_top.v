module img_top
(
	input clk,
	input sys_retn,

	input [2:0] cur_st,//3种游戏状态，IDLE：001；GAMING：010；end：100；
//	input Flash_over_sig,
	
	
	input GAMING_show_red,
	input GAMING_show_green,
	input GAMING_show_blue,
	input GAMING_hs,
	input GAMING_vs,
	
	
	
	output vs_out,
	output hs_out,
	output [3:0]show_red_out,
	output [3:0]show_green_out,
	output [3:0]show_blue_out
);

	
	
	
	

	wire IDLE_hs;	
	wire IDLE_vs;	
	wire [4:0] IDLE_show_red;	
	wire [5:0] IDLE_show_green;
	wire [4:0] IDLE_show_blue;
	
	wire over_vs;
	wire over_hs;
	wire over_show_red;
	wire over_show_blue;
	wire over_show_green;
	


info_show_top	info_show_top_inst		
(										
	.pclk(clk),							
	.sys_retn(sys_retn),				
	.info_show_green(IDLE_show_green),	
	.info_show_blue(IDLE_show_blue),	
	.info_show_red(IDLE_show_red),
	.info_hs(IDLE_hs),
	.info_vs(IDLE_vs)
);

over_show_top		over_show_top_inst			
(												
	.clk(clk),									
	.sys_retn(sys_retn),	
	
	.over_show_green(over_show_green),			
	.over_show_blue(over_show_blue),			
	.over_show_red(over_show_red),
	.over_hs(over_hs),
	.over_vs(over_vs)
);
	

	
img_choose 	img_choose_inst
(

	.GAMING_vs(GAMING_vs),
	.GAMING_hs(GAMING_hs),
	.GAMING_show_red(GAMING_show_red),
	.GAMING_show_green(GAMING_show_green),
	.GAMING_show_blue(GAMING_show_blue),

	.over_vs(over_vs),
	.over_hs(over_hs),
	.over_show_red(over_show_red),
	.over_show_green(over_show_green),
	.over_show_blue(over_show_blue),
	
	.IDLE_show_green(IDLE_show_green),			
	.IDLE_show_blue(IDLE_show_blue),		
	.IDLE_show_red(IDLE_show_red),
	.IDLE_hs(IDLE_hs),
	.IDLE_vs(IDLE_vs),
	

	.cur_st(cur_st),
	
	.vs_out(vs_out),
	.hs_out(hs_out),
	
	.show_red_out(show_red_out),
	.show_green_out(show_green_out),
	.show_blue_out(show_blue_out)
);
	
	
		

	
	
endmodule
