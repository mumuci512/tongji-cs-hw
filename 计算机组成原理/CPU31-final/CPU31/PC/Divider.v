`timescale 1ns / 1ps
module Divider (
input I_CLK, // 输入时钟信号，上升沿有效
input rst, // 同步复位信号，低电平有效
output reg O_CLK // 输出时钟
);
// 使用参数来设置分频倍数，默认为20
parameter DIV_RATIO = 100000000;

// 定义一个计数器
reg [31:0] counter=32'd0; // 用一个32位计数器来存储计数值

// 每次 I_CLK 上升沿触发，且同步复位
always @(posedge I_CLK) begin
    if (rst) begin
        // 如果复位信号有效，重置计数器和输出时钟
        counter <= 0;
        O_CLK <= 0; // 输出时钟先为高电平（同步复位后）
    end else begin
        // 如果计数器达到分频倍数的一半时，反转输出时钟
        if (counter == DIV_RATIO / 2 - 1) begin
            counter <= 0; // 重置计数器
            O_CLK <= ~O_CLK; // 反转输出时钟
        end else begin
            counter <= counter + 1; // 计数器加1
        end
    end
end

endmodule