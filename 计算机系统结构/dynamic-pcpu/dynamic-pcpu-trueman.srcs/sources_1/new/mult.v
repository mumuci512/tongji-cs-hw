module MULT(
    input reset,           // 复位信号，高电平有效
    input start,           // 启动信号，高电平开始计算
    input in_sign,         // 符号控制（1为有符号，0为无符号）
    input [31:0] a,
    input [31:0] b,
    output [31:0] out_hi,
    output [31:0] out_lo
);

    // 内部寄存器：存储临时操作数与运算结果
    reg [31:0] a_tmp, b_tmp;  
    reg [63:0] answer_tmp, answer;          
    reg neg;              
    
    integer i;
    
    // 组合逻辑实现32位乘法运算，支持有符号/无符号模式
    always @(*) begin
        if (reset) begin
            // 复位状态：清空所有临时寄存器与运算结果
            a_tmp = 32'd0;
            b_tmp = 32'd0;
            answer = 64'd0;
            neg = 1'b0;
        end
        else if (start) begin
            // 有一个操作数为0时，直接返回0结果
            if (a == 0 || b == 0) begin
                answer = 64'd0;
            end 
            else if (~in_sign) begin  // 无符号乘法：移位累加实现
                answer = 64'd0;
                for (i = 0; i < 32; i = i + 1) begin
                    answer_tmp = b[i] ? ({32'd0, a} << i) : 64'd0;
                    answer = answer + answer_tmp;
                end
            end 
            else begin  // 有符号乘法：先取绝对值运算，再处理符号
                answer = 64'd0;
                neg = a[31] ^ b[31];  // 计算结果符号（异或判断正负）
                
                // 对有符号数取绝对值（补码转原码）
                a_tmp = a[31] ? (~a + 1) : a;
                b_tmp = b[31] ? (~b + 1) : b;
                
                // 移位累加计算绝对值的乘积
                for (i = 0; i < 32; i = i + 1) begin
                    answer_tmp = b_tmp[i] ? ({32'd0, a_tmp} << i) : 64'd0;
                    answer = answer + answer_tmp;
                end
                
                // 结果为负时，将乘积转为补码形式
                if (neg) begin
                    answer = ~answer + 1;
                end
            end
        end
        else begin
            // 未启动时，运算结果清零
            answer = 64'd0;
        end
    end
    
    // 输出结果：启动时输出64位乘积的高低32位，否则输出0
    assign out_lo = start ? answer[31:0] : 32'd0;
    assign out_hi = start ? answer[63:32] : 32'd0;
    
endmodule