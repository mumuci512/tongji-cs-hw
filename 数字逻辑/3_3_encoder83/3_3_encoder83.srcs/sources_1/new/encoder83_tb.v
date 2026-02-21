module tb_encoder83();
    reg [7:0] iData;
    wire [2:0] oData;

    encoder83 uut (
        .iData(iData),
        .oData(oData)
    );

    initial begin
        $monitor("At time %0t: iData=%b, oData=%b", $time, iData, oData);

        iData = 8'b00000001; #10; // 输入 D0
        iData = 8'b00000010; #10; // 输入 D1
        iData = 8'b00000100; #10; // 输入 D2
        iData = 8'b00001000; #10; // 输入 D3
        iData = 8'b00010000; #10; // 输入 D4
        iData = 8'b00100000; #10; // 输入 D5
        iData = 8'b01000000; #10; // 输入 D6
        iData = 8'b10000000; #10; // 输入 D7

        #10 $finish;
    end
endmodule