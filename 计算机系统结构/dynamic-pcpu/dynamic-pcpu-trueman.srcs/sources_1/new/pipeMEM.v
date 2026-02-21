module pipeMEM(
    input           clk,
    input [31:0]    mem_pc4,
    input           ena,
    input [31:0]    mem_rs_data,
    input [31:0]    mem_rt_data,
    input [31:0]    mem_hi,
    // 时钟、使能信号与各类寄存器原始数据
    input [31:0]    mem_lo,
    input [31:0]    mem_cp0,
    input [31:0]    mem_alu_out,
    input [31:0]    mem_mul_hi,
    input [31:0]    mem_mul_lo,
    // CP0寄存器与各类运算结果数据
    input [31:0]    mem_rem,
    input [31:0]    mem_quo,
    input [31:0]    mem_cnt,
    input           mem_cutter_sign,
    input           mem_cutter_addr_sel,
    // 乘除法结果、辅助运算结果与数据截取控制信号
    input [2:0]     mem_cutter_sel,
    input [1:0]     mem_dm_wsel,
    input [1:0]     mem_dm_rsel,
    input           mem_dm_ena,
    input           mem_dm_we,
    // 数据截取类型与数据存储器控制信号
    input           mem_hi_we,
    input           mem_lo_we,
    input           mem_rf_we,
    input [1:0]     mem_hi_sel,
    input [1:0]     mem_lo_sel,
    // 寄存器写使能与写回选择信号
    input [2:0]     mem_rf_sel,
    input [4:0]     mem_wr_addr,
    // 通用寄存器写回选择信号与写地址
    
    // 访存输出
    output [31:0]   mem_out_pc4,
    output [31:0]   mem_out_rs_data,
    output [31:0]   mem_out_hi,
    output [31:0]   mem_out_lo,
    output [31:0]   mem_out_cp0,
    // 程序地址与各类寄存器数据直通输出
    output [31:0]   mem_out_alu_out,
    output [31:0]   mem_out_mul_hi,
    output [31:0]   mem_out_mul_lo,
    output [31:0]   mem_out_rem,
    output [31:0]   mem_out_quo,
    // 各类运算结果直通输出
    output [31:0]   mem_out_cnt,
    output [31:0]   mem_out_dm_data,
    output          mem_out_hi_we,
    output          mem_out_lo_we,
    output          mem_out_rf_we,
    // 前导零计数与访存处理后数据、寄存器写使能输出
    output [1:0]    mem_out_hi_sel,
    output [1:0]    mem_out_lo_sel,
    output [2:0]    mem_out_rf_sel,
    output [4:0]    mem_out_wr_addr
    // 寄存器写回选择信号与写地址直通输出
);

    // 内部信号：数据截取输入、数据存储器临时数据与存储器读写使能
    wire [31:0] cutter_in;
    wire [31:0] dm_data_temp;
    wire        dm_read_en;
    wire        dm_write_en;
    
    // 直通信号组：无需逻辑处理，直接映射输入到输出，传递无变化的信息
    // 辅助运算与ALU运算结果直通
    assign mem_out_cnt      = mem_cnt;
    assign mem_out_alu_out  = mem_alu_out;
    // 程序地址与乘除法结果直通
    assign mem_out_pc4      = mem_pc4;
    assign mem_out_quo      = mem_quo;
    assign mem_out_rem      = mem_rem;        
    assign mem_out_mul_hi   = mem_mul_hi;
    assign mem_out_mul_lo   = mem_mul_lo;
    // HI/LO寄存器数据与写使能信号直通
    assign mem_out_hi       = mem_hi;
    assign mem_out_lo       = mem_lo;
    assign mem_out_hi_we    = mem_hi_we;
    assign mem_out_lo_we    = mem_lo_we;
    // 通用寄存器写控制信号直通
    assign mem_out_rf_we    = mem_rf_we;
    assign mem_out_hi_sel   = mem_hi_sel;
    assign mem_out_lo_sel   = mem_lo_sel;    
    // RS寄存器数据与通用寄存器写回选择信号直通
    assign mem_out_rs_data  = mem_rs_data;    
    assign mem_out_rf_sel   = mem_rf_sel;
    assign mem_out_wr_addr  = mem_wr_addr;
    // CP0寄存器数据直通
    assign mem_out_cp0      = mem_cp0;    
    
    // 数据存储器使能信号：根据总使能与读写使能生成独立的读写控制
    assign dm_read_en       = mem_dm_ena && ~mem_dm_we;
    // 数据存储器读使能：总使能有效且写使能无效
    assign dm_write_en      = mem_dm_ena && mem_dm_we;
    // 数据存储器写使能：总使能有效且写使能有效
    
    // Cutter输入选择：2选1多路选择器，选择数据截取模块的输入数据源
    Mux2To1_32Bit mux_cutter(
        mem_rt_data, 
        dm_data_temp, 
        mem_cutter_addr_sel, 
        cutter_in
    );
    
    // Cutter实例化：对输入数据进行位宽截取与符号扩展，生成标准32位输出数据
    cutter cutter_inst(
        cutter_in, 
        mem_cutter_sel, 
        mem_cutter_sign, 
        mem_out_dm_data
    );
    
    // 数据存储器实例化：实现数据的读写操作，处理字节/半字/字类型的存储与读取
    DMEM dmem_inst(
        clk, 
        dm_read_en, 
        dm_write_en, 
        mem_dm_wsel, 
        mem_dm_rsel,
        mem_out_dm_data, 
        mem_alu_out, 
        dm_data_temp
    );

endmodule