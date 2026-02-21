`timescale 1ns / 1ps
module HI_LO(
    input clk,
    input rst,
    input [31:0] HI_in,
    input [31:0] LO_in,
    input HI_we,             //写使能信号
    input LO_we,             //写使能信号
    output [31:0] HI_out,
    output [31:0] LO_out
    );
    
    reg [63:0] hi_lo;
    
    assign HI_out = hi_lo[63:32];
    assign LO_out = hi_lo[31:0];
    
    always @(negedge clk or posedge rst) begin
        if (rst) begin
            hi_lo <= 64'b0;
        end else begin
            if(HI_we)
                hi_lo[63:32] <= HI_in;
            if(LO_we)
                hi_lo[31:0] <= LO_in;
        end
    end
endmodule