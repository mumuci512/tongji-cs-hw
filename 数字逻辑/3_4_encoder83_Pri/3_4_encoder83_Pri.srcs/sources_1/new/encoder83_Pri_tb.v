module tb_encoder83_Pri();
    reg [7:0] iData;
    reg iEI;
    wire [2:0] oData;
    wire oEO;

    encoder83_Pri uut (
        .iData(iData),
        .iEI(iEI),
        .oData(oData),
        .oEO(oEO)
    );

    initial begin
        $monitor("At time %0t: iData=%b, iEI=%b, oData=%b, oEO=%b", 
                 $time, iData, iEI, oData, oEO);

        iEI = 0;

        iData = 8'b00010010; #10; 
        iData = 8'b10010001; #10; 
        iData = 8'b11010011; #10; 
        iData = 8'b11100110; #10; 
        iData = 8'b11110011; #10; 
        iData = 8'b11111010; #10;
        iData = 8'b11111101; #10;
        iData = 8'b11111110; #10; 
        iData = 8'b11111111; #10; 

        iEI = 1; iData = 8'b11111110; #10;
        iEI = 1; iData = 8'b11111101; #10;

        #10 $finish;
    end
endmodule
