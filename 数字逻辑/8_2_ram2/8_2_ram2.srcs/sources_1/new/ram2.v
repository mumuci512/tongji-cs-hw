module ram2 (
    input clk,            // 存储器时钟信号
    input ena,            // 存储器有效信号，高电平有效
    input wena,           // 存储器读写有效信号，高电平为写，低电平为读
    input [4:0] addr,     // 地址输入信号，指定读写地址
    inout [31:0] data     // 数据总线，既用于读，也用于写
);

    // 定义存储器数组，32个32位寄存器
    reg [31:0] mem_array [31:0];
    reg [31:0] data_out; // 内部寄存器，用于保存输出数据

    // 双向数据总线控制：当为读模式时输出数据，否则为高阻态
    assign data = (ena && !wena) ? data_out : 32'bz;

    // 写操作：在时钟上升沿执行
    always @(posedge clk) begin
        if (ena && wena) begin
            mem_array[addr] <= data; // 将输入数据写入指定地址
        end
    end

    // 读操作：组合逻辑，随时读取数据
    always @(*) begin
        if (ena && !wena) begin
            data_out = mem_array[addr]; // 从指定地址读取数据
        end else begin
            data_out = 32'bz; // 输出高阻态
        end
    end

endmodule
