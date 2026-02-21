module img_choose 
(	
	input GAMING_vs,
	input GAMING_hs,
	input GAMING_show_red,
	input GAMING_show_green,
	input GAMING_show_blue,

	input over_vs,
	input over_hs,
	input over_show_red,
	input over_show_green,
	input over_show_blue,
	
	input IDLE_vs,
	input IDLE_hs,
	input [4:0]  IDLE_show_red,
	input [5:0]  IDLE_show_green,
	input [4:0]  IDLE_show_blue,
	
	
	input [2:0] cur_st,//3种游戏状态，IDLE：001；GAMING：010；end：100；
	input Flash_over_sig,
	
	
	output vs_out,
	output hs_out,
	
	output [3:0] show_red_out,
	output [3:0] show_green_out,
	output [3:0] show_blue_out
);
	
	//3种游戏状态	
	localparam IDLE = 3'b001;
	localparam GAMING = 3'b010;
	localparam ST_DONE = 3'b100;
	
	
	
	
	reg vs;
	reg hs;
	reg [3:0] show_red;
	reg [3:0] show_green;
	reg [3:0] show_blue;	
	
	
	always @ (*)	
		begin
			case(cur_st)			
				IDLE:
					begin
						vs = IDLE_vs; 
						hs = IDLE_hs;
						show_red   = IDLE_show_red  [4:1] ;
						show_green = IDLE_show_green[5:2];
						show_blue  = IDLE_show_blue [4:1]; 									
					end							
				GAMING:
					begin
						vs = GAMING_vs; 
						hs = GAMING_hs;
						show_red   = {4{GAMING_show_red  }};
						show_green = {4{GAMING_show_green}};
						show_blue  = {4{GAMING_show_blue }}; 
					end	
				ST_DONE:
					begin
						vs = over_vs; 
						hs = over_hs;
						show_red   = {4{over_show_red  }};
						show_green = {4{over_show_green}} ;
						show_blue  = {4{over_show_blue }}; 
					end
				default:
					begin
						vs = IDLE_vs; 
						hs = IDLE_hs;
						show_red   = IDLE_show_red  [4:1] ;
						show_green = IDLE_show_green[5:2];
						show_blue  = IDLE_show_blue [4:1];						
					end
			endcase
		end
		
	assign	 vs_out = vs; 
	assign   hs_out = hs;
	assign	show_red_out   = show_red;
	assign	show_green_out = show_green;
	assign	show_blue_out  = show_blue; 		

endmodule					
					
						
									
