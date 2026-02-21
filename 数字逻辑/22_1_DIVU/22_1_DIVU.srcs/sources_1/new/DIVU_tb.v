`timescale 1ns / 1ps

module DIVU_tb;
    reg [31:0] dividend;
    reg [31:0] divisor;
    reg start;
    reg clock;
    reg reset;
    
    wire [31:0] q;
    wire [31:0] r;
    wire busy;
    
    DIVU uut (
        .dividend(dividend),
        .divisor(divisor),
        .start(start),
        .clock(clock),
        .reset(reset),
        .q(q),
        .r(r),
        .busy(busy)
    );
    
    initial begin
        clock = 0;
        forever #5 clock = ~clock;
    end
    
    initial begin
        reset = 1;
        start = 0;
        dividend = 0;
        divisor = 0;
        
        #10 reset = 0;
        #10;
        
        // 15 / 3 = 5 余0
        dividend = 32'h0000000F;
        divisor = 32'h00000003;
        start = 1;
        #10 start = 0;
        wait(busy == 0); // 等待运算完成
        #10;
        if (q === 32'h00000005 && r === 32'h00000000)
            $display("Test 1: Passed");
        else
            $display("Test 1: Failed (q=%h, r=%h)", q, r);
        
        // 14 / 3 = 4 余2
        dividend = 32'h0000000E;
        divisor = 32'h00000003;
        start = 1;
        #10 start = 0;
        wait(busy == 0);
        #10;
        if (q === 32'h00000004 && r === 32'h00000002)
            $display("Test 2: Passed");
        else
            $display("Test 2: Failed (q=%h, r=%h)", q, r);
        
        // 大数测试 0xFFFFFFFF / 0x0000FFFF
        dividend = 32'hFFFFFFFF;
        divisor = 32'h0000FFFF;
        start = 1;
        #10 start = 0;
        wait(busy == 0);
        #10;
        if (q === 32'h00010000 && r === 32'h0000FFFF)
            $display("Test 3: Passed");
        else
            $display("Test 3: Failed (q=%h, r=%h)", q, r);
        
        // 除数为0
        dividend = 32'h0000000F;
        divisor = 32'h00000000;
        start = 1;
        #10 start = 0;
        wait(busy == 0);
        #10;
        $display("Test 4: Divisor=0 (q=%h, r=%h)", q, r);
        
        $finish;
    end
endmodule