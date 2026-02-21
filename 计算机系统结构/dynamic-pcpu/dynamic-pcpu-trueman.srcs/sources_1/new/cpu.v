`timescale 1ns / 1ps

module cpu(
    input           clk,        // 时钟信号
    input           rst,        // 复位信号
    input           ena,        // CPU使能信号
    output [31:0]   pc,         // 程序计数器输出
    output [31:0]   inst,       // 当前指令
    output [31:0]   a,       // 寄存器6的值（用于调试）
    output [31:0]   b,       // 寄存器7的值（用于调试）
    output [31:0]   c,      // 寄存器15的值（用于调试）
    output [31:0]   d       // 寄存器16的值（用于调试）
);

    // 全局控制信号
    wire stall;     // 流水线暂停信号
    wire branch;    // 分支跳转信号

    // IF阶段信号：取指阶段相关数据与地址
    wire [31:0] if_npc;     // 下一个PC值
    wire [31:0] if_pc4;     // PC+4值

    // ID阶段信号：译码阶段相关数据与控制信号
    wire [31:0] id_pc4;     // ID阶段的PC+4
    wire [31:0] id_inst;    // ID阶段的指令
    wire [2:0]  id_pc_sel;  // PC选择控制（异常/分支/跳转/正常）
    wire [5:0]  id_op;      // 操作码
    wire [5:0]  id_func;    // 功能码
    wire [31:0] id_imm;     // 扩展后的立即数
    wire [31:0] id_shamt;   // 位移量（符号扩展后）
    wire [31:0] id_pc4_out; // ID输出的PC+4
    wire [31:0] id_eaddr;   // 异常地址
    wire [31:0] id_baddr;   // 分支地址
    wire [31:0] id_jaddr;   // 跳转地址
    wire [31:0] id_raddr;   // 寄存器跳转地址
    wire [31:0] id_rs_data; // RS寄存器数据
    wire [31:0] id_rt_data; // RT寄存器数据
    wire [31:0] id_hi_data; // HI寄存器数据
    wire [31:0] id_lo_data; // LO寄存器数据
    wire [31:0] id_cp0_data;// CP0寄存器数据
    wire        id_alu_a_sel;  // ALU输入A选择
    wire [1:0]  id_alu_b_sel;  // ALU输入B选择
    wire [3:0]  id_aluc;       // ALU操作控制码
    wire        id_mul_ena;    // 乘法使能
    wire        id_div_ena;    // 除法使能
    wire        id_mul_sign;   // 乘法有符号/无符号控制
    wire        id_div_sign;   // 除法有符号/无符号控制
    wire        id_cutter_sign;// 数据截取符号扩展控制
    wire        id_cutter_addr_sel; // 数据截取地址选择
    wire [2:0]  id_cutter_sel; // 数据截取类型选择
    wire        id_dmem_ena;   // 数据存储器使能
    wire        id_dmem_wena;  // 数据存储器写使能
    wire [1:0]  id_dmem_wsel;  // 数据存储器写数据位宽选择
    wire [1:0]  id_dmem_rsel;  // 数据存储器读数据位宽选择
    wire        id_hi_wena;    // HI寄存器写使能
    wire        id_lo_wena;    // LO寄存器写使能
    wire        id_rd_wena;    // 通用寄存器写使能
    wire [1:0]  id_hi_sel;     // HI寄存器写回数据选择
    wire [1:0]  id_lo_sel;     // LO寄存器写回数据选择
    wire [2:0]  id_rd_sel;     // 通用寄存器写回数据选择
    wire [4:0]  id_rd_addr;    // 通用寄存器写地址

    // EX阶段信号：执行阶段相关数据与控制信号
    wire [5:0]  ex_op;
    wire [5:0]  ex_func;
    wire [31:0] ex_pc4;
    wire [31:0] ex_imm;
    wire [31:0] ex_shamt;
    wire [31:0] ex_rs_data;
    wire [31:0] ex_rt_data;
    wire [31:0] ex_hi_data;
    wire [31:0] ex_lo_data;
    wire [31:0] ex_cp0_data;
    wire        ex_alu_a_sel;
    wire [1:0]  ex_alu_b_sel;
    wire [3:0]  ex_aluc;
    wire        ex_mul_ena;
    wire        ex_div_ena;
    wire        ex_mul_sign;
    wire        ex_div_sign;
    wire        ex_cutter_sign;
    wire        ex_cutter_addr_sel;
    wire [2:0]  ex_cutter_sel;
    wire        ex_dmem_ena;
    wire        ex_dmem_wena;
    wire [1:0]  ex_dmem_wsel;
    wire [1:0]  ex_dmem_rsel;
    wire        ex_hi_wena;
    wire        ex_lo_wena;
    wire        ex_rd_wena;
    wire [1:0]  ex_hi_sel;
    wire [1:0]  ex_lo_sel;
    wire [2:0]  ex_rd_sel;
    wire [4:0]  ex_rd_addr;

    // EX阶段输出信号：执行阶段运算结果与传递的控制信号
    wire [31:0] ex_out_pc4;
    wire [31:0] ex_out_rs_data;
    wire [31:0] ex_out_rt_data;
    wire [31:0] ex_out_hi_data;
    wire [31:0] ex_out_lo_data;
    wire [31:0] ex_out_cp0_data;
    wire [31:0] ex_out_alu_data;    // ALU运算结果
    wire [31:0] ex_out_mul_hi;      // 乘法高位结果
    wire [31:0] ex_out_mul_lo;      // 乘法低位结果
    wire [31:0] ex_out_div_r;       // 除法余数
    wire [31:0] ex_out_div_q;       // 除法商
    wire [31:0] ex_out_clz_data;    // 前导零计数结果
    wire        ex_out_cutter_sign;
    wire        ex_out_cutter_addr_sel;
    wire [2:0]  ex_out_cutter_sel;
    wire        ex_out_dmem_ena;
    wire        ex_out_dmem_wena;
    wire [1:0]  ex_out_dmem_wsel;
    wire [1:0]  ex_out_dmem_rsel;
    wire        ex_out_hi_wena;
    wire        ex_out_lo_wena;
    wire        ex_out_rd_wena;
    wire [1:0]  ex_out_hi_sel;
    wire [1:0]  ex_out_lo_sel;
    wire [2:0]  ex_out_rd_sel;
    wire [4:0]  ex_out_rd_addr;

    // MEM阶段信号：访存阶段相关数据与控制信号
    wire [31:0] mem_pc4;
    wire [31:0] mem_rs_data;
    wire [31:0] mem_rt_data;
    wire [31:0] mem_hi_data;
    wire [31:0] mem_lo_data;
    wire [31:0] mem_cp0_data;
    wire [31:0] mem_alu_data;
    wire [31:0] mem_mul_hi;
    wire [31:0] mem_mul_lo;
    wire [31:0] mem_div_r;
    wire [31:0] mem_div_q;
    wire [31:0] mem_clz_data;
    wire        mem_cutter_sign;
    wire        mem_cutter_addr_sel;
    wire [2:0]  mem_cutter_sel;
    wire        mem_dmem_ena;
    wire        mem_dmem_wena;
    wire [1:0]  mem_dmem_wsel;
    wire [1:0]  mem_dmem_rsel;
    wire        mem_hi_wena;
    wire        mem_lo_wena;
    wire        mem_rd_wena;
    wire [1:0]  mem_hi_sel;
    wire [1:0]  mem_lo_sel;
    wire [2:0]  mem_rd_sel;
    wire [4:0]  mem_rd_addr;

    // MEM阶段输出信号：访存阶段结果与传递的控制信号
    wire [31:0] mem_out_pc4;
    wire [31:0] mem_out_rs_data;
    wire [31:0] mem_out_hi_data;
    wire [31:0] mem_out_lo_data;
    wire [31:0] mem_out_cp0_data;
    wire [31:0] mem_out_alu_data;
    wire [31:0] mem_out_mul_hi;
    wire [31:0] mem_out_mul_lo;
    wire [31:0] mem_out_div_r;
    wire [31:0] mem_out_div_q;
    wire [31:0] mem_out_clz_data;
    wire [31:0] mem_out_dmem_data;  // 数据存储器读取数据
    wire        mem_out_hi_wena;
    wire        mem_out_lo_wena;
    wire        mem_out_rd_wena;
    wire [1:0]  mem_out_hi_sel;
    wire [1:0]  mem_out_lo_sel;
    wire [2:0]  mem_out_rd_sel;
    wire [4:0]  mem_out_rd_addr;

    // WB阶段信号：写回阶段相关数据与控制信号
    wire [31:0] wb_pc4;
    wire [31:0] wb_rs_data;
    wire [31:0] wb_hi_data;
    wire [31:0] wb_lo_data;
    wire [31:0] wb_cp0_data;
    wire [31:0] wb_alu_data;
    wire [31:0] wb_mul_hi;
    wire [31:0] wb_mul_lo;
    wire [31:0] wb_div_r;
    wire [31:0] wb_div_q;
    wire [31:0] wb_clz_data;
    wire [31:0] wb_dmem_data;
    wire        wb_hi_wena;
    wire        wb_lo_wena;
    wire        wb_rd_wena;
    wire [1:0]  wb_hi_sel;
    wire [1:0]  wb_lo_sel;
    wire [2:0]  wb_rd_sel;
    wire [4:0]  wb_rd_addr;

    // WB阶段输出信号：最终写回寄存器的控制信号与数据
    wire        wb_out_hi_wena;
    wire        wb_out_lo_wena;
    wire        wb_out_rd_wena;
    wire [31:0] wb_out_hi_data;
    wire [31:0] wb_out_lo_data;
    wire [31:0] wb_out_rd_data;
    wire [4:0]  wb_out_rd_addr;

    // 实例化各功能模块与流水线寄存器，按流水线顺序连接
    // 程序计数器模块：生成当前PC值，支持暂停与跳转
    pc pc_inst(
        .clk(clk),
        .rst(rst),
        .stall(stall),          // 流水线暂停信号
        .pc_in(if_npc),         // 输入下一个PC值
        .pc_out(pc)             // 输出当前PC值
    );

    // IF阶段模块：取指令并计算各类下一个PC地址
    pipeIF pipe_if_inst(
        .pc(pc),                // 当前PC值
        .pc_sel(id_pc_sel),     // PC选择控制信号
        .cpc(id_eaddr),         // 异常地址
        .bpc(id_baddr),         // 分支地址
        .rpc(id_raddr),         // 寄存器跳转地址
        .jpc(id_jaddr),         // 直接跳转地址
        .npc(if_npc),           // 输出下一个PC值
        .pc4(if_pc4),           // 输出PC+4值
        .inst(inst)             // 输出读取的指令
    );

    // IF-ID流水线寄存器：锁存IF阶段数据，支持暂停与分支清空
    pipeIR pipe_ir_inst(
        .clk(clk),
        .rst(rst),
        .stall(stall),          // 流水线暂停信号
        .branch(branch),        // 分支跳转信号（清空流水线）
        .if_pc4(if_pc4),        // IF阶段的PC+4
        .if_inst(inst),         // IF阶段的指令
        .id_pc4(id_pc4),        // 输出到ID阶段的PC+4
        .id_inst(id_inst)       // 输出到ID阶段的指令
    );

    // ID阶段模块：指令译码、寄存器读取、控制信号生成与数据转发
    pipeID pipe_id_inst(
        .clk(clk),
        .rst(rst),
        .ena(ena),
        .id_pc4(id_pc4),        // ID阶段的PC+4
        .id_inst(id_inst),      // ID阶段的指令
        
        // 来自WB阶段的写回信号
        .wb_hi_we(wb_out_hi_wena),    // HI寄存器写使能
        .wb_lo_we(wb_out_lo_wena),    // LO寄存器写使能
        .wb_rf_we(wb_out_rd_wena),    // 通用寄存器写使能
        .wb_wr_addr(wb_out_rd_addr),  // 通用寄存器写地址
        .wb_hi_data(wb_out_hi_data),  // HI寄存器写回数据
        .wb_lo_data(wb_out_lo_data),  // LO寄存器写回数据
        .wb_rf_data(wb_out_rd_data),  // 通用寄存器写回数据
        
        // 来自EX阶段的转发数据
        .ex_op(ex_op),
        .ex_func(ex_func),
        .ex_pc4(ex_out_pc4),
        .ex_alu_out(ex_out_alu_data), // EX阶段ALU运算结果
        .ex_mul_hi(ex_out_mul_hi),   // EX阶段乘法高位结果
        .ex_mul_lo(ex_out_mul_lo),   // EX阶段乘法低位结果
        .ex_rem(ex_out_div_r),       // EX阶段除法余数
        .ex_quo(ex_out_div_q),       // EX阶段除法商
        .ex_cnt(ex_out_clz_data),    // EX阶段前导零计数结果
        .ex_hi(ex_out_hi_data),      // EX阶段HI寄存器数据
        .ex_lo(ex_out_lo_data),      // EX阶段LO寄存器数据
        .ex_rs_data(ex_out_rs_data), // EX阶段RS寄存器数据
        .ex_hi_we(ex_out_hi_wena),   // EX阶段HI寄存器写使能
        .ex_lo_we(ex_out_lo_wena),   // EX阶段LO寄存器写使能
        .ex_rf_we(ex_out_rd_wena),   // EX阶段通用寄存器写使能
        .ex_hi_sel(ex_out_hi_sel),   // EX阶段HI寄存器数据选择
        .ex_lo_sel(ex_out_lo_sel),   // EX阶段LO寄存器数据选择
        .ex_rf_sel(ex_out_rd_sel),   // EX阶段通用寄存器数据选择
        .ex_wr_addr(ex_out_rd_addr), // EX阶段通用寄存器写地址
        
        // 来自MEM阶段的转发数据
        .mem_pc4(mem_out_pc4),
        .mem_alu_out(mem_out_alu_data), // MEM阶段ALU运算结果
        .mem_mul_hi(mem_out_mul_hi),   // MEM阶段乘法高位结果
        .mem_mul_lo(mem_out_mul_lo),   // MEM阶段乘法低位结果
        .mem_quo(mem_out_div_r),       // MEM阶段除法余数
        .mem_rem(mem_out_div_q),       // MEM阶段除法商
        .mem_cnt(mem_out_clz_data),    // MEM阶段前导零计数结果
        .mem_lo(mem_out_lo_data),      // MEM阶段LO寄存器数据
        .mem_hi(mem_out_hi_data),      // MEM阶段HI寄存器数据
        .mem_rs_data(mem_out_rs_data), // MEM阶段RS寄存器数据
        .mem_dm_out(mem_out_dmem_data),// MEM阶段数据存储器读取数据
        .mem_hi_we(mem_out_hi_wena),   // MEM阶段HI寄存器写使能
        .mem_lo_we(mem_out_lo_wena),   // MEM阶段LO寄存器写使能
        .mem_rf_we(mem_out_rd_wena),   // MEM阶段通用寄存器写使能
        .mem_hi_sel(mem_out_hi_sel),   // MEM阶段HI寄存器数据选择
        .mem_lo_sel(mem_out_lo_sel),   // MEM阶段LO寄存器数据选择
        .mem_rf_sel(mem_out_rd_sel),   // MEM阶段通用寄存器数据选择
        .mem_wr_addr(mem_out_rd_addr), // MEM阶段通用寄存器写地址
        
        // 控制信号输出
        .stall(stall),          // 生成流水线暂停信号
        .is_goto(branch),       // 生成分支跳转信号
        
        // 指令信息输出
        .op_out(id_op),         // 输出操作码
        .func_out(id_func),     // 输出功能码
        
        // PC控制信号
        .pc_sel_out(id_pc_sel), // 输出PC选择控制信号
        .id_pc4_out(id_pc4_out),// 输出ID阶段PC+4
        
        // 立即数扩展
        .imm_ext(id_imm),       // 输出扩展后的立即数
        .shamt_ext(id_shamt),   // 输出扩展后的位移量
        
        // 地址计算
        .cpc_out(id_eaddr),     // 输出异常地址
        .bpc_out(id_baddr),     // 输出分支地址
        .jpc_out(id_jaddr),     // 输出跳转地址
        .rpc_out(id_raddr),     // 输出寄存器跳转地址
        
        // 寄存器数据
        .rs_data(id_rs_data),   // 输出RS寄存器数据
        .rt_data(id_rt_data),   // 输出RT寄存器数据
        .hi_out(id_hi_data),    // 输出HI寄存器数据
        .lo_out(id_lo_data),    // 输出LO寄存器数据
        .cp0_out(id_cp0_data),  // 输出CP0寄存器数据
        
        // ALU控制
        .a_sel(id_alu_a_sel),   // 输出ALU输入A选择信号
        .b_sel(id_alu_b_sel),   // 输出ALU输入B选择信号
        .alu_op(id_aluc),       // 输出ALU操作控制码
        
        // 乘除法控制
        .mul_ena(id_mul_ena),   // 输出乘法使能信号
        .div_ena(id_div_ena),   // 输出除法使能信号
        .mul_sign(id_mul_sign), // 输出乘法有符号控制信号
        .div_sign(id_div_sign), // 输出除法有符号控制信号
        
        // 写使能控制
        .hi_we(id_hi_wena),     // 输出HI寄存器写使能
        .lo_we(id_lo_wena),     // 输出LO寄存器写使能
        .rf_we(id_rd_wena),     // 输出通用寄存器写使能
        
        // 数据截取控制
        .cutter_sign(id_cutter_sign),       // 输出数据截取符号扩展控制
        .cutter_addr_sel(id_cutter_addr_sel), // 输出数据截取地址选择
        .cutter_sel(id_cutter_sel),         // 输出数据截取类型选择
        
        // 数据存储器控制
        .dm_ena(id_dmem_ena),   // 输出数据存储器使能
        .dm_we(id_dmem_wena),   // 输出数据存储器写使能
        .dm_wsel(id_dmem_wsel), // 输出数据存储器写位宽选择
        .dm_rsel(id_dmem_rsel), // 输出数据存储器读位宽选择
        
        // 写回数据选择
        .hi_sel(id_hi_sel),     // 输出HI寄存器写回数据选择
        .lo_sel(id_lo_sel),     // 输出LO寄存器写回数据选择
        .rf_sel(id_rd_sel),     // 输出通用寄存器写回数据选择
        .wr_addr(id_rd_addr),   // 输出通用寄存器写地址
        
        // 调试寄存器输出
        .a(a),
        .b(b),
        .c(c),
        .d(d)
    );

    // ID-EX流水线寄存器：锁存ID阶段数据与控制信号，支持暂停
    pipeIDEreg pipe_id_ex_inst(
        .clk(clk),
        .rst(rst),
        .stall(stall),          // 流水线暂停信号
        
        // ID阶段输入
        .id_op(id_op),
        .id_func(id_func),
        .id_pc4(id_pc4_out),
        .id_imm(id_imm),
        .id_shamt(id_shamt),
        .id_rs_data(id_rs_data),
        .id_rt_data(id_rt_data),
        .id_hi(id_hi_data),
        .id_lo(id_lo_data),
        .id_cp0(id_cp0_data),
        .id_a_sel(id_alu_a_sel),
        .id_b_sel(id_alu_b_sel),
        .id_alu_op(id_aluc),
        .id_mul_ena(id_mul_ena),
        .id_div_ena(id_div_ena),
        .id_mul_sign(id_mul_sign),
        .id_div_sign(id_div_sign),
        .id_cutter_sign(id_cutter_sign),
        .id_cutter_addr_sel(id_cutter_addr_sel),
        .id_cutter_sel(id_cutter_sel),
        .id_dm_ena(id_dmem_ena),
        .id_dm_we(id_dmem_wena),
        .id_dm_wsel(id_dmem_wsel),
        .id_dm_rsel(id_dmem_rsel),
        .id_hi_we(id_hi_wena),
        .id_lo_we(id_lo_wena),
        .id_rf_we(id_rd_wena),
        .id_hi_sel(id_hi_sel),
        .id_lo_sel(id_lo_sel),
        .id_rf_sel(id_rd_sel),
        .id_wr_addr(id_rd_addr),

        // EX阶段输出
        .ex_op(ex_op),
        .ex_func(ex_func),
        .ex_pc4(ex_pc4),
        .ex_imm(ex_imm),
        .ex_shamt(ex_shamt),
        .ex_rs_data(ex_rs_data),
        .ex_rt_data(ex_rt_data),
        .ex_hi(ex_hi_data),
        .ex_lo(ex_lo_data),
        .ex_cp0(ex_cp0_data),
        .ex_a_sel(ex_alu_a_sel),
        .ex_b_sel(ex_alu_b_sel),
        .ex_alu_op(ex_aluc),
        .ex_mul_ena(ex_mul_ena),
        .ex_div_ena(ex_div_ena),
        .ex_mul_sign(ex_mul_sign),
        .ex_div_sign(ex_div_sign),
        .ex_cutter_sign(ex_cutter_sign),
        .ex_cutter_addr_sel(ex_cutter_addr_sel),
        .ex_cutter_sel(ex_cutter_sel),
        .ex_dm_ena(ex_dmem_ena),
        .ex_dm_we(ex_dmem_wena),
        .ex_dm_wsel(ex_dmem_wsel),
        .ex_dm_rsel(ex_dmem_rsel),
        .ex_rf_we(ex_rd_wena),
        .ex_hi_we(ex_hi_wena),
        .ex_lo_we(ex_lo_wena),
        .ex_hi_sel(ex_hi_sel),
        .ex_lo_sel(ex_lo_sel),
        .ex_rf_sel(ex_rd_sel),
        .ex_wr_addr(ex_rd_addr)
    );

    // EX阶段模块：执行算术逻辑运算、乘除法运算与数据截取
    pipeEX pipe_ex_inst(
        .rst(rst),
        .ex_pc4(ex_pc4),
        .ex_imm(ex_imm),
        .ex_shamt(ex_shamt),
        .ex_rs_data(ex_rs_data),
        .ex_rt_data(ex_rt_data),
        .ex_hi(ex_hi_data),
        .ex_lo(ex_lo_data),
        .ex_cp0(ex_cp0_data),
        .ex_a_sel(ex_alu_a_sel),
        .ex_b_sel(ex_alu_b_sel),
        .ex_alu_op(ex_aluc),
        .ex_mul_ena(ex_mul_ena),
        .ex_div_ena(ex_div_ena),
        .ex_mul_sign(ex_mul_sign),
        .ex_div_sign(ex_div_sign),
        .ex_cutter_sign(ex_cutter_sign),
        .ex_cutter_addr_sel(ex_cutter_addr_sel),
        .ex_cutter_sel(ex_cutter_sel),
        .ex_dm_ena(ex_dmem_ena),
        .ex_dm_we(ex_dmem_wena),
        .ex_dm_wsel(ex_dmem_wsel),
        .ex_dm_rsel(ex_dmem_rsel),
        .ex_rf_we(ex_rd_wena),
        .ex_hi_we(ex_hi_wena),
        .ex_lo_we(ex_lo_wena),
        .ex_hi_sel(ex_hi_sel),
        .ex_lo_sel(ex_lo_sel),
        .ex_rf_sel(ex_rd_sel),
        .ex_wr_addr(ex_rd_addr),
        
        // EX阶段输出
        .ex_out_pc4(ex_out_pc4),
        .ex_mul_hi(ex_out_mul_hi),      // 输出乘法高位结果
        .ex_mul_lo(ex_out_mul_lo),      // 输出乘法低位结果
        .ex_rem(ex_out_div_r),          // 输出除法余数
        .ex_quo(ex_out_div_q),          // 输出除法商
        .ex_out_rs_data(ex_out_rs_data),
        .ex_out_rt_data(ex_out_rt_data),
        .ex_out_hi(ex_out_hi_data),
        .ex_out_lo(ex_out_lo_data),
        .ex_out_cp0(ex_out_cp0_data),
        .ex_cnt(ex_out_clz_data),       // 输出前导零计数结果
        .ex_alu_out(ex_out_alu_data),   // 输出ALU运算结果
        
        // 控制信号传递
        .ex_out_cutter_sign(ex_out_cutter_sign),
        .ex_out_cutter_addr_sel(ex_out_cutter_addr_sel),
        .ex_out_cutter_sel(ex_out_cutter_sel),
        .ex_out_dm_ena(ex_out_dmem_ena),
        .ex_out_dm_we(ex_out_dmem_wena),
        .ex_out_dm_wsel(ex_out_dmem_wsel),
        .ex_out_dm_rsel(ex_out_dmem_rsel),
        .ex_out_hi_we(ex_out_hi_wena),
        .ex_out_lo_we(ex_out_lo_wena),
        .ex_out_rf_we(ex_out_rd_wena),
        .ex_out_hi_sel(ex_out_hi_sel),
        .ex_out_lo_sel(ex_out_lo_sel),
        .ex_out_rf_sel(ex_out_rd_sel),
        .ex_out_wr_addr(ex_out_rd_addr)
    );

    // EX-MEM流水线寄存器：锁存EX阶段运算结果与控制信号
    pipeEXMreg pipe_ex_mem_inst(
        .clk(clk),
        .rst(rst),
        
        // EX阶段输入
        .ex_pc4(ex_out_pc4),
        .ex_rs_data(ex_out_rs_data),
        .ex_rt_data(ex_out_rt_data),
        .ex_hi(ex_out_hi_data),
        .ex_lo(ex_out_lo_data),
        .ex_cp0(ex_out_cp0_data),
        .ex_alu_out(ex_out_alu_data),
        .ex_mul_hi(ex_out_mul_hi),
        .ex_mul_lo(ex_out_mul_lo),
        .ex_rem(ex_out_div_r),
        .ex_quo(ex_out_div_q),
        .ex_cnt(ex_out_clz_data),
        .ex_cutter_sign(ex_out_cutter_sign),
        .ex_cutter_sel(ex_out_cutter_sel),
        .ex_cutter_addr_sel(ex_out_cutter_addr_sel),
        .ex_dm_ena(ex_out_dmem_ena),
        .ex_dm_we(ex_out_dmem_wena),
        .ex_dm_wsel(ex_out_dmem_wsel),
        .ex_dm_rsel(ex_out_dmem_rsel),
        .ex_hi_we(ex_out_hi_wena),
        .ex_lo_we(ex_out_lo_wena),
        .ex_rf_we(ex_out_rd_wena),
        .ex_hi_sel(ex_out_hi_sel),
        .ex_lo_sel(ex_out_lo_sel),
        .ex_rf_sel(ex_out_rd_sel),
        .ex_wr_addr(ex_out_rd_addr),
        
        // MEM阶段输出
        .mem_pc4(mem_pc4),
        .mem_rs_data(mem_rs_data),
        .mem_rt_data(mem_rt_data),
        .mem_hi(mem_hi_data),
        .mem_lo(mem_lo_data),
        .mem_cp0(mem_cp0_data),
        .mem_alu_out(mem_alu_data),
        .mem_mul_hi(mem_mul_hi),
        .mem_mul_lo(mem_mul_lo),
        .mem_rem(mem_div_r),
        .mem_quo(mem_div_q),
        .mem_cnt(mem_clz_data),
        .mem_cutter_sign(mem_cutter_sign),
        .mem_cutter_addr_sel(mem_cutter_addr_sel),
        .mem_cutter_sel(mem_cutter_sel),
        .mem_dm_ena(mem_dmem_ena),
        .mem_dm_we(mem_dmem_wena),
        .mem_dm_wsel(mem_dmem_wsel),
        .mem_dm_rsel(mem_dmem_rsel),
        .mem_rf_we(mem_rd_wena),
        .mem_hi_we(mem_hi_wena),
        .mem_lo_we(mem_lo_wena),
        .mem_hi_sel(mem_hi_sel),
        .mem_lo_sel(mem_lo_sel),
        .mem_rf_sel(mem_rd_sel),
        .mem_wr_addr(mem_rd_addr)
    );

    // MEM阶段模块：访问数据存储器，完成数据读写操作
    pipeMEM pipe_mem_inst(
        .clk(clk),
        .mem_pc4(mem_pc4),
        .ena(ena),
        .mem_rs_data(mem_rs_data),
        .mem_rt_data(mem_rt_data),
        .mem_hi(mem_hi_data),
        .mem_lo(mem_lo_data),
        .mem_cp0(mem_cp0_data),
        .mem_alu_out(mem_alu_data),
        .mem_mul_hi(mem_mul_hi),
        .mem_mul_lo(mem_mul_lo),
        .mem_rem(mem_div_r),
        .mem_quo(mem_div_q),
        .mem_cnt(mem_clz_data),
        .mem_cutter_sign(mem_cutter_sign),
        .mem_cutter_addr_sel(mem_cutter_addr_sel),
        .mem_cutter_sel(mem_cutter_sel),
        .mem_dm_wsel(mem_dmem_wsel),
        .mem_dm_rsel(mem_dmem_rsel),
        .mem_dm_ena(mem_dmem_ena),
        .mem_dm_we(mem_dmem_wena),
        .mem_hi_we(mem_hi_wena),
        .mem_lo_we(mem_lo_wena),
        .mem_rf_we(mem_rd_wena),
        .mem_hi_sel(mem_hi_sel),
        .mem_lo_sel(mem_lo_sel),
        .mem_rf_sel(mem_rd_sel),
        .mem_wr_addr(mem_rd_addr),
        
        // MEM阶段输出
        .mem_out_pc4(mem_out_pc4),
        .mem_out_rs_data(mem_out_rs_data),
        .mem_out_hi(mem_out_hi_data),
        .mem_out_lo(mem_out_lo_data),
        .mem_out_cp0(mem_out_cp0_data),
        .mem_out_alu_out(mem_out_alu_data),
        .mem_out_mul_hi(mem_out_mul_hi),
        .mem_out_mul_lo(mem_out_mul_lo),
        .mem_out_rem(mem_out_div_r),
        .mem_out_quo(mem_out_div_q),
        .mem_out_cnt(mem_out_clz_data),
        .mem_out_dm_data(mem_out_dmem_data),  // 输出数据存储器读取数据
        .mem_out_hi_we(mem_out_hi_wena),
        .mem_out_lo_we(mem_out_lo_wena),
        .mem_out_rf_we(mem_out_rd_wena),
        .mem_out_hi_sel(mem_out_hi_sel),
        .mem_out_lo_sel(mem_out_lo_sel),
        .mem_out_rf_sel(mem_out_rd_sel),
        .mem_out_wr_addr(mem_out_rd_addr)
    );

    // MEM-WB流水线寄存器：锁存MEM阶段结果与控制信号
    pipeMWreg pipe_mem_wb_inst(
        .clk(clk),
        .rst(rst),
        
        // MEM阶段输入
        .mem_pc4(mem_out_pc4),
        .mem_rs_data(mem_out_rs_data),
        .mem_hi(mem_out_hi_data),
        .mem_lo(mem_out_lo_data),
        .mem_cp0(mem_out_cp0_data),
        .mem_alu_out(mem_out_alu_data),
        .mem_mul_hi(mem_out_mul_hi),
        .mem_mul_lo(mem_out_mul_lo),
        .mem_rem(mem_out_div_r),
        .mem_quo(mem_out_div_q),
        .mem_cnt(mem_out_clz_data),
        .mem_dm_data(mem_out_dmem_data),
        .mem_hi_we(mem_out_hi_wena),
        .mem_lo_we(mem_out_lo_wena),
        .mem_rf_we(mem_out_rd_wena),
        .mem_hi_sel(mem_out_hi_sel),
        .mem_lo_sel(mem_out_lo_sel),
        .mem_rf_sel(mem_out_rd_sel),
        .mem_wr_addr(mem_out_rd_addr),
        
        // WB阶段输出
        .wb_pc4(wb_pc4),
        .wb_rs_data(wb_rs_data),
        .wb_hi(wb_hi_data),
        .wb_lo(wb_lo_data),
        .wb_cp0(wb_cp0_data),
        .wb_alu_out(wb_alu_data),
        .wb_mul_hi(wb_mul_hi),
        .wb_mul_lo(wb_mul_lo),
        .wb_rem(wb_div_r),
        .wb_quo(wb_div_q),
        .wb_cnt(wb_clz_data),
        .wb_dm_data(wb_dmem_data),
        .wb_hi_we(wb_hi_wena),
        .wb_lo_we(wb_lo_wena),
        .wb_rf_we(wb_rd_wena),
        .wb_hi_sel(wb_hi_sel),
        .wb_lo_sel(wb_lo_sel),
        .wb_rf_sel(wb_rd_sel),
        .wb_wr_addr(wb_rd_addr)
    );

    // WB阶段模块：选择写回数据，完成对通用寄存器/HI/LO寄存器的写回
    pipeWB pipe_wb_inst(
        .wb_pc4(wb_pc4),
        .wb_rs_data(wb_rs_data),
        .wb_hi(wb_hi_data),
        .wb_lo(wb_lo_data),
        .wb_cp0(wb_cp0_data),
        .wb_alu_out(wb_alu_data),
        .wb_mul_hi(wb_mul_hi),
        .wb_mul_lo(wb_mul_lo),
        .wb_rem(wb_div_r),
        .wb_quo(wb_div_q),
        .wb_cnt(wb_clz_data),
        .wb_dm_data(wb_dmem_data),
        .wb_hi_we(wb_hi_wena),
        .wb_lo_we(wb_lo_wena),
        .wb_rf_we(wb_rd_wena),
        .wb_hi_sel(wb_hi_sel),
        .wb_lo_sel(wb_lo_sel),
        .wb_rf_sel(wb_rd_sel),
        .wb_wr_addr(wb_rd_addr),
        
        // 写回输出
        .wb_out_hi_we(wb_out_hi_wena),
        .wb_out_lo_we(wb_out_lo_wena),
        .wb_out_rf_we(wb_out_rd_wena),
        .wb_out_wr_addr(wb_out_rd_addr),
        .wb_hi_data(wb_out_hi_data),
        .wb_lo_data(wb_out_lo_data),
        .wb_rf_data(wb_out_rd_data)
    );

endmodule