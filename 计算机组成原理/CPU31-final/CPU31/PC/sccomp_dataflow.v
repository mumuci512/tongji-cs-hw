`timescale 1ns / 1ps
module sccomp_dataflow(
    input clk_in,       //时钟信号
    input reset,        //复位信号
    output [31:0] inst, //输出指令
    output [31:0] pc    //执行地址
    );
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
endmodule