`timescale 1ns / 1ps

module tb_pcreg;

    // 输入信号
    reg clk;
    reg rst;
    reg ena;
    reg [31:0] data_in;

    // 输出信号
    wire [31:0] data_out;

    // 实例化被测试的PC寄存器
    pcreg uut (
        .clk(clk),
        .rst(rst),
        .ena(ena),
        .data_in(data_in),
        .data_out(data_out)
    );

    // 时钟生成
    initial begin
        clk = 0;
        forever #5 clk = ~clk;  // 10ns周期的时钟
    end

    // 测试过程
    initial begin
        // 初始化信号
        data_in = 32'h12345678;
        rst = 0;
        ena = 0;

        // 应用复位
        $display("Applying reset...");
        rst = 1;
        #10;
        rst = 0;  // 取消复位
        #10;

        // 测试：复位后应保持为0
        $display("After reset, data_out = %h", data_out);

        // 测试：写入数据（ena = 1）
        ena = 1;
        data_in = 32'hA5A5A5A5;  // 写入数据
        #10;
        $display("ena = 1, data_in = %h, data_out = %h", data_in, data_out);

        // 测试：保持数据（ena = 0）
        ena = 0;
        data_in = 32'h5A5A5A5A;  // 修改data_in，但不应影响data_out
        #10;
        $display("ena = 0, data_in = %h, data_out = %h", data_in, data_out);

        // 测试：再次写入数据（ena = 1）
        ena = 1;
        data_in = 32'hFFFFFFFF;
        #10;
        $display("ena = 1, data_in = %h, data_out = %h", data_in, data_out);

        // 结束仿真
        $finish;
    end

endmodule
