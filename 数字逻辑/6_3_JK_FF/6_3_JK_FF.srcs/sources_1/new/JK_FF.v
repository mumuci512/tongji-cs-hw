module JK_FF(
    input CLK,        // 时钟信号，上升沿有效
    input J,         // 输入信号J
    input K,         // 输入信号K
    input RST_n,     // 复位信号，低电平有效
    output reg Q1,   // 输出信号Q
    output reg Q2    // 输出信号Q?
);

    always @(posedge CLK or negedge RST_n) begin
        if (!RST_n) begin
            Q1 <= 0;   // 复位时将Q设为0
            Q2 <= 1;   // 复位时将Q?设为1
        end else begin
            case ({J, K})
                2'b00: ;         // 保持状态
                2'b01: 	begin
				Q1 <= 0;  // 设置状态
				Q2 <= 1;
			end
                2'b10:	begin
				Q1 <= 1;  // 设置状态
				Q2 <= 0;
			end
                2'b11: 	begin
				Q1 <= ~Q1; // 取反状态
				Q2 <= ~Q2;
			end
            endcase
            //Q2 <= ~Q1; // Q?为Q的反相
        end
    end

endmodule
