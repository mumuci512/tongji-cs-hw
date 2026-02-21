
module info_display(
    input             pclk,              //VGA驱动时钟
    input             sys_retn,            //复位信号
    
    input      [ 9:0] pixel_xpos,           //像素点横坐标
    input      [ 9:0] pixel_ypos,           //像素点纵坐标    
    output   reg  [15:0] pixel_data            //像素点数据
    );    

                                
//640*480            

//589*339
localparam PIC_X_START = 10'd170 ;      //图片起始点横坐标
localparam PIC_Y_START = 10'd79;      //图片起始点纵坐标
localparam PIC_WIDTH   = 10'd300;    //图片宽度
localparam PIC_HEIGHT  = 10'd300;    //图片高度
  
localparam PIC_X_START2 = 10'd0;      //图片起始点横坐标
localparam PIC_Y_START2 = 10'd380;      //图片起始点纵坐标
localparam PIC_WIDTH2   = 10'd640;    //图片宽度
localparam PIC_HEIGHT2  = 10'd100;    //图片高度
                       
localparam BACK_COLOR  = 16'b00000_111111_00000;


always @(posedge pclk or negedge sys_retn) begin
    if (!sys_retn)
        pixel_data <= BACK_COLOR;
    else
        pixel_data <= BACK_COLOR;        //屏幕背景色
end





endmodule 