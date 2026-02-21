module tb_ram;

    reg clk;
    reg ena;
    reg wena;
    reg [4:0] addr;
    reg [31:0] data_in;
    wire [31:0] data_out;

    ram uut (
        .clk(clk),
        .ena(ena),
        .wena(wena),
        .addr(addr),
        .data_in(data_in),
        .data_out(data_out)
    );

    // 初始化时钟信号
    initial begin
        clk = 0;
        forever #5 clk = ~clk; // 时钟周期10单位时间
    end

    // 测试程序
    initial begin
        // 初始化
        ena = 0;
        wena = 0;
        addr = 0;
        data_in = 0;

        // 复位
        #10 ena = 1; wena = 1; addr = 5'd0; data_in = 32'hA5A5A5A5; // 写入地址0
        #10 wena = 1; addr = 5'd1; data_in = 32'h5A5A5A5A;         // 写入地址1
        #10 wena = 0; addr = 5'd0;                                // 读取地址0
        #10 wena = 0; addr = 5'd1;                                // 读取地址1
        #10 ena = 0;                                             // 关闭使能，输出高阻态

        #20 $stop;
    end
endmodule
