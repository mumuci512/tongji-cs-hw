module clk_div
(
	input Clk_in,
	input sys_retn,
	output  o_clk_div2,	
	output  o_clk_div4
);

	reg  clk_div1;
	reg  clk_div2;	

always @(posedge Clk_in or negedge sys_retn)begin
    if(!sys_retn)
        clk_div1 <= 1'b0;
    else  
        clk_div1 <=  ~clk_div1;
end


always @(posedge clk_div1 or negedge sys_retn)begin
    if(!sys_retn)
        clk_div2 <= 1'b0;
    else  
        clk_div2 <=  ~clk_div2;
end

assign  o_clk_div2 = clk_div1; 
assign  o_clk_div4 = clk_div2; 

endmodule
