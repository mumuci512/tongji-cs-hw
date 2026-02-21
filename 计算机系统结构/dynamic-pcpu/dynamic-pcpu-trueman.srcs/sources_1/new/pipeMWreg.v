module pipeMWreg(
    input               clk,
    input               rst,
    
    // 访存阶段输入
    input [31:0]        mem_pc4,
    input [31:0]        mem_rs_data,
    input [31:0]        mem_hi,
    input [31:0]        mem_lo,
    input [31:0]        mem_cp0,
    // 程序地址、寄存器原始数据与CP0寄存器数据
    input [31:0]        mem_alu_out,
    input [31:0]        mem_mul_hi,
    input [31:0]        mem_mul_lo,
    input [31:0]        mem_rem,
    input [31:0]        mem_quo,
    // ALU运算结果与乘除法运算结果
    input [31:0]        mem_cnt,
    input [31:0]        mem_dm_data,
    input               mem_hi_we,
    input               mem_lo_we,
    // 前导零计数、访存处理后数据与HI/LO寄存器写使能
    input               mem_rf_we,
    input [1:0]         mem_hi_sel,
    input [1:0]         mem_lo_sel,
    input [2:0]         mem_rf_sel,
    // 通用寄存器写使能与各类寄存器写回选择信号
    input [4:0]         mem_wr_addr,
    // 通用寄存器写地址
    
    // 写回阶段输出
    output reg [31:0]   wb_pc4,
    output reg [31:0]   wb_rs_data,
    output reg [31:0]   wb_hi,
    output reg [31:0]   wb_lo,
    output reg [31:0]   wb_cp0,
    // 程序地址、寄存器原始数据与CP0寄存器数据
    output reg [31:0]   wb_alu_out,
    output reg [31:0]   wb_mul_hi,
    output reg [31:0]   wb_mul_lo,
    output reg [31:0]   wb_rem,
    output reg [31:0]   wb_quo,
    // ALU运算结果与乘除法运算结果
    output reg [31:0]   wb_cnt,
    output reg [31:0]   wb_dm_data,
    output reg          wb_hi_we,
    output reg          wb_lo_we,
    // 前导零计数、访存处理后数据与HI/LO寄存器写使能
    output reg          wb_rf_we,
    output reg [1:0]    wb_hi_sel,
    output reg [1:0]    wb_lo_sel,
    output reg [2:0]    wb_rf_sel,
    // 通用寄存器写使能与各类寄存器写回选择信号
    output reg [4:0]    wb_wr_addr
    // 通用寄存器写地址
);

    // 核心逻辑：时钟上升沿触发锁存，复位时清零所有输出寄存器
    always @(posedge clk or posedge rst) 
    begin
        if(rst)
        begin
            // 复位状态：清空所有输出寄存器，防止无效数据流入WB阶段
            // HI/LO寄存器相关数据与控制信号清零
            wb_pc4          <= 32'b0;
            wb_hi           <= 32'b0;
            wb_lo           <= 32'b0;
            wb_hi_we        <= 1'b0;
            wb_lo_we        <= 1'b0;
            wb_hi_sel       <= 2'b0;
            wb_lo_sel       <= 2'b0;
                        
            // CP0寄存器与各类运算结果数据清零
            wb_cp0          <= 32'b0;
            wb_alu_out      <= 32'b0;
            wb_mul_hi       <= 32'b0;
            wb_mul_lo       <= 32'b0;
            wb_rem          <= 32'b0;
            wb_quo          <= 32'b0;
            wb_cnt          <= 32'b0;
            wb_dm_data      <= 32'b0;
            
            // 通用寄存器相关数据与控制信号清零
            wb_rs_data      <= 32'b0;            
            wb_rf_we        <= 1'b0;
            wb_rf_sel       <= 3'b0;
            wb_wr_addr      <= 5'b0;
        end
        else
        begin
            // 正常工作状态：逐位锁存MEM阶段输入，完整传递至WB阶段
            // 确保各类数据与控制信号稳定传输，为写回阶段提供完整输入
            wb_pc4          <= mem_pc4;		  
            wb_cp0          <= mem_cp0;
            wb_alu_out      <= mem_alu_out;
            wb_mul_hi       <= mem_mul_hi;			
            wb_mul_lo       <= mem_mul_lo;
            wb_rem          <= mem_rem;			
            wb_quo          <= mem_quo;
            wb_cnt          <= mem_cnt;
            wb_dm_data      <= mem_dm_data;
            wb_hi           <= mem_hi;
            wb_lo           <= mem_lo;            
            wb_hi_we        <= mem_hi_we;
            wb_lo_we        <= mem_lo_we;
            wb_hi_sel       <= mem_hi_sel;
            wb_lo_sel       <= mem_lo_sel;
            wb_rf_we        <= mem_rf_we;            
            wb_rs_data      <= mem_rs_data;            
            wb_rf_sel       <= mem_rf_sel;
            wb_wr_addr      <= mem_wr_addr;
        end
    end 

endmodule