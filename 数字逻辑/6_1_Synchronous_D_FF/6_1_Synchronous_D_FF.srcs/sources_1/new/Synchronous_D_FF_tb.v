`timescale 1ns / 1ps

module tb_Synchronous_D_FF;

    // 输入信号
    reg CLK;
    reg D;
    reg RST_n;

    // 输出信号
    wire Q1;
    wire Q2;

    // 实例化被测试的同步复位D触发器
    Synchronous_D_FF uut (
        .CLK(CLK),
        .D(D),
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
        D = 0;
        RST_n = 1;

        // 复位测试
        RST_n = 0; // 激活复位
        #10;       // 维持复位10ns
        RST_n = 1; // 取消复位
        #10;

        // 测试不同D值
        D = 1; #10; // D = 1
        
        D = 0; #10; // D = 0
        
        D = 1; #10; // D = 1
        
        // 测试复位
        RST_n = 0; // 激活复位
        #10; 
        RST_n = 1; // 取消复位
        #10;

        // 结束仿真
        $finish;
    end

endmodule
