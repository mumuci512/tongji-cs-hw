`timescale 1ns / 1ps
module alu(
    input [31:0] a,         // 32 位输入，操作数1
    input [31:0] b,         // 32 位输入，操作数2
    input [3:0] aluc,       // 4位输入，控制ALU的操作
    output reg [31:0] r,    // 32 位输出，由a、b经过aluc指定的操作生成
    output zero,            // 零标志位
    output reg carry,       // 进位标志位
    output negative,        // 负数标志位
    output reg overflow     // 溢出标志位
);

    // 定义临时变量用于无符号加法和减法的进位标志
    reg [32:0] unsigned_result;
    reg signed [31:0] signed_a, signed_b, signed_r;

    always @(*) begin
        carry = 0;
        overflow = 0;
        unsigned_result = 0;
        signed_a = a;
        signed_b = b;

        case (aluc)
            4'b0000: begin // Addu 无符号加法
                unsigned_result = a + b;
                r = unsigned_result[31:0];
                carry = unsigned_result[32];
            end

            4'b0010: begin // Add 有符号加法
                signed_r = signed_a + signed_b;
                r = signed_r;
                overflow = (signed_a[31] == signed_b[31]) && (signed_a[31] != signed_r[31]);
            end

            4'b0001: begin // Subu 无符号减法
                unsigned_result = a - b;
                r = unsigned_result[31:0];
                carry = unsigned_result[32];
            end

            4'b0011: begin // Sub 有符号减法
                signed_r = signed_a - signed_b;
                r = signed_r;
                overflow = (signed_a[31] != signed_b[31]) && (signed_a[31] != signed_r[31]);
            end

            4'b0100: r = a & b; // And
            4'b0101: r = a | b; // Or
            4'b0110: r = a ^ b; // Xor
            4'b0111: r = ~(a | b); // Nor

            4'b1000: r = {b[15:0], 16'b0}; // Lui

            4'b1011: begin // Slt 有符号比较
                r = (signed_a < signed_b) ? 32'b1 : 32'b0;
            end

            4'b1010: begin // Sltu 无符号比较
                r = (a < b) ? 32'b1 : 32'b0;
                carry = (a < b);
            end

            4'b1100: begin // Sra 算术右移
                r = $signed(b) >>> a[4:0];
                if (a[4:0] != 5'b0)
                    carry = b[a[4:0] - 1]; // 算术右移，最后移出的位是 b 中的第 a[4:0] - 1 位
                else
                    carry=0;
            end
            
            4'b1110: begin // Sll/Sla 逻辑/算术左移
                r = b << a[4:0];
                if (a[4:0] != 5'b0)
                    carry = b[31 - (a[4:0] - 1)]; // 逻辑左移，最后移出的位是 b 中的第 31 - (a[4:0] - 1) 位
                else
                    carry=0;            
            end
            
            4'b1101: begin // Srl 逻辑右移
                r = b >> a[4:0];
                if (a[4:0] != 5'b0)
                    carry = b[a[4:0] - 1]; // 逻辑右移，最后移出的位是 b 中的第 a[4:0] - 1 位
                else
                    carry=0;         
            end


            default: r = 32'b0; // 默认情况，输出0
        endcase
    end

   // 对所有操作统一设置 zero 标志位
    assign zero = (aluc == 4'b1011 || aluc == 4'b1010) ? (a == b) : (r == 32'b0);
    // 设置负数标志位
    assign negative = (aluc == 4'b1011) ? (signed_a < signed_b) : // Slt 操作时判断 a - b 是否小于 0                      
                      r[31]; // 其他操作直接依据 r[31]
endmodule
