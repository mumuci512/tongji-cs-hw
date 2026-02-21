`timescale 1ns / 1ps
module DMEM (
    input         clk,        // 下降沿有效
    input         cs,
    input         dm_w,     
    input         dm_r,        
    input  [31:0] addr,      // 访问地址（需字对齐）
    input  [31:0] data_in,    // 写入数据
    output [31:0] data_out      // 读取数据
);

// 存储器定义（1KB容量，地址范围 0x0000~0x03FF）
reg [31:0] dmem [31:0];  // 256个32位存储单元（256*4=1024字节）

assign data_out = (cs && dm_r && !dm_w) ? dmem[addr] : 32'bz;

// 地址处理（取[9:2]位，忽略低2位）
wire [10:0] word_address = addr[10:0];  // 将字节地址转换为字地址

// 同步写入
always @(negedge clk && cs) begin
    if (dm_w && !dm_r && (word_address <= 8'hFF)) begin
        dmem[word_address] <= data_in;
    end
end

endmodule