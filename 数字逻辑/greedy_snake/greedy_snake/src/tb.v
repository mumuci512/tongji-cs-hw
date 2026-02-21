`timescale  1ns / 1ps

module tb_greedy_snake_top;

parameter CLK_PERIOD  = 10;
defparam DUT.bt_debU6.Tmax	= 2;

reg   clk                                  = 0 ;
reg   sys_retn                             = 0 ;
reg   [2:0]  sw                            = 0 ;
reg   [4:0]  key                            = 0 ;

// reg   BTNL                                 = 0 ;
// reg   BTNR                                 = 0 ;
// reg   BTND                                 = 0 ;
// reg   BTNU                                 = 0 ;

wire  vs_out                               ;
wire  hs_out                               ;
wire  [3:0]  vga_red_o                     ;
wire  [3:0]  vga_green_o                   ;
wire  [3:0]  vga_blue_o                    ;
wire  [7:0]  digtal7_seg                   ;
wire  [7:0]  digtal7_sel                   ;


initial
begin
    forever #(CLK_PERIOD/2)  clk=~clk;
end

initial
begin

    sw  = 3'b001 ; 
    #100 sys_retn  =  1; //复位失效，系统开始工作
#100000
     press_BTND();  //开始游戏

#100000
     press_BTNR();  

#100000
     press_BTND();  

#100000
     press_BTNL();  

#100000
     press_BTNU(); 

//=============================
#1000000 
    sys_retn  =  0;
    sw  = 3'b010; 
    #100 sys_retn  =  1; //复位失效，系统开始工作
#100000
     press_BTND();  //开始游戏

#100000
     press_BTNR();  

#100000
     press_BTND();  

#100000
     press_BTNL();  

//=============================
#1000000 
    sys_retn  =  0;
    sw  = 3'b100; 
    #100 sys_retn  =  1; //复位失效，系统开始工作
#100000
     press_BTND();  //开始游戏

#100000
     press_BTNL();  

#100000
     press_BTND();  

#100000
     press_BTNU();  



#10000  $stop;   //停止仿真
end

greedy_snake_top  DUT (
    .clk                     ( clk                ),
    .sys_retn                ( sys_retn           ),
    .sw                      ( sw           [2:0] ),
    .BTNL                    ( key[0]               ),
    .BTNR                    ( key[1]               ),
    .BTND                    ( key[2]               ),
    .BTNU                    ( key[3]               ),

    .vs_out                  ( vs_out             ),
    .hs_out                  ( hs_out             ),
    .vga_red_o               ( vga_red_o    [3:0] ),
    .vga_green_o             ( vga_green_o  [3:0] ),
    .vga_blue_o              ( vga_blue_o   [3:0] ),
    .digtal7_seg             ( digtal7_seg  [7:0] ),
    .digtal7_sel             ( digtal7_sel  [7:0] )
);



//===========BTNL键按下================
task press_BTNL();
begin 
#100 key = 4'b0000;
 @ (posedge clk)
 #1	key = 4'b0001;
  #(10*CLK_PERIOD)
 	key = 4'b0000;
end	
endtask
//===========BTNR键按下================
task press_BTNR();
begin 
#100 key = 4'b0000;
 @ (posedge clk)
 #1	key = 4'b0010;
  #(10*CLK_PERIOD)
 	key = 4'b0000;
end	
endtask
//===========BTND键按下================
task press_BTND();
begin 
#100 key = 4'b0000;
 @ (posedge clk)
 #1	key = 4'b0100;
 #(10*CLK_PERIOD)
 	key = 4'b0000;
end	
endtask
//===========BTNU键按下================
task press_BTNU();
begin 
#100 key = 4'b0000;
 @ (posedge clk)
 #1	key = 4'b1000;
 #(10*CLK_PERIOD)
 	key = 4'b0000;
end	
endtask
endmodule