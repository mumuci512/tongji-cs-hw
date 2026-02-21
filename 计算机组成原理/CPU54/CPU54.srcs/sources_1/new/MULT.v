module MULT(
    input clk,             // 时钟信号
    input reset,           // 复位信号，高电平有效
    input start,           // 启动信号，高电平开始计算
    input [31:0] a,        // 被乘数（带符号）
    input [31:0] b,        // 乘数（带符号）
    output reg [63:0] z,   // 乘积输出（64位带符号数）
    output reg busy        // 忙信号，高电平表示计算中
);

    // 内部寄存器
    reg [31:0] abs_a, abs_b;  
    reg [63:0] temp;          
    reg sign;              
    reg [5:0] count;       // 迭代计数器
    reg running;           // 计算进行中标志

    always @(negedge clk or posedge reset) begin
        if (reset) begin
            abs_a <= 32'd0;
            abs_b <= 32'd0;
            temp  <= 64'd0;
            sign  <= 1'b0;
            count <= 6'd0;
            running <= 1'b0;
            busy <= 1'b0;
            z <= 64'd0;
        end
        else begin
            // 启动信号处理
            if (start && !running) begin
                // 当a与b的符号不同，结果为负
                sign <= a[31] ^ b[31];
                
                // 取a和b的绝对值
                abs_a <= a[31] ? (~a + 1) : a;
                abs_b <= b[31] ? (~b + 1) : b;
                
                // 初始化临时结果
                temp <= 64'd0;
                count <= 6'd0;
                running <= 1'b1;
                busy <= 1'b1;
            end
            
            // 计算过程
            if (running) begin
                if (abs_b[count]) begin
                    temp <= temp + ({32'd0, abs_a} << count);
                end
                
                count <= count + 1;
                
                // 计算完成
                if (count == 32) begin
                    running <= 1'b0;
                    if (sign) begin
                        z <= ~temp + 1;
                    end else begin
                        z <= temp;
                    end
                    busy <= 1'b0;
                end
            end
        end
    end

endmodule