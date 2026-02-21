module MULT(
    input clk,             // 时钟信号
    input reset,           // 复位信号，高电平有效
    input [31:0] a,        // 被乘数（带符号）
    input [31:0] b,        // 乘数（带符号）
    output reg [63:0] z    // 乘积输出（64位带符号数）
);

    reg [31:0] abs_a, abs_b;  
    reg [63:0] temp;          
    reg sign;              
    integer i;              

    always @(posedge clk or posedge reset) begin
        if (reset) begin
            abs_a <= 32'd0;
            abs_b <= 32'd0;
            temp  <= 64'd0;
            sign  <= 1'b0;
        end
        else begin
            // 当a与b的符号不同，结果为负
            sign = a[31] ^ b[31];

            // 取a和b的绝对值
            abs_a = a[31] ? (~a + 1) : a;
            abs_b = b[31] ? (~b + 1) : b;

            // 同 MULTU 模块思路
            temp = 64'd0;
            for (i = 0; i < 32; i = i + 1) begin
                if (abs_b[i])
                    temp = temp + ({32'd0, abs_a} << i);
            end

            if (sign)
                temp = ~temp + 1;
        end
    end

    always @(posedge clk or posedge reset) begin
        if (reset)
            z <= 64'd0;
        else
            z <= temp;
    end

endmodule
