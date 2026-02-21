`timescale 1ns / 1ps
module Regfiles (
    input clk,                // 时钟信号，下降沿写入数据
    input rst,                // 异步复位信号，高电平时全部寄存器置零
    input we,                 // 写使能信号，高电平允许写入
    input [4:0] RdC,          // Rd的地址（写入）
    input [4:0] RtC,          // Rt的地址（输出）
    input [4:0] RsC,          // Rs的地址（输出）
    input [31:0] Rd_data_in,  // 要写入的值
    output [31:0] Rs_data_out,// Rs输出值
    output [31:0] Rt_data_out // Rt输出值
);

// 寄存器堆（32个32位寄存器）
reg [31:0] array_reg [0:31];

// 写
integer i;
always @(negedge clk or posedge rst) begin
    if (rst) begin
        // 异步复位，所有寄存器清零（包括寄存器0）
        for (i = 0; i < 32; i = i + 1)
            array_reg[i] <= 32'b0;
    end else if (we && (RdC != 5'b0)) begin
        // 写，寄存器0不可以写
        array_reg[RdC] <= Rd_data_in;
    end
end

// 读，寄存器0硬连线为0
assign Rs_data_out = (RsC == 5'b0) ? 32'b0 : array_reg[RsC];
assign Rt_data_out = (RtC == 5'b0) ? 32'b0 : array_reg[RtC];

endmodule