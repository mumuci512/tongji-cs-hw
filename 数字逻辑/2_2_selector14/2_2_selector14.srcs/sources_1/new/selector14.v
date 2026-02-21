module de_selector14 (
    input iC,            // 输入信号
    input iS1, iS0,     // 选择信号
    output reg oZ0, oZ1, oZ2, oZ3 // 4个输出信号
);

    always @(*) begin
        // 默认输出为0
        oZ0 = 0;
        oZ1 = 0;
        oZ2 = 0;
        oZ3 = 0;

        case ({iS1, iS0})
            2'b00: begin
                oZ0 = iC; // oZ0 与 iC 相同
                oZ1 = 1;  // oZ1 = 1
                oZ2 = 1;  // oZ2 = 1
                oZ3 = 1;  // oZ3 = 1
            end
            2'b01: begin
                oZ0 = 1;  // oZ0 = 1
                oZ1 = iC; // oZ1 与 iC 相同
                oZ2 = 1;  // oZ2 = 1
                oZ3 = 1;  // oZ3 = 1
            end
            2'b10: begin
                oZ0 = 1;  // oZ0 = 1
                oZ1 = 1;  // oZ1 = 1
                oZ2 = iC; // oZ2 与 iC 相同
                oZ3 = 1;  // oZ3 = 1
            end
            2'b11: begin
                oZ0 = 1;  // oZ0 = 1
                oZ1 = 1;  // oZ1 = 1
                oZ2 = 1;  // oZ2 = 1
                oZ3 = iC; // oZ3 与 iC 相同
            end
        endcase
    end
endmodule
