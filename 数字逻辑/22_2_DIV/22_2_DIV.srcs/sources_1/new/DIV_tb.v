`timescale 1ns / 1ps

module DIV_tb;
    reg [31:0] dividend;
    reg [31:0] divisor;
    reg start;
    reg clock;
    reg reset;
    
    wire [31:0] q;
    wire [31:0] r;
    wire busy;
    
    DIV uut (
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
        
        // +15 / +3 = +5 гр0
        dividend = 32'h0000000F;
        divisor = 32'h00000003;
        start = 1;
        #10 start = 0;
        wait(busy == 0);
        #10;
        if (q === 32'h00000005 && r === 32'h00000000)
            $display("Test 1: Passed");
        else
            $display("Test 1: Failed (q=%h, r=%h)", q, r);
        
        // +16 / -3 = -5 гр1
        dividend = 32'h00000010;
        divisor = 32'hFFFFFFFD;
        start = 1;
        #10 start = 0;
        wait(busy == 0);
        #10;
        if (q === 32'hFFFFFFFB && r === 32'h00000001)
            $display("Test 3: Passed");
        else
            $display("Test 3: Failed (q=%h, r=%h)", q, r);

        // -16 / +3 = -5 гр-1
        dividend = 32'hFFFFFFFD;
        divisor = 32'h00000003;
        start = 1;
        #10 start = 0;
        wait(busy == 0);
        #10;
        if (q === 32'hFFFFFFFB && r === 32'hFFFFFFFF)
            $display("Test 3: Passed");
        else
            $display("Test 3: Failed (q=%h, r=%h)", q, r);
            
        // -15 / +3 = -5 гр0
            dividend = 32'hFFFFFFF1; 
            divisor = 32'h00000003;
            start = 1;
            #10 start = 0;
            wait(busy == 0);
            #10;
            if (q === 32'hFFFFFFFB && r === 32'h00000000)
                $display("Test 2: Passed");
            else
                $display("Test 2: Failed (q=%h, r=%h)", q, r);
                
        // -1508 / +342 = -4 гр -140
        dividend = 32'hFFFFFA1C;
        divisor = 32'h00000156;
        start = 1;
        #10 start = 0;
        wait(busy == 0);
        #10;
        if (q === 32'hFFFFFFFC && r === 32'hFFFFFF74)
            $display("Test 4: Passed");

        $finish;
    end
endmodule