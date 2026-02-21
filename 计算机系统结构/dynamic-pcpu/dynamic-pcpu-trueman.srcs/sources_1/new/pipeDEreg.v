module pipeIDEreg(
    input               clk,
    input               rst,
    input               stall,
    
    // 译码阶段输入信号
    input [5:0]         id_op,
    input [5:0]         id_func,
    input [31:0]        id_pc4,         // ID阶段PC+4地址
    input [31:0]        id_imm,
    input [31:0]        id_shamt,       // ID阶段符号扩展后的移位量
    input [31:0]        id_rs_data,
    input [31:0]        id_rt_data,
    input [31:0]        id_hi,
    input [31:0]        id_lo,
    input [31:0]        id_cp0,
    input               id_a_sel,       // ID阶段ALU输入A选择信号
    input [1:0]         id_b_sel,       // ID阶段ALU输入B选择信号
    input [3:0]         id_alu_op,
    input               id_mul_ena,
    input               id_div_ena,
    input               id_mul_sign,
    input               id_div_sign,
    input               id_cutter_sign, // ID阶段数据截取符号扩展控制信号
    input               id_cutter_addr_sel, // ID阶段数据截取地址选择信号
    input [2:0]         id_cutter_sel,
    input               id_dm_ena,
    input               id_dm_we,
    input [1:0]         id_dm_wsel,
    input [1:0]         id_dm_rsel,
    input               id_hi_we,
    input               id_lo_we,
    input               id_rf_we,       // ID阶段通用寄存器写使能信号
    input [1:0]         id_hi_sel,
    input [1:0]         id_lo_sel,
    input [2:0]         id_rf_sel,     // ID阶段通用寄存器写回数据选择信号
    input [4:0]         id_wr_addr,    // ID阶段通用寄存器写地址

    // 执行阶段输出信号
    output reg [5:0]    ex_op,
    output reg [5:0]    ex_func,
    output reg [31:0]   ex_pc4,        // EX阶段PC+4地址
    output reg [31:0]   ex_imm,
    output reg [31:0]   ex_shamt,
    output reg [31:0]   ex_rs_data,
    output reg [31:0]   ex_rt_data,
    output reg [31:0]   ex_hi,
    output reg [31:0]   ex_lo,
    output reg [31:0]   ex_cp0,
    output reg          ex_a_sel,
    output reg [1:0]    ex_b_sel,
    output reg [3:0]    ex_alu_op,      // EX阶段ALU操作控制码
    output reg          ex_mul_ena,
    output reg          ex_div_ena,
    output reg          ex_mul_sign,
    output reg          ex_div_sign,
    output reg          ex_cutter_sign,
    output reg          ex_cutter_addr_sel,
    output reg [2:0]    ex_cutter_sel,  // EX阶段数据截取类型选择信号
    output reg          ex_dm_ena,
    output reg          ex_dm_we,
    output reg [1:0]    ex_dm_wsel,
    output reg [1:0]    ex_dm_rsel,
    output reg          ex_rf_we,
    output reg          ex_hi_we,
    output reg          ex_lo_we,
    output reg [1:0]    ex_hi_sel,
    output reg [1:0]    ex_lo_sel,
    output reg [2:0]    ex_rf_sel,      // EX阶段通用寄存器写回数据选择信号
    output reg [4:0]    ex_wr_addr      // EX阶段通用寄存器写地址
);

    // 核心逻辑：时钟上升沿触发锁存，复位或暂停时清零所有输出
    always @(posedge clk or posedge rst) 
    begin
        if(rst || stall) 
        begin
            // 复位/暂停状态：清空所有输出寄存器，防止无效数据流入EX阶段
            // 指令基础信息清零
            ex_op              <= 6'b0;
            ex_func            <= 6'b0;
            ex_imm             <= 32'b0;
            ex_shamt           <= 32'b0;
            ex_pc4             <= 32'b0;
                                                
            // HI/LO寄存器相关信号清零：避免乘除法运算出现无效数据
            ex_hi              <= 32'b0;
            ex_lo              <= 32'b0;
            ex_hi_we           <= 1'b0;
            ex_lo_we           <= 1'b0;
            ex_hi_sel          <= 2'b0;
            ex_lo_sel          <= 2'b0;
                                    
            // 数据存储器与截取相关信号清零：防止访存阶段出现误操作
            ex_dm_ena          <= 1'b0;
            ex_dm_we           <= 1'b0;
            ex_dm_wsel         <= 2'b0;
            ex_dm_rsel         <= 2'b0;            
            ex_cutter_sign     <= 1'b0;
            ex_cutter_addr_sel <= 1'b0;
            ex_cutter_sel      <= 3'b0;
            
            // 通用寄存器相关信号清零：避免寄存器堆出现错误写操作
            ex_rs_data         <= 32'b0;
            ex_rt_data         <= 32'b0;
            ex_rf_we           <= 1'b0;
            ex_rf_sel          <= 3'b0;
            ex_wr_addr         <= 5'b0;
            
            // CP0与运算单元相关信号清零：保证执行阶段运算逻辑稳定
            ex_cp0             <= 32'b0;
            ex_a_sel           <= 1'b0;
            ex_b_sel           <= 1'b0;
            ex_alu_op          <= 4'b0;
            ex_mul_ena         <= 1'b0;
            ex_mul_sign        <= 1'b0;            
            ex_div_ena         <= 1'b0;
            ex_div_sign        <= 1'b0;
        end
        else
        begin
            // 正常工作状态：逐位锁存ID阶段输入，传递至EX阶段
            // 确保指令信息、运算数据与控制信号的完整且稳定传输
            
            // 传递指令基础信息：为EX阶段指令解码提供依据
            ex_op              <= id_op;
            ex_func            <= id_func;
            ex_imm             <= id_imm;
            ex_shamt           <= id_shamt;
            ex_pc4             <= id_pc4;
            
            // 传递HI/LO寄存器相关信号：为乘除法运算提供原始数据与控制
            ex_hi              <= id_hi;
            ex_lo              <= id_lo;
            ex_hi_we           <= id_hi_we;
            ex_lo_we           <= id_lo_we;
            ex_hi_sel          <= id_hi_sel;
            ex_lo_sel          <= id_lo_sel;
                                                
            // 传递数据存储器控制信号：提前配置访存阶段的读写参数
            ex_dm_ena          <= id_dm_ena;
            ex_dm_we           <= id_dm_we;
            ex_dm_wsel         <= id_dm_wsel;
            ex_dm_rsel         <= id_dm_rsel;

            // 传递CP0与数据截取信号：为特权指令执行与数据位宽调整提供支持
            ex_cp0             <= id_cp0;
            ex_cutter_sign     <= id_cutter_sign;
            ex_cutter_addr_sel <= id_cutter_addr_sel;
            ex_cutter_sel      <= id_cutter_sel;
            
            // 传递ALU与乘除法控制信号：配置执行阶段的运算类型与模式
            ex_a_sel           <= id_a_sel;
            ex_b_sel           <= id_b_sel;
            ex_alu_op          <= id_alu_op;            
            ex_mul_ena         <= id_mul_ena;
            ex_div_ena         <= id_div_ena;
            ex_mul_sign        <= id_mul_sign;
            ex_div_sign        <= id_div_sign;

            // 传递通用寄存器相关信号：为运算提供源数据，为写回阶段配置目标参数
            ex_rf_we           <= id_rf_we;
            ex_rs_data         <= id_rs_data;
            ex_rt_data         <= id_rt_data;
            ex_rf_sel          <= id_rf_sel;
            ex_wr_addr         <= id_wr_addr;
        end
    end 
endmodule