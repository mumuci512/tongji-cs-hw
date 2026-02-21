`timescale 1ns / 1ps
module PC (
    input         clk,         // 下降沿有效，同步读取
    input         rst,        // 异步复位信号（高有效）
    input [31:0]  pc_in,    // 本次输入PC寄存器的指令地址，也就是下一次要执行的指令
    output reg [31:0] pc_out       // 本次从PC寄存器中传出的指令地址，也就是当前需要执行的指令
);

always @(negedge clk or posedge rst) begin
    if (rst) begin
        pc_out = 32'h00400000;    // 复位时PC=32'h00400000
    end else begin
        pc_out = pc_in;
    end
end

endmodule