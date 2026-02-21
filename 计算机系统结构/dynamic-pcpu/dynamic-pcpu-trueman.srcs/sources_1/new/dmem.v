`timescale 1ns / 1ps
module DMEM(
    input clk,
    input cs,
    input dm_w,
    input dm_r,
    input [1:0] wsel,         // 写入选择信号
    input [1:0] rsel,         // 读取选择信号
    input [31:0] data_in,
    input [6:0] addr,

    // input sb_flag,           // SB指令标志
    // input sh_flag,           // SH指令标志
    // input sw_flag,           // SW指令标志
    // input lb_flag,           // LB指令标志
    // input lh_flag,           // LH指令标志
    // input lbu_flag,          // LBU指令标志
    // input lhu_flag,          // LHU指令标志
    // input lw_flag,           // LW指令标志
    // 输出数据
    output reg [31:0] data_out
);

reg [31:0] dmem [2047:0];

// 构建完整的32位地址（假设基地址为0x10010000）
wire [31:0] full_addr = {25'b0, addr, 2'b00} + 32'h10010000;

wire [9:0] addr_hi = (full_addr - 32'h10010000) >> 2;
wire [1:0] addr_lo = (full_addr - 32'h10010000) & 2'b11;

always @(*) begin
    // 默认输出0
    data_out = 32'b0;
    
    // 只有当片选和读使能同时有效时才进行读取
    if (cs && dm_r) begin
        case(rsel)
            2'b01: // 字读取（32位）
            begin
                data_out = dmem[addr_hi];
            end
            2'b10: // 半字读取（16位）
            begin
                case(addr_lo)
                    2'b00: data_out = {16'b0, dmem[addr_hi][15:0]};
                    2'b10: data_out = {16'b0, dmem[addr_hi][31:16]};
                    default: data_out = 32'b0;
                endcase
            end
            2'b11: // 字节读取（8位）
            begin
                case(addr_lo)
                    2'b00: data_out = {24'b0, dmem[addr_hi][7:0]};
                    2'b01: data_out = {24'b0, dmem[addr_hi][15:8]};
                    2'b10: data_out = {24'b0, dmem[addr_hi][23:16]};
                    2'b11: data_out = {24'b0, dmem[addr_hi][31:24]};
                endcase
            end
            default: data_out = 32'b0;
        endcase
    end
end

always @(posedge clk) begin
    // 只有当片选和写使能同时有效时才进行写入
    if (cs && dm_w) begin
        case(wsel)
            2'b01: // 字写入（32位）
            begin
                dmem[addr_hi] <= data_in; 
            end
            2'b10: // 半字写入（16位）
            begin
                case(addr_lo)
                    2'b00: dmem[addr_hi][15:0] <= data_in[15:0];
                    2'b10: dmem[addr_hi][31:16] <= data_in[15:0];
                endcase
            end
            2'b11: // 字节写入（8位）
            begin
                case(addr_lo)
                    2'b00: dmem[addr_hi][7:0] <= data_in[7:0];
                    2'b01: dmem[addr_hi][15:8] <= data_in[7:0];
                    2'b10: dmem[addr_hi][23:16] <= data_in[7:0];
                    2'b11: dmem[addr_hi][31:24] <= data_in[7:0];
                endcase
            end
        endcase
    end
end

/*
// reg [31:0] dmem [0:31];

// wire [4:0] word_addr = addr[6:2];  // 字地址（高5位）
// wire [1:0] byte_sel = addr[1:0];   // 字节选择（低2位）

always @(*) begin
    data_out = 32'bz;  // 默认高阻态
    
    if (cs && dm_r && !dm_w) begin
        casez ({lb_flag, lbu_flag, lh_flag, lhu_flag, lw_flag})
            // LB: 字节读取（符号扩展）
            5'b1????: begin
                case (byte_sel)
                    2'b00: data_out = {{24{current_word[7]}}, current_word[7:0]};
                    2'b01: data_out = {{24{current_word[15]}}, current_word[15:8]};
                    2'b10: data_out = {{24{current_word[23]}}, current_word[23:16]};
                    2'b11: data_out = {{24{current_word[31]}}, current_word[31:24]};
                endcase
            end
            
            // LBU: 字节读取（零扩展）
            5'b?1???: begin
                case (byte_sel)
                    2'b00: data_out = {24'b0, current_word[7:0]};
                    2'b01: data_out = {24'b0, current_word[15:8]};
                    2'b10: data_out = {24'b0, current_word[23:16]};
                    2'b11: data_out = {24'b0, current_word[31:24]};
                endcase
            end
            
            // LH: 半字读取（符号扩展）
            5'b??1??: begin
                if (byte_sel[0] == 0) begin // 确保半字对齐
                    case (byte_sel[1])
                        1'b0: data_out = {{16{current_word[15]}}, current_word[15:0]};
                        1'b1: data_out = {{16{current_word[31]}}, current_word[31:16]};
                    endcase
                end
            end
            
            // LHU: 半字读取（零扩展）
            5'b???1?: begin
                if (byte_sel[0] == 0) begin // 确保半字对齐
                    case (byte_sel[1])
                        1'b0: data_out = {16'b0, current_word[15:0]};
                        1'b1: data_out = {16'b0, current_word[31:16]};
                    endcase
                end
            end
            
            // LW: 字读取
            5'b????1: begin
                if (byte_sel == 2'b00) begin // 确保字对齐
                    data_out = current_word;
                end
            end
            
            default: data_out = 32'bz;
        endcase
    end
end

always @(negedge clk) begin
    if (cs && dm_w && !dm_r) begin
        casez ({sb_flag, sh_flag, sw_flag})
            // SB: 写入字节
            3'b1??: begin
                case (byte_sel)
                    2'b00: dmem[word_addr][7:0] <= data_in[7:0];
                    2'b01: dmem[word_addr][15:8] <= data_in[7:0];
                    2'b10: dmem[word_addr][23:16] <= data_in[7:0];
                    2'b11: dmem[word_addr][31:24] <= data_in[7:0];
                endcase
            end
            
            // SH: 写入半字
            3'b?1?: begin
                if (byte_sel[0] == 0) begin // 确保半字对齐
                    case (byte_sel[1])
                        1'b0: dmem[word_addr][15:0] <= data_in[15:0];
                        1'b1: dmem[word_addr][31:16] <= data_in[15:0];
                    endcase
                end
            end
            
            // SW: 写入字
            3'b??1: begin
                if (byte_sel == 2'b00) begin // 确保字对齐
                    dmem[word_addr] <= data_in;
                end
            end
            
            default: ; // 无操作
        endcase
    end
end
*/

endmodule