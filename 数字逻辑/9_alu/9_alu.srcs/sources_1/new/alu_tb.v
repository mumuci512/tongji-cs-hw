module alu_tb;
    // 定义输入输出信号
    reg [31:0] a, b;
    reg [3:0] aluc;
    wire [31:0] r;
    wire zero, carry, negative, overflow;

    // 实例化被测试的ALU模块
    alu uut (
        .a(a),
        .b(b),
        .aluc(aluc),
        .r(r),
        .zero(zero),
        .carry(carry),
        .negative(negative),
        .overflow(overflow)
    );

    // 测试各个操作
    initial begin

        // 测试 Addu（无符号加法）
        a = 32'd10;
        b = 32'd20;
        aluc = 4'b0000;
        #10;

        // 测试 Add（有符号加法）
        a = 32'h7FFFFFFF; // 最大正数
        b = 32'd1;
        aluc = 4'b0010;
        #10;

        // 测试 Subu（无符号减法）
        a = 32'd20;
        b = 32'd30;
        aluc = 4'b0001;
        #10;

        // 测试 Sub（有符号减法）
        a = 32'd10;
        b = 32'd20;
        aluc = 4'b0011;
        #10;

        // 测试 And
        a = 32'hF0F0F0F0;
        b = 32'h0F0F0F0F;
        aluc = 4'b0100;
        #10;

        // 测试 Or
        a = 32'hF0F0F0F0;
        b = 32'h0F0F0F0F;
        aluc = 4'b0101;
        #10;

        // 测试 Xor
        a = 32'hF0F0F0F0;
        b = 32'h0F0F0F0F;
        aluc = 4'b0110;
        #10;

        // 测试 Nor
        a = 32'hF0F0F0F0;
        b = 32'h0F0F0F0F;
        aluc = 4'b0111;
        #10;

        // 测试 Lui（置高位立即数）
        a = 32'd0;
        b = 32'h1234;
        aluc = 4'b1000;
        #10;

        // 测试 Slt（有符号比较）
        a = -32'd10;
        b = 32'd20;
        aluc = 4'b1011;
        #10;

        // 测试 Sltu（无符号比较）
        a = 32'd10;
        b = 32'd20;
        aluc = 4'b1010;
        #10;

        // 测试 Sra（算术右移）
        a = 32'd2;
        b = -32'd16;
        aluc = 4'b1100;
        #10;

        // 测试 Sll（逻辑左移）
        a = 32'd4;
        b = 32'h1;
        aluc = 4'b1110;
        #10;

        // 测试 Srl（逻辑右移）
        a = 32'd4;
        b = 32'hFFFFFFFF;
        aluc = 4'b1101;
        #10;

        // 检查零标志位
        a = 32'd0;
        b = 32'd0;
        aluc = 4'b0000;
        #10;

        $finish;
    end
endmodule
