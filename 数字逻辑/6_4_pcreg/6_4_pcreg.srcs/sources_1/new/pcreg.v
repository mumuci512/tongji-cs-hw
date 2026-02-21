module pcreg(
    input clk,               // 时钟信号，上升沿时为PC寄存器赋值
    input rst,               // 异步重置信号，高电平时清零PC寄存器
    input ena,               // 有效信号，高电平时PC寄存器读取data_in
    input [31:0] data_in,    // 32位输入数据，将被存入寄存器
    output reg [31:0] data_out // 32位输出，始终输出PC寄存器的存储值
);

    // 异步复位和数据读入逻辑
    always @(posedge clk or posedge rst) begin
        if (rst) begin
            data_out <= 32'b0;         // 复位信号高时，清零寄存器
        end else if (ena) begin
            data_out <= data_in;       // ena有效时，将data_in的值存入寄存器
        end
        // 如果ena无效，则保持原有data_out值不变
    end

endmodule
