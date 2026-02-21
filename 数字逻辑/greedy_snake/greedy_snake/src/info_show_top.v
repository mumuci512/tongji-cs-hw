
module info_show_top(
    input           pclk,        //系统时钟
    input           sys_retn,      //复位信号
    //VGA接口                          


  	output 			[4:0]  info_show_blue  ,
  	output 			[5:0]  info_show_green ,	
  	output 			[4:0]  info_show_red   ,	
	
	output 			info_hs,
	output 			info_vs
	
    ); 

wire    [15:0]  vga_rgb;   
wire [15:0]  p_data;          //像素点数据
wire [ 9:0]  p_xpit;          //像素点横坐标
wire [ 9:0]  p_ypit;          //像素点纵坐标    
    

assign     info_show_blue    = vga_rgb[4:0]  ;
assign     info_show_green  = vga_rgb[10:5]  ;   
assign     info_show_red    = vga_rgb[15:11]  ;

info_driver u_info_driver(
    .pclk        (pclk),    
    .sys_retn      (sys_retn),    

    .vga_hs         (info_hs),       
    .vga_vs         (info_vs),       
    .vga_rgb        (vga_rgb),      
    
    .p_data     (p_data), 
    .p_xpos     (p_xpit), 
    .p_ypos     (p_ypit)
    ); 
    
info_display  u_info_display(
    .pclk        (pclk),
    .sys_retn      (sys_retn),
    
    .pixel_xpos     (p_xpit),
    .pixel_ypos     (p_ypit),
    .pixel_data     (p_data)
    );   
    
endmodule 

    