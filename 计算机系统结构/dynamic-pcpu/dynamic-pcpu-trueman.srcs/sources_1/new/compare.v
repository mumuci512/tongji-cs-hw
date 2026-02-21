`include "mips_def.vh"
`timescale 1ns / 1ps

// 分支与跳转判断模块，根据指令类型和寄存器数据判断是否发生跳转
module compare(
    input           clk,
    input           rst,
    input [31:0]    rs_data, 
    input [31:0]    rt_data,
    input [5:0]     op,
    input [5:0]     func,
    input           cp0_exc,
    output reg      is_goto 
);
    
    // 组合逻辑判断是否跳转，覆盖各类跳转/分支指令与异常情况
    always@(*) 
    begin
        if(rst)
            is_goto <= 1'b0;
        // 处理无条件跳转指令
        else if(op == `OP_J)
            is_goto <= 1'b1;
        else if(op == `OP_JR && func == `FUNC_JR)
            is_goto <= 1'b1;
        else if(op == `OP_JAL)
            is_goto <= 1'b1;
        else if(op == `OP_JALR && func == `FUNC_JALR)
            is_goto <= 1'b1;
        // 处理条件分支与比较指令
        else if(op == `OP_BEQ) 
            is_goto <= (rs_data == rt_data);
        else if(op == `OP_BNE) 
            is_goto <= (rs_data != rt_data);
        else if(op == `OP_BGEZ) 
            is_goto <= (rs_data >= 0);            
        else if(op == `OP_TEQ && func == `FUNC_TEQ)
            is_goto <= (rs_data == rt_data);
        // 处理CP0异常触发的跳转
        else if(cp0_exc)
            is_goto <= 1'b1;
        else
            is_goto <= 1'b0;
    end
    
endmodule