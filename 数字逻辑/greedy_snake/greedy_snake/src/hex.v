module hex//数码管计分显示模块
(
	input pclk,
	input sys_retn,
	
	input growup,//身体增长信号
	input [2:0] cur_st,//3种游戏状态，IDLE：001；GAMING：010；end：100；
	
	input [7:0] ps2_data,	
	input [7:0] temp_data,		
	
	output reg [7:0] digtal7_seg,//数码管段选
	output     [7:0] digtal7_sel//数码管位选
);


	reg [15:0] score;//分数，用来显示在数码管
	wire [31:0]disp_data;
	
	assign  disp_data ={ score , 8'haa , temp_data} ;
	
	//assign  disp_data =  ps2_data ;
	
	
	reg [14:0] divider_cnt;//25000-1
	
	reg clk_1K;
	reg [7:0]digtal7_sel_r;
	
	reg [4:0]data_tmp;//数据缓存 ;data_tmp[4]用来控制DP显示
	
	
	reg get_pill;//吃苹果信号
	
//	分频计数器计数模块
	always@(posedge pclk or negedge sys_retn)
	if(!sys_retn)
		divider_cnt <= 15'd0;
	else if(divider_cnt == 12500)  //加快仿真
		divider_cnt <= 15'd0;
	else
		divider_cnt <= divider_cnt + 1'b1;

//1K扫描时钟生成模块		
	always@(posedge pclk or negedge sys_retn)
	if(!sys_retn)
		clk_1K <= 1'b0;
	else if(divider_cnt ==   12500)  //加快仿真
		clk_1K <= ~clk_1K;
	else
		clk_1K <= clk_1K;
		
//8位循环移位寄存器
	always@(posedge clk_1K or negedge sys_retn)
	if(!sys_retn)
		digtal7_sel_r <= 8'b0000_0001;
	else if(digtal7_sel_r == 8'b1000_0000)
		digtal7_sel_r <= 8'b0000_0001;
	else
		digtal7_sel_r <=  digtal7_sel_r << 1;
		
	always@(*)
		case(digtal7_sel_r)
			8'b0000_0001:data_tmp = {1'b1, disp_data[3:0]};
			8'b0000_0010:data_tmp = {1'b1, disp_data[7:4]};
			8'b0000_0100:data_tmp = {1'b1, disp_data[11:8]};
			8'b0000_1000:data_tmp = {1'b1, disp_data[15:12]};
			8'b0001_0000:data_tmp = {1'b1, disp_data[19:16]};
			8'b0010_0000:data_tmp = {1'b1, disp_data[23:20]};
			8'b0100_0000:data_tmp = {1'b1, disp_data[27:24]};
			8'b1000_0000:data_tmp = {1'b1, disp_data[31:28]};
			default:data_tmp = 5'b00000;
		endcase
		
	always@(*) begin
		case(data_tmp[3:0])
			4'h0:digtal7_seg[6:0] = 7'b1000000; /*0*/
			4'h1:digtal7_seg[6:0] = 7'b1111001; /*1*/
			4'h2:digtal7_seg[6:0] = 7'b0100100; /*2*/
			4'h3:digtal7_seg[6:0] = 7'b0110000; /*3*/
			4'h4:digtal7_seg[6:0] = 7'b0011001; /*4*/
			4'h5:digtal7_seg[6:0] = 7'b0010010; /*5*/
			4'h6:digtal7_seg[6:0] = 7'b0000010; /*6*/
			4'h7:digtal7_seg[6:0] = 7'b1111000; /*7*/
			4'h8:digtal7_seg[6:0] = 7'b0000000; /*8*/
			4'h9:digtal7_seg[6:0] = 7'b0010000; /*9*/
		//	4'ha:digtal7_seg[6:0] = 7'b0001000; /*a*/
			4'ha:digtal7_seg[6:0] = 7'b0111111; /*a*/			
			4'hb:digtal7_seg[6:0] = 7'b0000011; /*b*/
			4'hc:digtal7_seg[6:0] = 7'b1000110; /*c*/
			4'hd:digtal7_seg[6:0] = 7'b0100001; /*d*/
			4'he:digtal7_seg[6:0] = 7'b0000110; /*e*/
			
			4'hf:digtal7_seg[6:0] = 7'b1111111; /* */
		endcase
		digtal7_seg[7] = data_tmp[4];	/*用来控制小数点*/
	end
	
	assign digtal7_sel = ~digtal7_sel_r ;




	
	always @ (posedge pclk or negedge sys_retn)
	begin
		if(!sys_retn)
		begin
			score <= 16'd0;
			get_pill <= 1'd0;
		end
		else if(cur_st == 3'b100)
				score <= 16'd0;
		else 
		begin
			case(get_pill)
				1'd0		:
				begin
					if(growup)
					begin
						if(score[3:0] < 4'd9)//1
							score[3:0] <= score[3:0] + 4'd1;
						else
						begin
							score[3:0] <= 4'd0;//10
							if(score[7:4] < 4'd9)
								score[7:4] <= score[7:4] + 4'd1;
							else
							begin
								score[7:4] <= 4'd0;
								if(score[11:8] < 4'd9)
									score[11:8] <= score[11:8] + 4'd1;
								else 
								begin
									score[11:8] <= 4'd0;//100
									score[15:12] <= score[15:12] + 4'd1;
								end
							end
						end								
						get_pill <= 1'd1;
					end
				end
				1'd1:
					if(!growup)
						get_pill <= 1'd0;	
			endcase
		end							
	end	
		
endmodule
