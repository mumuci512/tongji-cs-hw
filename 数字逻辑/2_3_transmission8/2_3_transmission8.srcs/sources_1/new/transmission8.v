module transmission8 (
    input [7:0] iData,   // 输入信号
    input A, B, C,       // 选择信号
    output [7:0] oData   // 输出信号
);
    reg [7:0] temp;

    always @(*) begin
        // 默认输出为1
        temp = 8'b11111111;
        // 根据选择信号设置输出
        case ({A, B, C})
            3'b000: temp[0] = iData[0];
            3'b001: temp[1] = iData[1];
            3'b010: temp[2] = iData[2];
            3'b011: temp[3] = iData[3];
            3'b100: temp[4] = iData[4];
            3'b101: temp[5] = iData[5];
            3'b110: temp[6] = iData[6];
            3'b111: temp[7] = iData[7];
        endcase
    end

    assign oData = temp;
endmodule
