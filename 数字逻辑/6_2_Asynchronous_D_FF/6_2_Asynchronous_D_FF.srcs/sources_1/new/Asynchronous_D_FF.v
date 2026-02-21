module Asynchronous_D_FF(
    input CLK,        // 时钟信号
    input D,         // 输入信号D
    input RST_n,     // 复位信号，低电平有效
    output reg Q1,   // 输出信号Q
    output reg Q2    // 输出信号Q?
);

    always @(negedge RST_n or posedge CLK) begin
        if (!RST_n) begin
            Q1 <= 0;   // 复位时将Q设为0
            Q2 <= 1;   // 复位时将Q?设为1
        end else begin
            Q1 <= D;   // 在时钟上升沿更新Q
            Q2 <= ~D;  // Q?为D的反相
        end
    end

endmodule
