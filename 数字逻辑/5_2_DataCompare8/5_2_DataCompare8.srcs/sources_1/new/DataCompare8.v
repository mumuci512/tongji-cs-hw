module DataCompare8(
    input [7:0] iData_a, // 输入数据 a
    input [7:0] iData_b, // 输入数据 b
    output reg [2:0] oData // 结果输出
);
    always @(*) begin
        if (iData_a > iData_b) begin
            oData = 3'b100; // A > B
        end else if (iData_a < iData_b) begin
            oData = 3'b010; // A < B
        end else begin
            oData = 3'b001; // A = B
        end
    end
endmodule
