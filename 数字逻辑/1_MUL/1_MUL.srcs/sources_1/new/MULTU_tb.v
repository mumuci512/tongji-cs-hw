`timescale 1ns/1ps

module tb_MULTU;
    reg clk;
    reg reset;
    reg [31:0] a;
    reg [31:0] b;
    wire [63:0] z;

    MULTU uut (
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
        a = 32'd0;
        b = 32'd0;
        #10;
        reset = 0;
        #10;

        a = 32'd0;
        b = 32'd0;
        #10;

        a = 32'd0;
        b = 32'd255; // 8'b11111111
        #10;

        a = 32'd179; // 8'b10110011
        b = 32'd0;
        #10;

        a = 32'd255; // 8'b11111111
        b = 32'd255; // 8'b11111111
        #10;

        a = 32'd128; // 8'b10000000
        b = 32'd170; // 8'b10101010
        #10;

        a = 32'd170; // 8'b10101010
        b = 32'd128; // 8'b10000000
        #10;

        a = 32'd45; // 8'b00101101
        b = 32'd104; // 8'b01101000
        #10;

        a = 32'd71; // 8'b01000111
        b = 32'd14; // 8'b00001110
        #10;
        
        a = 32'd4294967295; // 32位全为1
        b = 32'd4294967295; // 32位全为1
        #10;
        
        #20;
        $stop;
    end
endmodule
