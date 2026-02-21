module DIV(
    input reset,
    input enable,
    input sign,
    input [31:0] dividend,
    input [31:0] divisor,
    output [31:0] q,  
    output [31:0] r 
);

    reg [31:0] q_reg;
    reg [31:0] r_reg;
    
    // reg [5:0] count;
    // reg [31:0] divisor_reg;
    // reg [63:0] dividend_reg;
    // reg running;
    // reg result_sign_q;
    // reg result_sign_r;
    // reg [31:0] abs_dividend;
    // reg [31:0] abs_divisor;
    
    reg neg_flag;
    reg div_neg_flag;
    reg [63:0] dividend_temp;
    reg [63:0] divisor_temp;
    
    integer i;

    always @(*) begin
        if (reset) begin
            q_reg = 0;
            r_reg = 0;
            dividend_temp = 0;
            divisor_temp = 0;
            neg_flag = 0;
            div_neg_flag = 0;
        end else if (enable) begin
            if (sign) begin
                // 无符号除法 (sign=1)
                dividend_temp = dividend;
                divisor_temp = {divisor, 32'b0};
                
                for (i = 0; i < 32; i = i + 1) begin
                    dividend_temp = dividend_temp << 1;
                    if (dividend_temp >= divisor_temp) begin
                        dividend_temp = dividend_temp - divisor_temp;
                        dividend_temp = dividend_temp + 1;
                    end
                end
                
                q_reg = dividend_temp[31:0];
                r_reg = dividend_temp[63:32];
            end else begin
                // 有符号除法 (sign=0)
                dividend_temp = dividend;
                divisor_temp = {divisor, 32'b0};
                neg_flag = dividend[31] ^ divisor[31];
                div_neg_flag = dividend[31];
                
                // 取绝对值
                if (dividend[31]) begin
                    dividend_temp = dividend ^ 32'hffffffff;
                    dividend_temp = dividend_temp + 1;
                end
                if (divisor[31]) begin
                    divisor_temp = {divisor ^ 32'hffffffff, 32'b0};
                    divisor_temp = divisor_temp + 64'h0000000100000000;
                end
                
                // 执行除法
                for (i = 0; i < 32; i = i + 1) begin
                    dividend_temp = dividend_temp << 1;
                    if (dividend_temp >= divisor_temp) begin
                        dividend_temp = dividend_temp - divisor_temp;
                        dividend_temp = dividend_temp + 1;
                    end
                end
                
                // 调整符号
                if (div_neg_flag) begin
                    dividend_temp = dividend_temp ^ 64'hffffffff00000000;
                    dividend_temp = dividend_temp + 64'h0000000100000000;
                end
                if (neg_flag) begin
                    dividend_temp = dividend_temp ^ 64'h00000000ffffffff;
                    dividend_temp = dividend_temp + 64'h0000000000000001;
                    if (dividend_temp[31:0] == 32'b0) begin
                        dividend_temp = dividend_temp - 64'h0000000100000000;
                    end
                end
                
                q_reg = dividend_temp[31:0];
                r_reg = dividend_temp[63:32];
            end
        end else begin
            // 当enable为0时，输出0
            q_reg = 0;
            r_reg = 0;
        end
    end

    // 分配输出
    assign q = q_reg;
    assign r = r_reg;
    // assign busy = 1'b0;

endmodule