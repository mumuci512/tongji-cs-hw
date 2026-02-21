module test_Divider;

    // 输入信号
    reg I_CLK;
    reg rst;

    // 输出信号
    wire O_CLK;

    // 实例化分频器
    Divider #(.DIV_RATIO(20)) uut (
        .I_CLK(I_CLK),
        .rst(rst),
        .O_CLK(O_CLK)
    );

    // 时钟生成
    always begin
        #5 I_CLK = ~I_CLK;  // 每5个时间单位翻转一次时钟
    end

    initial begin
        // 初始化信号
        I_CLK = 0;
        rst = 0;

        // 测试过程
        $display("Test begins...");
        rst = 1;  // 激活复位信号
        #10 rst = 0;  // 解除复位

        // 等待一段时间观察分频器的输出
        #200;

        // 结束模拟
        $stop;
    end

    // 监视输出信号
    initial begin
        $monitor("Time=%0t, I_CLK=%b, O_CLK=%b", $time, I_CLK, O_CLK);
    end

endmodule
