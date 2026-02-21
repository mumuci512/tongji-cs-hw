module pipeWB(
    input [31:0]    wb_pc4,
    input [31:0]    wb_rs_data,
    input [31:0]    wb_hi,
    input [31:0]    wb_lo,
    input [31:0]    wb_cp0,
    // 程序地址、寄存器原始数据与CP0寄存器数据
    input [31:0]    wb_alu_out,
    input [31:0]    wb_mul_hi,
    input [31:0]    wb_mul_lo,
    input [31:0]    wb_rem,
    input [31:0]    wb_quo,
    // ALU运算结果与乘除法运算结果
    input [31:0]    wb_cnt,
    input [31:0]    wb_dm_data,
    input           wb_hi_we,
    input           wb_lo_we,
    input           wb_rf_we,
    // 前导零计数、访存数据与寄存器写使能信号
    input [1:0]     wb_hi_sel,
    input [1:0]     wb_lo_sel,
    input [2:0]     wb_rf_sel,
    input [4:0]     wb_wr_addr,
    // 各类寄存器写回选择信号与通用寄存器写地址
    
    // 写回数据输出
    output          wb_out_hi_we,
    output          wb_out_lo_we,
    output          wb_out_rf_we,
    output [4:0]    wb_out_wr_addr,
    // 寄存器写使能信号与通用寄存器写地址输出
    output [31:0]   wb_hi_data,
    output [31:0]   wb_lo_data,
    output [31:0]   wb_rf_data
    // HI/LO/通用寄存器写回数据输出
);

    // HI寄存器写回数据选择：4选1多路选择器，根据wb_hi_sel选择HI寄存器的写回数据
    Mux4To1_32Bit mux_hi(
        wb_rem, 
        wb_mul_hi, 
        wb_rs_data, 
        32'hz, 
        wb_hi_sel, 
        wb_hi_data
    );
    
    // LO寄存器写回数据选择：4选1多路选择器，根据wb_lo_sel选择LO寄存器的写回数据
    Mux4To1_32Bit mux_lo(
        wb_quo, 
        wb_mul_lo, 
        wb_rs_data, 
        32'hz, 
        wb_lo_sel, 
        wb_lo_data
    );
    
    // 寄存器文件写回数据选择：8选1多路选择器，根据wb_rf_sel选择通用寄存器的写回数据
    Mux8To1_32Bit mux_rf(
        wb_lo, 
        wb_pc4, 
        wb_cnt, 
        wb_cp0, 
        wb_dm_data, 
        wb_alu_out, 
        wb_hi, 
        wb_mul_lo, 
        wb_rf_sel, 
        wb_rf_data
    );
    
    // 写使能和地址直通：无需逻辑处理，直接映射输入到输出，传递写控制信号
    assign wb_out_hi_we  = wb_hi_we;
    assign wb_out_lo_we  = wb_lo_we;
    assign wb_out_rf_we  = wb_rf_we;
    assign wb_out_wr_addr = wb_wr_addr;

endmodule