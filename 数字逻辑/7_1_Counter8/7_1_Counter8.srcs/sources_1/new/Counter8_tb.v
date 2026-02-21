`timescale 1ns / 1ps

module tb_Counter8;  // Testbench模块

    // 输入信号
    reg CLK;           // 时钟信号
    reg rst_n;         // 复位信号，低电平有效

    // 输出信号
    wire [2:0] oQ;     // 三位计数器输出
    wire [6:0] oDisplay;  // 七段显示管输出

    // 实例化 Counter8 模块
    Counter8 uut (
        .CLK(CLK),
        .rst_n(rst_n),
        .oQ(oQ),
        .oDisplay(oDisplay)
    );

    // 时钟生成器
    always begin
        #5 CLK = ~CLK;  // 时钟周期 10ns (频率 100 MHz)
    end

    // 初始过程
    initial begin
        // 初始化信号
        CLK = 0;
        rst_n = 1;

        // 进行复位操作
        rst_n = 0;  // 低电平复位
        #10;         // 等待 10ns
        rst_n = 1;   // 解除复位

        // 等待几个时钟周期，观察计数器和七段显示的行为
        #100;        // 等待 100ns

        // 模拟过程完成
        $finish;
    end

endmodule
