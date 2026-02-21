module tb_DataCompare8;
    reg [7:0] iData_a;
    reg [7:0] iData_b;
    wire [2:0] oData;

    DataCompare8 uut (
        .iData_a(iData_a),
        .iData_b(iData_b),
        .oData(oData)
    );

    initial begin
        // 测试不同输入组合
        iData_a = 8'b11110000; iData_b = 8'b00001111; #10; // A > B
        iData_a = 8'b00001111; iData_b = 8'b00000001; #10; // A > B
        iData_a = 8'b00000001; iData_b = 8'b00001111; #10; // A < B
        iData_a = 8'b11111111; iData_b = 8'b00000000; #10; // A > B
        iData_a = 8'b10101010; iData_b = 8'b10101010; #10; // A = B
        
        $finish;
    end

    initial begin
        $monitor("Time: %0d, A: %b, B: %b, oData: %b", $time, iData_a, iData_b, oData);
    end
endmodule
