`timescale 1ns/1ps

module Regfiles_tb;

    // 输入信号
    reg clk;
    reg rst;
    reg we;
    reg [4:0] raddr1;
    reg [4:0] raddr2;
    reg [4:0] waddr;
    reg [31:0] wdata;

    // 输出信号
    wire [31:0] rdata1;
    wire [31:0] rdata2;

    // 存储初始化数据
    reg [31:0] mem_init [0:31];
    integer i;
    
    // 实例化寄存器堆
    Regfiles uut (
        .clk(clk),
        .rst(rst),
        .we(we),
        .raddr1(raddr1),
        .raddr2(raddr2),
        .waddr(waddr),
        .wdata(wdata),
        .rdata1(rdata1),
        .rdata2(rdata2)
    );

    // 时钟生成
    initial begin
        clk = 1;
        forever #5 clk = ~clk; // 每 5ns 翻转一次时钟
    end

    // 初始化数据
    initial begin
        mem_init[0] = 32'h125ED79B;
        mem_init[1] = 32'h1717486A;
        mem_init[2] = 32'h78A0F299;
        mem_init[3] = 32'h602356EE;
        mem_init[4] = 32'h3DF045B7;
        mem_init[5] = 32'h9DC1AD7E;
        mem_init[6] = 32'h91DCA834;
        mem_init[7] = 32'hD49E900D;
        mem_init[8] = 32'h71A4E0BB;
        mem_init[9] = 32'h164D736A;
        mem_init[10] = 32'hA2A35E3A;
        mem_init[11] = 32'hCED8F75E;
        mem_init[12] = 32'hF6C2FD4A;
        mem_init[13] = 32'hD40D0447;
        mem_init[14] = 32'hA174B2C5;
        mem_init[15] = 32'h0CD35494;
        mem_init[16] = 32'hF8F0059D;
        mem_init[17] = 32'h9EFF173F;
        mem_init[18] = 32'h6323286A;
        mem_init[19] = 32'h15D26E90;
        mem_init[20] = 32'h5C56ED5E;
        mem_init[21] = 32'hF587E000;
        mem_init[22] = 32'h40FE8108;
        mem_init[23] = 32'h55C02FE8;
        mem_init[24] = 32'hCBE95B7B;
        mem_init[25] = 32'h72CA41B9;
        mem_init[26] = 32'h47BAE53A;
        mem_init[27] = 32'h98952334;
        mem_init[28] = 32'h3F4B9D64;
        mem_init[29] = 32'h817E2529;
        mem_init[30] = 32'h96D4FDB6;
        mem_init[31] = 32'h17CDA30D;
    end

    // 测试逻辑
    initial begin
        // 初始化信号
        rst = 1;
        we = 0;
        raddr1 = 0;
        raddr2 = 0;
        waddr = 0;
        wdata = 0;

        // 复位寄存器堆
        #10;
        rst = 0;

        // 写入 mem_init 数据
        for (i = 0; i < 32; i = i + 1) begin
            @(negedge clk); // 等待下降沿
            we = 1;         // 开启写使能
            waddr = i[4:0]; // 写地址
            wdata = mem_init[i]; // 写入对应数据
        end

        @(negedge clk);
        we = 0; // 停止写入

        // 读取并验证 mem_init 数据
        for (i = 0; i < 32; i = i + 1) begin
            @(negedge clk); // 等待下降沿
            raddr1 = i[4:0]; // 设置读地址
            #1; // 等待组合逻辑稳定
        end

        // 停止仿真
        #20;
        $stop;
    end

endmodule
