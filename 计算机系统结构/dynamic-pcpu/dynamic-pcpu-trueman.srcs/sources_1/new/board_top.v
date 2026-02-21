`timescale 1ns / 1ps

// 顶层板级模块，集成CPU、数码管显示与时钟分频功能
module board_top(
    input           clk,
    input           rst,
    input           ena,
    input  [2:0]    switch,
    output [7:0]    o_seg,
    output [7:0]    o_sel
    );

    wire  [31:0] pc;
    wire [31:0] inst;
    wire clk_cpu;
    wire clk_out;
    reg [20:0]  clk_div;
    wire [31:0] display_data;

// 验证程序数组
    wire [31:0] a;
    wire [31:0] b;
    wire [31:0] c;
    wire [31:0] d;
    
    // 时钟分频计数器累加
    always@(posedge clk)
        clk_div = clk_div + 1;
            
    assign clk_cpu = clk;               //   仿真
//    assign clk_cpu = clk_out;       //  下板慢
//    assign clk_cpu = clk_div[19];       // 下板快

    // 根据开关选择待显示的数据
    Mux8To1_32Bit mux_display(pc, inst, 32'b0,32'b0,a,b,c,d, switch, display_data);

    // 数码管显示驱动实例化
    seg7x16 seg7x16_inst(
        .clk(clk),
        .reset(rst),
        .cs(1'b1),
        .i_data(display_data),
        .o_seg(o_seg),
        .o_sel(o_sel)
        );

    // CPU模块实例化
    cpu cpu_inst(clk_cpu, rst, ena, pc, inst, a,b,c,d);

    // 时钟分频器模块实例化
    Divider Divider_inst(
            .clk(clk), 
            .rst_n(rst), 
            .clk_out(clk_out)
            );

endmodule