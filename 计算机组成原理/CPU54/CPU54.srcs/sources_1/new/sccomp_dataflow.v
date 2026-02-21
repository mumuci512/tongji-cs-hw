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
wire sw_flag;
wire sb_flag;
wire sh_flag;
wire lb_flag;               //当前读取指令是否是LB发出
wire lh_flag;               //当前读取指令是否是LH发出
wire lbu_flag;              //当前读取指令是否是LBU发出
wire lhu_flag;              //当前读取指令是否是LHU发出
wire lw_flag;               //当前读取指令是否是LW发出

imem imem(
    .a(imem_in[10:0]),  //11位指令码地址，从IMEM中读指令
    .spo(inst)     //32位指令码
    );
    
DMEM dmem (
        .clk(clk_in),
        .cs(cs),
        .dm_w(dm_w && !busy),
        .dm_r(dm_r && !busy),
        .addr(dm_addr), 
        .data_in(dm_data_w),
        .sb_flag(sb_flag),              //当前写入指令是否是SB发出
        .sh_flag(sh_flag),              //当前写入指令是否是SH发出
        .sw_flag(sw_flag),              //当前写入指令是否是SW发出
        .lb_flag(lb_flag),              //当前读取指令是否是LB发出
        .lh_flag(lh_flag),              //当前读取指令是否是LH发出
        .lbu_flag(lbu_flag),            //当前读取指令是否是LBU发出
        .lhu_flag(lhu_flag),            //当前读取指令是否是LHU发出
        .lw_flag(lw_flag),              //当前读取指令是否是LW发出
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
    .pc_out(pc),
    .busy(busy),
    .sw_flag(sw_flag),
    .sb_flag(sb_flag),
    .sh_flag(sh_flag),
    .lb_flag(lb_flag),              //当前读取指令是否是LB发出
    .lh_flag(lh_flag),              //当前读取指令是否是LH发出
    .lbu_flag(lbu_flag),            //当前读取指令是否是LBU发出
    .lhu_flag(lhu_flag),            //当前读取指令是否是LHU发出
    .lw_flag(lw_flag)               //当前读取指令是否是LW发出
    );
endmodule