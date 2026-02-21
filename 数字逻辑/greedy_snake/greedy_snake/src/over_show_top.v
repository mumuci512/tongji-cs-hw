module over_show_top		           
(								
	input clk,				
	input sys_retn,					
	output over_show_green,		
	output over_show_blue,		
	output over_show_red,
	output over_hs,
	output over_vs
);


	wire [10:0] X_up;
	wire [255:0] mm_dout;
	wire [10:0] mm_addra; 	
	wire [10:0] y_up;
	wire done_wire;	
	
over_dri	U2
	(
		.clk(clk),
		.sys_retn(sys_retn),
		.over_hs(over_hs),
		.over_vs(over_vs),
		.done_wire(done_wire),
		.X_up(X_up),
		.y_up(y_up)
	);


over_img_gen	U3
	(
		.clk(clk),
		.sys_retn(sys_retn),
		.done_wire(done_wire),
		.X_up(X_up),
		.y_up(y_up),
		.over_show_red(over_show_red),
		.over_show_green(over_show_green),
		
		.mm_addra(mm_addra),
		.mm_dout(mm_dout),
		
		.over_show_blue(over_show_blue)
	);


over_rom	U4 
	(
		.addra(mm_addra),
		.clka(clk),
		.douta(mm_dout)
	);


endmodule 