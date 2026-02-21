`timescale 1ns / 1ps

// 2选1多路选择器，5位数据宽度
module Mux2To1_5Bit(
    input [4:0]         d0,
    input [4:0]         d1,
    input               sel,
    output reg [4:0]    out
    );
    
    always@(*) begin
        case(sel)
            1'b0: out <= d0;    // 选择输入0
            1'b1: out <= d1;    // 选择输入1
        endcase
    end
    
endmodule

// 2选1多路选择器，32位数据宽度
module Mux2To1_32Bit(
    input [31:0]        d0,
    input [31:0]        d1,
    input               sel,
    output reg [31:0]   out
    );
    
    always@(*) begin
        case(sel)
            1'b0: out <= d0;    // 选择输入0
            1'b1: out <= d1;    // 选择输入1
        endcase
    end
    
endmodule

// 4选1多路选择器，32位数据宽度
module Mux4To1_32Bit(
    input [31:0]        d0,
    input [31:0]        d1,
    input [31:0]        d2,
    input [31:0]        d3,
    input [1:0]         sel,
    output reg [31:0]   out
    );
    
    always@(*) begin
        case(sel)
            2'b00: out <= d0;   // 选择输入0
            2'b01: out <= d1;   // 选择输入1
            2'b10: out <= d2;   // 选择输入2
            2'b11: out <= d3;   // 选择输入3
        endcase
    end
    
endmodule

// 8选1多路选择器，32位数据宽度
module Mux8To1_32Bit(
    input [31:0]        d0,
    input [31:0]        d1,
    input [31:0]        d2,
    input [31:0]        d3,
    input [31:0]        d4,
    input [31:0]        d5,
    input [31:0]        d6,
    input [31:0]        d7,
    input [2:0]         sel,
    output reg [31:0]   out
    );
    
    always@(*) begin
        case(sel)
            3'b000: out <= d0;  // 选择输入0
            3'b001: out <= d1;  // 选择输入1
            3'b010: out <= d2;  // 选择输入2
            3'b011: out <= d3;  // 选择输入3
            3'b100: out <= d4;  // 选择输入4
            3'b101: out <= d5;  // 选择输入5
            3'b110: out <= d6;  // 选择输入6
            3'b111: out <= d7;  // 选择输入7
        endcase
    end
    
endmodule