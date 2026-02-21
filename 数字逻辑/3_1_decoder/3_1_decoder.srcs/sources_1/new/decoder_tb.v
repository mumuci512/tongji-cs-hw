module tb_decoder();
    reg [2:0] iData;
    reg [1:0] iEna;
    wire [7:0] oData;

    decoder uut (iData,iEna,oData);

    initial begin
        $monitor("At time %0t: iData=%b, iEna=%b, oData=%b", 
                 $time, iData, iEna, oData);

        iEna = 2'b10;

        iData = 3'b000; #10;
        iData = 3'b001; #10;
        iData = 3'b010; #10;
        iData = 3'b011; #10;
        iData = 3'b100; #10;
        iData = 3'b101; #10;
        iData = 3'b110; #10;
        iData = 3'b111; #10;

        iEna = 2'b00; iData = 3'b000; #10;
        iEna = 2'b01; iData = 3'b001; #10;
        iEna = 2'b11; iData = 3'b010; #10;

        #10 $finish;
    end
endmodule
