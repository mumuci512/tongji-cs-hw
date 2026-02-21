`timescale 1ns / 1ps
module sccomp_dataflow(
    input clk_in,       //时钟信号
    input reset,        //复位信号
    //output [31:0] pc,
    //output [31:0] inst,
    output [7:0]  o_seg,//输出内容
    output [7:0]  o_sel //片选信号
    ); 
wire  [31:0] pc;
wire [31:0] inst;
wire clk_cpu;
/* IMEM用 */
wire [31:0] imem_in;

/* DMEM用 */
wire cs;
wire dm_w;
wire dm_r;
wire [31:0] dm_addr;
wire [31:0] dm_data_w;
wire [31:0] dm_data_r;


imem imem(
    .a(imem_in[12:2]),  //11位指令码地址，从IMEM中读指令
    .spo(inst)     //32位指令码
    );
    
DMEM dmem (
        .clk(clk_in),
        .cs(cs),
        .dm_w(dm_w),
        .dm_r(dm_r),
        .addr(dm_addr), 
        .data_in(dm_data_w),
        .data_out(dm_data_r)
    );

cpu sccpu(
    .clk(clk_in),
    .rst(reset),    
    .inst(inst),
    .imem_in(imem_in),
    .cs(cs),
    .dm_w(dm_w),
    .dm_r(dm_r),
    .dm_addr(dm_addr),
    .dm_data_w(dm_data_w),
    .dm_data_r(dm_data_r),
    .pc_out(pc)
    );

seg7x16 seg7x16_inst(
    .clk(clk_in),
	.reset(reset),
	.cs(1'b1),
	.i_data(inst),
	.o_seg(o_seg),
	.o_sel(o_sel)
    );

Divider Divider_inst(
    .clk(clk_in),                   //系统时钟
    .rst_n(reset),                 //复位信号
    .clk_out(clk_cpu)               //输出适配CPU的时钟
    );

endmodule
