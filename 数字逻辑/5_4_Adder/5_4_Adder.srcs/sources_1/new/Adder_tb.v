module tb_Adder();
    reg [7:0] iData_a;
    reg [7:0] iData_b;
    reg iC;
    wire [7:0] oData;
    wire oData_C;

    // 实例化被测试模块
    Adder uut (
        .iData_a(iData_a),
        .iData_b(iData_b),
        .iC(iC),
        .oData(oData),
        .oData_C(oData_C)
    );

    initial begin
        // 监视所有输入和输出信号
        $monitor("iData_a = %b, iData_b = %b, iC = %b -> oData = %b, oData_C = %b", iData_a, iData_b, iC, oData, oData_C);

        // 测试用例
        // Test case 1
        iData_a = 8'b00000001; iData_b = 8'b00000001; iC = 1'b0; #10; // 1 + 1 = 2
        // Test case 2
        iData_a = 8'b11111111; iData_b = 8'b00000001; iC = 1'b0; #10; // 255 + 1 = 256 (carry out)
        // Test case 3
        iData_a = 8'b10101010; iData_b = 8'b01010101; iC = 1'b1; #10; // 170 + 85 + 1 = 256 (carry out)
        // Test case 4
        iData_a = 8'b11110000; iData_b = 8'b00001111; iC = 1'b0; #10; // 240 + 15 = 255
        // Test case 5
        iData_a = 8'b01111111; iData_b = 8'b01111111; iC = 1'b0; #10; // 127 + 127 = 254

        // 测试结束
        $finish;
    end
endmodule
