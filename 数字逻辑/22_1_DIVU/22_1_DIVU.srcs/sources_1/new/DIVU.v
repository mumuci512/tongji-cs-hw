module DIVU(
    input [31:0] dividend,
    input [31:0] divisor,
    input start,
    input clock,
    input reset,
    output reg [31:0] q,
    output reg [31:0] r,
    output reg busy
);

    reg [5:0] count;                  
    reg [31:0] divisor_reg;
    reg [63:0] dividend_reg; // 高位存储余数，低位为商

    reg running; // 内部状态标志

    always @(posedge clock or posedge reset) begin
        if (reset) begin
            q <= 0;
            r <= 0;
            busy <= 0;
            running <= 0;
            count <= 0;
            dividend_reg <= 0;
            divisor_reg <= 0;
        end else begin
            if (start && !running) begin
                busy <= 1;
                running <= 1;
                count <= 0;
                dividend_reg <= {32'b0, dividend};  // 将被除数放在低32位
                divisor_reg <= divisor;
            end else if (running) begin
                if (count < 32) begin
                    dividend_reg = dividend_reg << 1;  // 左移1
                    if (dividend_reg[63:32] >= divisor_reg) begin
                        dividend_reg[63:32] = dividend_reg[63:32] - divisor_reg;
                        dividend_reg[0] = 1'b1;  // 够减，上商1
                    end else begin
                        dividend_reg[0] = 1'b0;  // 不够减，上商0
                    end
                    count = count + 1;
                end else begin
                    q <= dividend_reg[31:0];     
                    r <= dividend_reg[63:32];   
                    busy <= 0;
                    running <= 0;
                end
            end
        end
    end
endmodule
