module ram (
    input clk,               // 存储器时钟信号
    input ena,               // 存储器使能信号，高电平有效
    input wena,              // 读写控制信号，高电平为写入，低电平为读取
    input [4:0] addr,        // 输入地址
    input [31:0] data_in,    // 写入的数据
    output reg [31:0] data_out // 读出的数据
);

    // 定义存储器数组，大小为32x32位 (32个地址，每个32位数据)
    reg [31:0] mem_array [31:0]; 

    // 写操作：在时钟上升沿执行
    always @(posedge clk) begin
        if (ena && wena) begin
            mem_array[addr] <= data_in; // 写入数据到指定地址
        end
    end

    // 读操作：非时钟依赖
    always @(*) begin
        if (ena && !wena) begin
            data_out = mem_array[addr]; // 从指定地址读取数据
        end else begin
            data_out = 32'bz; // 当未使能或处于写入模式时，输出高阻态
        end
    end

endmodule
