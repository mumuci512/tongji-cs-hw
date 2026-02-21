`timescale 1ns / 1ps

module hi_lo(
    input           clk,
    input           rst,
    input  [31:0]   hi_in,
    input  [31:0]   lo_in,
    input           hi_we,
    input           lo_we,
    output [31:0]   hi_out,
    output [31:0]   lo_out
);

    // 64位寄存器整合HI（高32位）与LO（低32位）
    reg [63:0] hi_lo_reg;

    // 拆分64位寄存器，输出HI与LO数据
    assign hi_out = hi_lo_reg[63:32];
    assign lo_out = hi_lo_reg[31:0];

    // 负时钟沿触发的寄存器写操作，避免与流水线时序冲突
    always @(negedge clk or posedge rst) begin
        if (rst) begin
            // 复位时将HI/LO寄存器整体清零
            hi_lo_reg <= 64'b0;
        end else begin
            // HI写使能有效时，写入HI寄存器数据
            if (hi_we)
                hi_lo_reg[63:32] <= hi_in;
            // LO写使能有效时，写入LO寄存器数据
            if (lo_we)
                hi_lo_reg[31:0] <= lo_in;
        end
    end
endmodule