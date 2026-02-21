module pipeEXMreg(
    input               clk,
    input               rst,
    
    // 执行阶段输入
    input [31:0]        ex_pc4,
    input [31:0]        ex_rs_data,
    input [31:0]        ex_rt_data,
    input [31:0]        ex_hi,
    input [31:0]        ex_lo,
    // 程序地址与通用/HI/LO寄存器数据
    input [31:0]        ex_cp0,
    input [31:0]        ex_alu_out,
    input [31:0]        ex_mul_hi,
    input [31:0]        ex_mul_lo,
    input [31:0]        ex_rem,
    // CP0寄存器与各类运算核心结果
    input [31:0]        ex_quo,
    input [31:0]        ex_cnt,
    input               ex_cutter_sign,
    input [2:0]         ex_cutter_sel,
    input               ex_cutter_addr_sel,
    // 除法结果、辅助运算与数据截取控制信号
    input               ex_dm_ena,
    input               ex_dm_we,
    input [1:0]         ex_dm_wsel,
    input [1:0]         ex_dm_rsel,
    input               ex_hi_we,
    // 数据存储器控制与HI寄存器写控制信号
    input               ex_lo_we,
    input               ex_rf_we,
    input [1:0]         ex_hi_sel,
    input [1:0]         ex_lo_sel,
    input [2:0]         ex_rf_sel,
    // LO寄存器与通用寄存器写回控制信号
    input [4:0]         ex_wr_addr,
    
    // 访存阶段输出
    output reg [31:0]   mem_pc4,
    output reg [31:0]   mem_rs_data,
    output reg [31:0]   mem_rt_data,
    output reg [31:0]   mem_hi,
    output reg [31:0]   mem_lo,
    // 程序地址与通用/HI/LO寄存器数据
    output reg [31:0]   mem_cp0,
    output reg [31:0]   mem_alu_out,
    output reg [31:0]   mem_mul_hi,
    output reg [31:0]   mem_mul_lo,
    output reg [31:0]   mem_rem,
    // CP0寄存器与各类运算核心结果
    output reg [31:0]   mem_quo,
    output reg [31:0]   mem_cnt,
    output reg          mem_cutter_sign,
    output reg          mem_cutter_addr_sel,
    output reg [2:0]    mem_cutter_sel,
    // 除法结果、辅助运算与数据截取控制信号
    output reg          mem_dm_ena,
    output reg          mem_dm_we,
    output reg [1:0]    mem_dm_wsel,
    output reg [1:0]    mem_dm_rsel,
    output reg          mem_rf_we,
    // 数据存储器控制与HI寄存器写控制信号
    output reg          mem_hi_we,
    output reg          mem_lo_we,
    output reg [1:0]    mem_hi_sel,
    output reg [1:0]    mem_lo_sel,
    output reg [2:0]    mem_rf_sel,
    // LO寄存器与通用寄存器写回控制信号
    output reg [4:0]    mem_wr_addr
);

    // 核心逻辑：时钟上升沿触发锁存，复位时清零所有输出寄存器
    always @(posedge clk or posedge rst) 
    begin
        if(rst) 
        begin
            // 复位状态：清空所有输出寄存器，防止无效数据流入MEM阶段
            mem_pc4             <= 32'b0;
            mem_alu_out         <= 32'b0;
            mem_mul_hi          <= 32'b0;
            mem_mul_lo          <= 32'b0;
            mem_rem             <= 32'b0;
            mem_quo             <= 32'b0;
            
            // 辅助运算与特权寄存器相关清零
            mem_cnt             <= 32'b0;
            mem_cp0             <= 32'b0;
            
            // 通用寄存器相关数据与地址清零
            mem_wr_addr         <= 5'b0;
            mem_rs_data         <= 32'b0;
            mem_rt_data         <= 32'b0;
                    
            // 数据截取与访存控制信号清零
            mem_cutter_sign     <= 1'b0;
            mem_cutter_addr_sel <= 1'b0;
            mem_cutter_sel      <= 3'b0;
            mem_dm_ena          <= 1'b0;
            mem_dm_we           <= 1'b0;
            mem_dm_wsel         <= 1'b0;
            mem_dm_rsel         <= 1'b0;
            
            // HI/LO寄存器与写回控制信号清零
            mem_hi              <= 32'b0;
            mem_lo              <= 32'b0;
            mem_rf_we           <= 1'b0;      
            mem_rf_sel          <= 3'b0;
                              
            mem_hi_we           <= 1'b0;
            mem_lo_we           <= 1'b0;
            mem_hi_sel          <= 2'b0;
            mem_lo_sel          <= 2'b0;
        end 
        else
        begin
            // 正常工作状态：逐位锁存EX阶段输入，完整传递至MEM阶段
            // 确保运算结果、控制信号稳定传输，为访存阶段提供完整输入
            mem_pc4             <= ex_pc4;    
            mem_alu_out         <= ex_alu_out;    
            mem_mul_hi          <= ex_mul_hi;
            mem_mul_lo          <= ex_mul_lo;
            mem_rem             <= ex_rem;
            mem_quo             <= ex_quo;        
            mem_cnt             <= ex_cnt;
            mem_rs_data         <= ex_rs_data;
            mem_rt_data         <= ex_rt_data;
            mem_rf_we           <= ex_rf_we;
            mem_wr_addr         <= ex_wr_addr;
                    
            // 传递特权寄存器与数据截取控制信号
            mem_cp0             <= ex_cp0;
            mem_cutter_sign     <= ex_cutter_sign;
            mem_cutter_addr_sel <= ex_cutter_addr_sel;
            mem_cutter_sel      <= ex_cutter_sel;
            mem_dm_ena          <= ex_dm_ena;
            mem_dm_we           <= ex_dm_we;
            mem_dm_wsel         <= ex_dm_wsel;
            mem_dm_rsel         <= ex_dm_rsel;
            
            // 传递HI/LO寄存器数据与写控制信号
            mem_hi              <= ex_hi;
            mem_lo              <= ex_lo;        
            mem_hi_we           <= ex_hi_we;
            mem_lo_we           <= ex_lo_we;
            mem_hi_sel          <= ex_hi_sel;
            mem_lo_sel          <= ex_lo_sel;
            mem_rf_sel          <= ex_rf_sel;
        end
    end

endmodule