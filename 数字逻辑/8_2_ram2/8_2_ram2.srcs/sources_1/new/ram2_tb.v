module tb_ram2;

    // Testbench 信号定义
    reg clk;
    reg ena;
    reg wena;
    reg [4:0] addr;
    wire [31:0] data;
    reg [31:0] data_reg; // 用于模拟双向数据线

    // 双向数据线连接,在读模式下，将data设为高阻态，以允许ram输出
    assign data = (!wena && ena) ? 32'bz : data_reg;

    // 实例化 DUT
    ram2 uut (
        .clk(clk),
        .ena(ena),
        .wena(wena),
        .addr(addr),
        .data(data)
    );

    // 时钟信号生成
    initial begin
        clk = 0;
        forever #5 clk = ~clk; // 10时间单位为一个周期
    end

    // 手动初始化 RAM 内容
    initial begin
        // 初始化 RAM 的值（替代 $readmemh）
        uut.mem_array[0] = 32'h12345678; // 地址 0
        uut.mem_array[1] = 32'hABCDEF12; // 地址 1
        uut.mem_array[2] = 32'h98765432; // 地址 2
        uut.mem_array[3] = 32'h0FEDCBA9; // 地址 3
        // 其他地址保持未初始化（默认值）

        #10;
        // 初始化信号
        ena = 0;
        wena = 0;
        addr = 0;
        data_reg = 0;

        // 检查初始化的内容
        #10 ena = 1; wena = 0; addr = 5'd0; // 读取地址 0
        #10 addr = 5'd1;                   // 读取地址 1
        #10 addr = 5'd2;                   // 读取地址 2
        #10 addr = 5'd3;                   // 读取地址 3

        // 写入测试
        #10 wena = 1; addr = 5'd4; data_reg = 32'hDEADBEEF; // 写入地址 4

        // 读取刚写入的值
        #10 wena = 0; addr = 5'd4; // 读取地址 4

        // 禁止使能
        #10 ena = 0;

        // 结束仿真
        #20 $stop;
    end

endmodule
