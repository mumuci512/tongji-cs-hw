
module greedy_snake_top//贪吃蛇顶层模块
(
	input clk,
	input sys_retn,	
	input [2:0] sw ,
	
	input BTNL,
	input BTNR,
	input BTND,
	input BTNU,

	output vs_out,
	output hs_out,
	output [3:0] vga_red_o  ,
	output [3:0] vga_green_o,
	output [3:0] vga_blue_o ,

	input ps2k_clk,	//PS2接口时钟信号
	input ps2k_data,	//PS2接口数据信号	
	
    inout         TMP_SDA,          // i2c sda on temp sensor - bidirectional
    output        TMP_SCL,          // i2c scl on temp sensor
	
	output [7:0] digtal7_seg,//数码管段选
	output [7:0] digtal7_sel//数码管位选
	
);


	
	reg  [31:0]  PTMAX;
		always@(*)
		case(sw)
			3'b001:PTMAX <=  32'd12_500_000;
			3'b010:PTMAX <=  32'd6_000_000;
			3'b100:PTMAX <=  32'd3_500_000;
			default:PTMAX <=  32'd12_500_000;
		endcase
		

	wire bt_left;//左按键
	wire bt_right;//右按键
	wire bt_up;//上按键
	wire bt_down;//下按键

	reg ps_left;//左按键
	reg ps_right;//右按键
	reg ps_up;//上按键
	reg ps_down;//下按键

	
	wire [2:0] cur_st;
	
	wire inwall;
	wire inbody;

	wire GAMING_show_red;
	wire GAMING_show_green;
	wire GAMING_show_blue;
	
	wire GAMING_hs;
	wire GAMING_vs;

	
	wire [5:0] pill_x;//苹果的X坐标
	wire [4:0] pill_y;//苹果的Y坐标

	wire [5:0] fist_x;//蛇头部的X坐标
	wire [5:0] fist_y;//蛇头部的Y坐标

	wire growup;
	
	wire [1:0] snake_st;//对象
	
	wire [9:0] Pixel_x;//扫描的像素的X坐标
	wire [9:0] Pixel_y;//扫描的像素的Y坐标
	

	wire pclk,clk50m;	
	
clk_div 	clk_divU1 
	(
		.Clk_in(clk),
		.sys_retn(sys_retn),
		.o_clk_div2(clk50m),		
		.o_clk_div4(pclk)
	);
	
	
wire[7:0] ps2_byte;	// 1byte键值
wire ps2_state;		//按键状态标志位
reg   [7:0] ps2_data; 	


ps2scan			ps2scan_u(	
		.clk(clk50m),			  	//按键扫描模块
		.rst_n(sys_retn),				
		.ps2k_clk(ps2k_clk),
		.ps2k_data(ps2k_data),
		.ps2_byte(ps2_byte),
		.ps2_state(ps2_state)
);

ila_0 ila_0_ins(
	.clk(clk50m), // input wire clk


	.probe0(ps2k_clk), // input wire [0:0]  probe0  
	.probe1(ps2k_data), // input wire [0:0]  probe1 
	.probe2(ps2_state), // input wire [0:0]  probe2 
	.probe3(ps2_byte ) // input wire [7:0]  probe3
);


	

	always@(posedge clk50m or negedge sys_retn)
	if(!sys_retn)
		ps2_data <= 8'h79;
	else if(ps2_state == 1'b1) 
		ps2_data <= ps2_byte;
	else
		ps2_data <= ps2_data;
		
		
	reg ps_left  ;  //左按键
	reg ps_right ;//右按键
	reg ps_up    ;    //上按键
	reg ps_down  ;//下按键
	wire  flag ;
clk_flag			clk_flag_u(	
		.clk(pclk),			  	//按键扫描模块
		.rst_n(sys_retn),				
		.flag(flag)
);			


	
always@(*)
case(ps2_data)
	8'h1C:begin  ps_left <= flag;  ps_right <= 1'b0;   ps_up <= 1'b0;   ps_down <= 1'b0;   end
	8'h23:begin  ps_left <= 1'b0;  ps_right <= flag;   ps_up <= 1'b0;   ps_down <= 1'b0;   end
	8'h1B:begin  ps_left <= 1'b0;  ps_right <= 1'b0;   ps_up <= flag;   ps_down <= 1'b0;   end
	8'h1D:begin  ps_left <= 1'b0;  ps_right <= 1'b0;   ps_up <= 1'b0;   ps_down <= flag;   end
	default:  ;
endcase	


		
	    wire w_200KHz;                  // 200kHz SCL
    wire [7:0] c_data;              // 8 bits of Celsius temperature data
   // wire [7:0] f_data;              // 8 bits of Fahrenheit temperature data

    // Instantiate i2c master
    i2c_master i2cmaster(
        .clk_200KHz(w_200KHz),
        .temp_data(c_data),
        .SDA(TMP_SDA),
        .SCL(TMP_SCL)
    );
    
    // Instantiate 200kHz clock generator
    clkgen_200KHz clkgen(
        .clk_100MHz(clk),
        .clk_200KHz(w_200KHz)
    );
    


	

bt_deb	 bt_debU6//按键检测模块
	(
		.pclk(pclk),
		.sys_retn(sys_retn),
		
		.Left  (~BTNL),
		.Right (~BTNR),
		.Up    (~BTNU),
		.Down  (~BTND),
		
		.bt_left(bt_left),
		.bt_right(bt_right),
		.bt_up(bt_up),
		.bt_down(bt_down)		
	);

reg   infire; 
	always@(posedge pclk or negedge sys_retn)
	if(!sys_retn)
		infire <= 1'd0;
	else if(c_data > 8'd19)  
		infire <= 1'd1;
	else
		infire <= 1'd0;



	
myfsm	 myfsmU2
	(
		.pclk(pclk),
		.sys_retn(sys_retn),
		
		.bt_left   (bt_left  | ps_left     ),
		.bt_right  (bt_right | ps_right    ),
		.bt_up     (bt_up    | ps_up       ),
		.bt_down   (bt_down  | ps_down     ),
		.cur_st(cur_st),
		
		.infire(infire),		
		.inwall(inwall),
		.inbody(inbody)
		
	);
		


pill_gen	 pill_genU3//产生食物
	(
		.pclk(pclk),
		.sys_retn(sys_retn),
		
		.pill_x(pill_x),
		.pill_y(pill_y),
		
		.fist_x(fist_x),
		.fist_y(fist_y),
		
		.growup(growup)

		);


moving	movingU4//运动控制
	(
		.pclk(pclk),
		.sys_retn(sys_retn),
		.PTMAX(PTMAX),	
		.bt_GOLEFT(bt_left),
		.bt_GORIGHT(bt_right),
		.bt_GOUP(bt_up),
		.bt_GODOWN(bt_down),
		
		.snake_st(snake_st),
		
		.Pixel_x(Pixel_x),
		.Pixel_y(Pixel_y),
		
		.fist_x(fist_x),
		.fist_y(fist_y),
		
		.growGOUP(growup),
		.cur_st(cur_st),
		
		.inSANEK_ELSE(inbody),
		.inFOURWALL(inwall)
	);
	

vga_show	 vga_showU5//VGA控制模块
	(
		.pclk(pclk),
		.sys_retn(sys_retn),
		
		.hs(GAMING_hs),
		.vs(GAMING_vs),
		
		.snake_st(snake_st),
		
		.GAMING_show_red(GAMING_show_red),
		.GAMING_show_green(GAMING_show_green),
		.GAMING_show_blue(GAMING_show_blue),
		
		.Pixel_x(Pixel_x),
		.Pixel_y(Pixel_y),
		
		.pill_x(pill_x),
		.pill_y(pill_y)
	);


    // Binary to BCD conversion of temperature data
    wire [3:0] c_tens, c_ones;
    assign c_tens = c_data / 10;           // Tens value of C temp data
    assign c_ones = c_data % 10;           // Ones value of C temp data
	

hex	 hexU7
	(
		.pclk(pclk),
		.sys_retn(sys_retn),	
		
		.temp_data(  { c_tens,c_ones  }   ),		
		
		.ps2_data(ps2_data   ),		 //  ps2_data  {  24'h123456,ps2_data}  
		.growup(growup),
		.cur_st(cur_st),
		
		.digtal7_seg(digtal7_seg),
		.digtal7_sel(digtal7_sel)
	);
	
	
img_top		img_topU8
(
	.clk(pclk),
	.sys_retn(sys_retn),

	.cur_st(cur_st),
	
	.GAMING_show_red(GAMING_show_red),
	.GAMING_show_green(GAMING_show_green),
	.GAMING_show_blue(GAMING_show_blue),
	.GAMING_hs(GAMING_hs),
	.GAMING_vs(GAMING_vs),
	
	
	.vs_out(vs_out),
	.hs_out(hs_out),
	.show_red_out  (vga_red_o  ),
	.show_green_out(vga_green_o),
	.show_blue_out (vga_blue_o  )
);
	
	
	
	
	
	
endmodule
