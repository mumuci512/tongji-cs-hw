`timescale 1ns/1ps

module tb_MULT;
    reg clk;
    reg reset;
    reg signed [31:0] a;
    reg signed [31:0] b;
    wire [63:0] z;

    MULT uut (
        .clk(clk),
        .reset(reset),
        .a(a),
        .b(b),
        .z(z)
    );

    initial begin
        clk = 0;
        forever #5 clk = ~clk;
    end

    initial begin
        // 复位
        reset = 1;
        a = 32'sd0;
        b = 32'sd0;
        #10;
        reset = 0;
        #10;

        a = 32'sd0;
        b = 32'sd0;
        #10;

        a = 32'sd0;
        b = -32'sd1; // 对应 8'b11111111 的带符号情况
        #10;

        a = 32'sd179;
        b = 32'sd0;
        #10;

        a = -32'sd1;
        b = -32'sd1;
        #10;

        a = -32'sd128;
        b = 32'sd170;
        #10;

        a = 32'sd170;
        b = -32'sd128;
        #10;

        a = 32'sd45;
        b = -32'sd104;
        #10;

        a = -32'sd71;
        b = 32'sd14;
        #10;

        #20;
        $stop;
    end
endmodule
