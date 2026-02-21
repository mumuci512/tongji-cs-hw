`timescale 1ns / 1ps
module clz (
    input [31:0] in,      // 32位输入
    output reg [31:0] out // 32位输出（前导零个数）
);

wire [7:0] group3 = in[31:24]; // 高8位
wire [7:0] group2 = in[23:16]; // 次高8位
wire [7:0] group1 = in[15:8];  // 次低8位
wire [7:0] group0 = in[7:0];   // 低8位

// 组内优先级编码器
function [2:0] priority8;
    input [7:0] data;
    begin
        casez (data)
            8'b1???????: priority8 = 3'd0;
            8'b01??????: priority8 = 3'd1;
            8'b001?????: priority8 = 3'd2;
            8'b0001????: priority8 = 3'd3;
            8'b00001???: priority8 = 3'd4;
            8'b000001??: priority8 = 3'd5;
            8'b0000001?: priority8 = 3'd6;
            8'b00000001: priority8 = 3'd7;
            default:     priority8 = 3'd0;
        endcase
    end
endfunction

// 组选择逻辑
wire [2:0] pos3 = priority8(group3);
wire [2:0] pos2 = priority8(group2);
wire [2:0] pos1 = priority8(group1);
wire [2:0] pos0 = priority8(group0);

wire group3_nonzero = |group3;
wire group2_nonzero = |group2;
wire group1_nonzero = |group1;
wire group0_nonzero = |group0;

// 分层选择器
always @(*) begin
    casez ({group3_nonzero, group2_nonzero, group1_nonzero, group0_nonzero})
        4'b1???: out = {26'd0, 3'b000, pos3};  // 0-7
        4'b01??: out = {26'd0, 3'b001, pos2};  // 8-15
        4'b001?: out = {26'd0, 3'b010, pos1};  // 16-23
        4'b0001: out = {26'd0, 3'b011, pos0};  // 24-31
        default: out = 32'd32;                 // 全零情况
    endcase
end

endmodule
