module tb_display7();
    reg [3:0] iData;
    wire [6:0] oData;

    display7 uut (
        .iData(iData),
        .oData(oData)
    );

    initial begin
        $monitor("At time %0t: iData=%b, oData=%b", 
                 $time, iData, oData);

        iData = 4'b0000; #10; 
        iData = 4'b0001; #10; 
        iData = 4'b0010; #10;
        iData = 4'b0011; #10;
        iData = 4'b0100; #10; 
        iData = 4'b0101; #10; //5
        iData = 4'b0110; #10; //6
        iData = 4'b0111; #10; //7
        iData = 4'b1000; #10; //8
        iData = 4'b1001; #10; //9
        iData = 4'b1010; #10; //A
        iData = 4'b1011; #10; //b
        iData = 4'b1100; #10; //C
        iData = 4'b1101; #10; //d
        iData = 4'b1110; #10; //E
        iData = 4'b1111; #10; //F

        #10 $finish;
    end
endmodule