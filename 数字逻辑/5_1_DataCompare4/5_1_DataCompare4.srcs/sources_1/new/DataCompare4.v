module DataCompare4(
    input [3:0] iData_a, // 输入数据 a
    input [3:0] iData_b, // 输入数据 b
    input [2:0] iData,   // 级联输入 a>b、a<b、a=b
    output reg [2:0] oData // 结果输出 A>B、A<B、A=B
);
    always @(*) begin
        if (iData_a > iData_b) begin
            oData = 3'b100; // A > B
        end else if (iData_a < iData_b) begin
            oData = 3'b010; // A < B
        end else begin
            oData = iData; // A = B
        end
    end
endmodule
