
module clk_flag (
	input         clk,
	input         rst_n,
	output		  flag
);

	parameter T_DIV	= 12_000_000;
   // parameter    T_DIV = 5     ;        //  仿真使用，5 加快仿真速度
	
	reg [25:0] cnt;
	reg        cnt_1s;  //cnt_1s = cnt * 2
    
	always @ (posedge clk, negedge rst_n)
	if (rst_n == 1'b0)
		cnt <= 25'd0;
	else if (cnt < T_DIV - 1'b1)
		cnt <= cnt + 1'b1;
	else
		cnt <= 25'd0;
	
	assign flag = (cnt == T_DIV - 1'b1) ? 1'b1 : 1'b0;

    endmodule   