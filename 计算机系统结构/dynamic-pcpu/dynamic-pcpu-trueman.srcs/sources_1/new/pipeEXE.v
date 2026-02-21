module pipeEX(
    input           rst,
    input [31:0]    ex_pc4,
    input [31:0]    ex_imm,
    input [31:0]    ex_shamt,
    input [31:0]    ex_rs_data,
    input [31:0]    ex_rt_data,
    // 基础运算数据与立即数、移位量
    input [31:0]    ex_hi,
    input [31:0]    ex_lo,
    input [31:0]    ex_cp0,
    input           ex_a_sel,
    input [1:0]     ex_b_sel,
    // HI/LO/CP0寄存器数据与ALU操作数选择信号
    input [3:0]     ex_alu_op,
    input           ex_mul_ena,
    input           ex_div_ena,
    input           ex_mul_sign,
    input           ex_div_sign,
    // ALU操作码与乘除法使能、符号控制
    input           ex_cutter_sign,
    input           ex_cutter_addr_sel,
    input [2:0]     ex_cutter_sel,
    input           ex_dm_ena,
    input           ex_dm_we,
    // 数据截取控制与数据存储器使能、写使能
    input [1:0]     ex_dm_wsel,
    input [1:0]     ex_dm_rsel,
    input           ex_rf_we,
    input           ex_hi_we,
    input           ex_lo_we,
    // 数据存储器位宽选择与寄存器写使能
    input [1:0]     ex_hi_sel,
    input [1:0]     ex_lo_sel,
    input [2:0]     ex_rf_sel,
    input [4:0]     ex_wr_addr,
    // 寄存器写回选择信号与写地址
    
    // 执行结果输出
    output [31:0]   ex_out_pc4,
    output [31:0]   ex_mul_hi,
    output [31:0]   ex_mul_lo,
    output [31:0]   ex_rem,
    output [31:0]   ex_quo,
    // 程序地址与乘除法运算结果
    output [31:0]   ex_out_rs_data,
    output [31:0]   ex_out_rt_data,
    output [31:0]   ex_out_hi,
    output [31:0]   ex_out_lo,
    output [31:0]   ex_out_cp0,
    // 寄存器原始数据直通输出
    output [31:0]   ex_cnt,
    output [31:0]   ex_alu_out,
    // 前导零计数结果与ALU运算结果
    
    // 传递控制信号
    output          ex_out_cutter_sign,
    output          ex_out_cutter_addr_sel,
    output [2:0]    ex_out_cutter_sel,
    output          ex_out_dm_ena,
    output          ex_out_dm_we,
    // 数据截取与存储器控制信号直通
    output [1:0]    ex_out_dm_wsel,
    output [1:0]    ex_out_dm_rsel,
    output          ex_out_hi_we,
    output          ex_out_lo_we,
    output          ex_out_rf_we,
    // 存储器位宽与寄存器写使能信号直通
    output [1:0]    ex_out_hi_sel,
    output [1:0]    ex_out_lo_sel,
    output [2:0]    ex_out_rf_sel,
    output [4:0]    ex_out_wr_addr
    // 寄存器写回选择与地址信号直通
);

    // 直通信号组：无需逻辑处理，直接将输入信号映射到输出，用于传递无变化的信息
    // 程序地址直通：保持PC+4地址不变，传递至后续流水线阶段
    assign ex_out_pc4           = ex_pc4;
    
    // 数据存储器相关控制信号直通：传递访存阶段所需的读写控制参数
    assign ex_out_dm_ena        = ex_dm_ena;
    assign ex_out_dm_we         = ex_dm_we;
    assign ex_out_dm_rsel       = ex_dm_rsel;
    assign ex_out_dm_wsel       = ex_dm_wsel;
    // 数据截取相关控制信号直通：传递数据位宽调整的控制信息
    assign ex_out_cutter_sign   = ex_cutter_sign;
    assign ex_out_cutter_addr_sel = ex_cutter_addr_sel;
    assign ex_out_cutter_sel    = ex_cutter_sel;
        
    // 通用寄存器与CP0寄存器数据直通：保留原始读取数据，供后续阶段使用
    assign ex_out_rs_data       = ex_rs_data;
    assign ex_out_rt_data       = ex_rt_data;
    assign ex_out_cp0           = ex_cp0;
    // 通用寄存器写回控制信号直通：传递寄存器写使能、数据选择与目标地址
    assign ex_out_rf_we         = ex_rf_we;
    assign ex_out_rf_sel        = ex_rf_sel;
    assign ex_out_wr_addr       = ex_wr_addr;
        
    // HI/LO寄存器相关信号直通：传递乘除法结果寄存器的读写控制与原始数据
    assign ex_out_hi_we         = ex_hi_we;
    assign ex_out_lo_we         = ex_lo_we;
    assign ex_out_hi            = ex_hi;
    assign ex_out_lo            = ex_lo;    
    assign ex_out_hi_sel        = ex_hi_sel;
    assign ex_out_lo_sel        = ex_lo_sel;  
    
    // ALU操作数选择：定义ALU输入A、B的中间连线
    wire [31:0] alu_a;
    wire [31:0] alu_b;
    wire zero, carry, negative, overflow;
    
    // 2选1多路选择器：选择ALU的输入A（移位量或RS寄存器数据）
    Mux2To1_32Bit mux_alu_a(
        ex_shamt, 
        ex_rs_data, 
        ex_a_sel, 
        alu_a
    );
    
    // 4选1多路选择器：选择ALU的输入B（RT寄存器数据或立即数）
    Mux4To1_32Bit mux_alu_b(
        ex_rt_data, 
        ex_imm, 
        32'bz, 
        32'bz, 
        ex_b_sel, 
        alu_b
    );
    
    // ALU实例化：执行算术逻辑运算，输出运算结果与标志位
    alu alu_inst(
        alu_a, 
        alu_b, 
        ex_alu_op, 
        ex_alu_out, 
        zero, 
        carry, 
        negative, 
        overflow
    );
    
    // 乘法器实例化：执行32位有符号/无符号乘法，输出高低32位结果
    MULT mult_inst(
        rst, 
        ex_mul_ena, 
        ex_mul_sign, 
        ex_rs_data, 
        ex_rt_data, 
        ex_mul_hi, 
        ex_mul_lo
    );
    
    // 除法器实例化：执行32位有符号/无符号除法，输出商与余数
    DIV div_inst(
        rst, 
        ex_div_ena, 
        ex_div_sign, 
        ex_rs_data, 
        ex_rt_data, 
        ex_quo, 
        ex_rem
    );
    
    // 前导零计数器实例化：统计RS寄存器数据的前导零个数
    clz clz_inst(
        ex_rs_data, 
        ex_cnt
    );

endmodule