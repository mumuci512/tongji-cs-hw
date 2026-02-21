module DIV(
    input [31:0] dividend,
    input [31:0] divisor,
    input start,
    input clock,
    input reset,
    output reg [31:0] q,
    output reg [31:0] r,
    output reg busy
);

    // 同DIVU
    reg [5:0] count;
    reg [31:0] divisor_reg;
    reg [63:0] dividend_reg;
    reg running;

    reg result_sign_q;
    reg result_sign_r;

    reg [31:0] abs_dividend;
    reg [31:0] abs_divisor;

    always @(posedge clock or posedge reset) begin
        if (reset) begin
            q <= 0;
            r <= 0;
            busy <= 0;
            running <= 0;
            count <= 0;
            dividend_reg <= 0;
            divisor_reg <= 0;
            result_sign_q <= 0;
            result_sign_r <= 0;
        end else begin
            if (start && !running) begin
                result_sign_q <= dividend[31] ^ divisor[31]; // 商符号 = 被除数符号 异或 除数符号
                result_sign_r <= dividend[31]; // 余数符号 = 被除数符号

                abs_dividend <= dividend[31] ? (~dividend + 1) : dividend;
                abs_divisor <= divisor[31] ? (~divisor + 1) : divisor;

                dividend_reg <= {32'b0, dividend[31] ? (~dividend + 1) : dividend}; // 不用abs_dividend，因为时序逻辑
                divisor_reg <= divisor[31] ? (~divisor + 1) : divisor;
                count <= 0;
                busy <= 1;
                running <= 1;
            end else if (running) begin
                if (count < 32) begin // 与DIVU相同
                    dividend_reg = dividend_reg << 1;
                    if (dividend_reg[63:32] >= divisor_reg) begin
                        dividend_reg[63:32] = dividend_reg[63:32] - divisor_reg;
                        dividend_reg[0] = 1'b1;
                    end else begin
                        dividend_reg[0] = 1'b0;
                    end
                    count = count + 1;
                end else begin
                    q <= result_sign_q ? (~dividend_reg[31:0] + 1) : dividend_reg[31:0];
                    r <= result_sign_r ? (~dividend_reg[63:32] + 1) : dividend_reg[63:32];
                    busy <= 0;
                    running <= 0;
                end
            end
        end
    end

endmodule
