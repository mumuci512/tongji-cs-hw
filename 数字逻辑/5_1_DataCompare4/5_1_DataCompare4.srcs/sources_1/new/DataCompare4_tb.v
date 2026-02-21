module tb_DataCompare4;
    reg [3:0] iData_a;
    reg [3:0] iData_b;
    reg [2:0] iData;
    wire [2:0] oData;

    DataCompare4 uut (
        .iData_a(iData_a),
        .iData_b(iData_b),
        .iData(iData),
        .oData(oData)
    );

    initial begin
        // 测试不同输入组合
        iData_a = 4'b0101; iData_b = 4'b0101; iData = 3'b100; #10; // A = B
        iData_a = 4'b0101; iData_b = 4'b0101; iData = 3'b010; #10; // A = B
        iData_a = 4'b0101; iData_b = 4'b0101; iData = 3'b001; #10; // A = B
        iData_a = 4'b0011; iData_b = 4'b0001; iData = 3'b011; #10; // A > B
        iData_a = 4'b0001; iData_b = 4'b0011; iData = 3'b000; #10; // A < B
        iData_a = 4'b1111; iData_b = 4'b0000; iData = 3'b110; #10; // A > B
        
        $finish;
    end

    initial begin
        $monitor("Time: %0d, A: %b, B: %b, oData: %b", $time, iData_a, iData_b, oData);
    end
endmodule
