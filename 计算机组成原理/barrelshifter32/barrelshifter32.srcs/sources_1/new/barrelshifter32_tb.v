module testbench;
    reg [31:0] a;
    reg [4:0] b;
    reg [1:0] aluc;
    wire [31:0] c;
    
    barrelshifter32 uut (
        .a(a),
        .b(b),
        .aluc(aluc),
        .c(c)
    );
    
    initial begin
        //ËãÊõÓÒÒÆ
        a = 32'h80000000; 
        b = 5'd1;         
        aluc = 2'b00;    
        #10;
        $display("Arithmetic Right Shift: a=%h, b=%d => c=%h", a, b, c);
        
        //Âß¼­ÓÒÒÆ
        aluc = 2'b10;    
        #10;
        $display("Logical Right Shift: a=%h, b=%d => c=%h", a, b, c);
        
        //ËãÊõ×óÒÆ
        a = 32'h00000001; 
        aluc = 2'b01;     
        #10;
        $display("Arithmetic Left Shift: a=%h, b=%d => c=%h", a, b, c);
        
        //Âß¼­×óÒÆ
        aluc = 2'b11;   
        #10;
        $display("Logical Left Shift: a=%h, b=%d => c=%h", a, b, c);
        
        $finish;
    end
endmodule