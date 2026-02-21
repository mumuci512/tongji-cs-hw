module over_img_gen		
(
	input clk,
	input sys_retn,
	input done_wire,
	
	input [10:0] X_up,
	input [10:0] y_up,
	input [255:0] mm_dout,
	output [10:0] mm_addra,
	output over_show_red,
	output over_show_green,
	output over_show_blue
);
	reg [10:0] m;
	reg [10:0] n;
	always @ (posedge clk or negedge sys_retn)
	begin
		if(!sys_retn)
			begin
				m <= 11'd 0;
				n <= 11'd 0;
			end
		else
			begin//左边改的是整个图片的坐标，右边改的是像素点补偿
				n <= (done_wire && ((11'd0 + 11'd200) < y_up ) && (y_up < (11'd 256 + 11'd200)))?		(y_up [10:0] - 11'd200	)	:	11'd0;
				m <= (done_wire && ((11'd0 + 11'd100) < X_up ) && (X_up < (11'd 256 + 11'd100)))?				(X_up [10:0] - 11'd100		)	:	11'd0;		
			end	
	end
	
	assign mm_addra = m;
	
	assign over_show_red = (done_wire )?			(		(mm_dout [11'd 255 - n])?		1'b0	:		1'b0		)				:	1'd 0;
	assign over_show_green = (done_wire )?		(		(mm_dout [11'd 255 - n])?		1'b0	:		1'b0		)				:	1'd 0;

	assign over_show_blue = (done_wire )?	(	(mm_dout [11'd 255 - n])?		1'b1		:		1'b0		):	1'b0;

		
endmodule 