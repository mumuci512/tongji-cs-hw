`timescale 1ns / 1ps

module tb_JK_FF;

    // 输入信号
    reg CLK;
    reg J;
    reg K;
    reg RST_n;

    // 输出信号
    wire Q1;
    wire Q2;

    // 实例化被测试的异步复位JK触发器
    JK_FF uut (
        .CLK(CLK),
        .J(J),
        .K(K),
        .RST_n(RST_n),
        .Q1(Q1),
        .Q2(Q2)
    );

    // 时钟生成
    initial begin
        CLK = 0;
        forever #5 CLK = ~CLK;  // 10ns周期的时钟
    end

    // 测试过程
    initial begin
        // 初始化信号
        J = 0;
        K = 0;
        RST_n = 1;

        // 复位测试
        RST_n = 0; // 激活复位
        #10;       // 维持复位10ns
        RST_n = 1; // 取消复位
        #10;

        // 测试不同的J和K值
        J = 0; K = 0; #10; // 保持状态
        
        J = 0; K = 1; #10; // 清除状态
        
        J = 1; K = 0; #10; // 设置状态
        
        J = 1; K = 1; #10; // 取反状态
        
        // 再次清除状态
        J = 1; K = 0; #10; // 清除状态

        // 测试复位
        RST_n = 0; // 激活复位
        #10; 
        RST_n = 1; // 取消复位
        #10;

        // 结束仿真
        $finish;
    end

endmodule
