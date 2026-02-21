`timescale 1ns / 1ps

module Regfiles(
    input clk,                // 时钟信号，下降沿写入数据
    input rst,                // 异步复位信号，高电平时全部寄存器置零
    input we,                 // 写使能信号，高电平允许写入
    input [4:0] RsC,          // Rs的地址（输出）
    input [4:0] RtC,          // Rt的地址（输出）
    input Rs_ena,             // Rs读使能，高电平时输出有效
    input Rt_ena,             // Rt读使能，高电平时输出有效
    input [4:0] RdC,          // Rd的地址（写入）
    input [31:0] Rd_data_in,  // 要写入的值
    output reg [31:0] Rs_data_out, // Rs输出值
    output reg [31:0] Rt_data_out, // Rt输出值
    output [31:0] a,       // 监控寄存器6的值
    output [31:0] b,       // 监控寄存器7的值
    output [31:0] c,      // 监控寄存器15的值
    output [31:0] d       // 监控寄存器16的值
);

reg [31:0] array_reg [0:31];

// 写操作和复位逻辑
integer i;
always @(posedge clk or posedge rst) begin
    if (rst) begin
        for (i = 0; i < 32; i = i + 1)
            array_reg[i] <= 32'b0;
    end else if (we && (RdC != 5'b0)) begin
        // 写使能有效且目标地址不为0时写入数据
        // 寄存器0不可以写，始终保持为0
        array_reg[RdC] <= Rd_data_in;
    end
end

always @(*) begin
    if (rst) begin
        Rs_data_out = 32'b0;
    end else if (RsC == 5'b0) begin
        // 读取寄存器0时，硬连线输出0
        Rs_data_out = 32'b0;
    end else if ((RsC == RdC) && we && Rs_ena) begin
        // 旁路：当前周期写入的数据，同一周期读取时直接使用写入值
        Rs_data_out = Rd_data_in;
    end else if (Rs_ena) begin
        Rs_data_out = array_reg[RsC];
    end else begin
        // 读使能无效时，输出高阻态
        Rs_data_out = 32'bz;
    end
end

always @(*) begin
    if (rst) begin
        Rt_data_out = 32'b0;
    end else if (RtC == 5'b0) begin
        Rt_data_out = 32'b0;
    end else if ((RtC == RdC) && we && Rt_ena) begin
        // 旁路：当前周期写入的数据，同一周期读取时直接使用写入值
        Rt_data_out = Rd_data_in;
    end else if (Rt_ena) begin
        // 读使能有效时，输出对应寄存器的值
        Rt_data_out = array_reg[RtC];
    end else begin
        Rt_data_out = 32'bz;
    end
end

assign a = array_reg[6];
assign b = array_reg[7];
assign c = array_reg[15];
assign d = array_reg[16];

endmodule