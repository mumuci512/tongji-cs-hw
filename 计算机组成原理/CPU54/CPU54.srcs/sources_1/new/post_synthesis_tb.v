`timescale 1ns / 1ps
module cpu_tb;
reg clk; // 时钟信号
reg rst; // 复位信号
wire [31:0] inst; // 要执行的指令
wire [31:0] pc; // 下一条指令的地址
wire clk_cpu;
//integer file_open;

initial
begin
    clk = 1'b0;
    rst = 1'b1;
    #50 rst = 1'b0;
end

always #50 clk = ~clk;

sccomp_dataflow uut(
    .clk_in(clk),
    .reset(rst),
    .inst(inst), // 输出指令
    .pc(pc) // 执行地址
    );
endmodule