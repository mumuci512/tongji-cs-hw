`timescale 1ns / 1ps
module DMEM(
    input clk,               // 时钟信号（写操作使用）
    input cs,                // 片选信号（高有效）
    input dm_w,              // 写使能（高有效）
    input dm_r,              // 读使能（高有效）
    input [6:0] addr,        // 7位字节地址
    input [31:0] data_in,    // 写入数据
    input sb_flag,           // SB指令标志
    input sh_flag,           // SH指令标志
    input sw_flag,           // SW指令标志
    input lb_flag,           // LB指令标志
    input lh_flag,           // LH指令标志
    input lbu_flag,          // LBU指令标志
    input lhu_flag,          // LHU指令标志
    input lw_flag,           // LW指令标志
    output reg [31:0] data_out // 读取数据
);

// 32个32位寄存器（128字节存储空间）
reg [31:0] dmem [0:31];

// 地址分解
wire [4:0] word_addr = addr[6:2];  // 字地址（高5位）
wire [1:0] byte_sel = addr[1:0];   // 字节选择（低2位）

// 当前选中的存储字
wire [31:0] current_word = dmem[word_addr];

// 读操作（组合逻辑）
always @(*) begin
    data_out = 32'bz;  // 默认高阻态
    
    if (cs && dm_r && !dm_w) begin
        casez ({lb_flag, lbu_flag, lh_flag, lhu_flag, lw_flag})
            // LB: 字节加载（符号扩展）
            5'b1????: begin
                case (byte_sel)
                    2'b00: data_out = {{24{current_word[7]}}, current_word[7:0]};
                    2'b01: data_out = {{24{current_word[15]}}, current_word[15:8]};
                    2'b10: data_out = {{24{current_word[23]}}, current_word[23:16]};
                    2'b11: data_out = {{24{current_word[31]}}, current_word[31:24]};
                endcase
            end
            
            // LBU: 字节加载（零扩展）
            5'b?1???: begin
                case (byte_sel)
                    2'b00: data_out = {24'b0, current_word[7:0]};
                    2'b01: data_out = {24'b0, current_word[15:8]};
                    2'b10: data_out = {24'b0, current_word[23:16]};
                    2'b11: data_out = {24'b0, current_word[31:24]};
                endcase
            end
            
            // LH: 半字加载（符号扩展）
            5'b??1??: begin
                if (byte_sel[0] == 0) begin // 检查半字对齐
                    case (byte_sel[1])
                        1'b0: data_out = {{16{current_word[15]}}, current_word[15:0]};
                        1'b1: data_out = {{16{current_word[31]}}, current_word[31:16]};
                    endcase
                end
            end
            
            // LHU: 半字加载（零扩展）
            5'b???1?: begin
                if (byte_sel[0] == 0) begin // 检查半字对齐
                    case (byte_sel[1])
                        1'b0: data_out = {16'b0, current_word[15:0]};
                        1'b1: data_out = {16'b0, current_word[31:16]};
                    endcase
                end
            end
            
            // LW: 字加载
            5'b????1: begin
                if (byte_sel == 2'b00) begin // 检查字对齐
                    data_out = current_word;
                end
            end
            
            default: data_out = 32'bz;
        endcase
    end
end

// 写操作（时钟下降沿触发）
always @(negedge clk) begin
    if (cs && dm_w && !dm_r) begin
        casez ({sb_flag, sh_flag, sw_flag})
            // SB: 存储字节
            3'b1??: begin
                case (byte_sel)
                    2'b00: dmem[word_addr][7:0] <= data_in[7:0];
                    2'b01: dmem[word_addr][15:8] <= data_in[7:0];
                    2'b10: dmem[word_addr][23:16] <= data_in[7:0];
                    2'b11: dmem[word_addr][31:24] <= data_in[7:0];
                endcase
            end
            
            // SH: 存储半字
            3'b?1?: begin
                if (byte_sel[0] == 0) begin // 检查半字对齐
                    case (byte_sel[1])
                        1'b0: dmem[word_addr][15:0] <= data_in[15:0];
                        1'b1: dmem[word_addr][31:16] <= data_in[15:0];
                    endcase
                end
            end
            
            // SW: 存储字
            3'b??1: begin
                if (byte_sel == 2'b00) begin // 检查字对齐
                    dmem[word_addr] <= data_in;
                end
            end
            
            default: ; // 无操作
        endcase
    end
end

endmodule