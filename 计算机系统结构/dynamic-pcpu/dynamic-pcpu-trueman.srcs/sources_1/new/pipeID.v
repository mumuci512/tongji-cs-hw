module pipeID(
    input           clk,
    input           rst,
    input           ena,
    input [31:0]    id_pc4,
    input [31:0]    id_inst,
    // 时钟、复位、使能信号，以及当前PC+4地址与待译码指令
    
    // 来自WB阶段的写回数据
    input           wb_hi_we,
    input           wb_lo_we,
    input           wb_rf_we,
    input [4:0]     wb_wr_addr,
    input [31:0]    wb_hi_data,
    // HI/LO/通用寄存器写使能与通用寄存器写地址
    input [31:0]    wb_lo_data,
    input [31:0]    wb_rf_data,
    // HI/LO/通用寄存器写回数据
    
    // 来自EX阶段的数据
    input [5:0]     ex_op,
    input [5:0]     ex_func,
    input [31:0]    ex_pc4,
    input [31:0]    ex_alu_out,
    input [31:0]    ex_mul_hi,
    // EX阶段指令信息、PC地址与运算结果
    input [31:0]    ex_mul_lo,
    input [31:0]    ex_rem,
    input [31:0]    ex_quo,
    input [31:0]    ex_cnt,
    input [31:0]    ex_hi,
    // EX阶段乘除法结果、辅助运算结果与寄存器数据
    input [31:0]    ex_lo,
    input [31:0]    ex_rs_data,
    input           ex_hi_we,
    input           ex_lo_we,
    input           ex_rf_we,
    // EX阶段寄存器写使能信号
    input [1:0]     ex_hi_sel,
    input [1:0]     ex_lo_sel,
    input [2:0]     ex_rf_sel,
    input [4:0]     ex_wr_addr,
    // EX阶段寄存器写回选择信号与写地址
    
    // 来自MEM阶段的数据
    input [31:0]    mem_pc4,
    input [31:0]    mem_alu_out,
    input [31:0]    mem_mul_hi,
    input [31:0]    mem_mul_lo,
    input [31:0]    mem_quo,
    // MEM阶段PC地址与各类运算结果
    input [31:0]    mem_rem,
    input [31:0]    mem_cnt,
    input [31:0]    mem_lo,
    input [31:0]    mem_hi,
    input [31:0]    mem_rs_data,
    // MEM阶段辅助运算结果与寄存器数据
    input [31:0]    mem_dm_out,
    input           mem_hi_we,
    input           mem_lo_we,
    input           mem_rf_we,
    // MEM阶段访存结果与寄存器写使能
    input [1:0]     mem_hi_sel,
    input [1:0]     mem_lo_sel,
    input [2:0]     mem_rf_sel,
    input [4:0]     mem_wr_addr,
    // MEM阶段寄存器写回选择信号与写地址
    
    // 流水线控制输出
    output          stall,
    output          is_goto,
    // 流水线暂停信号与分支/跳转有效标志
    
    // 指令字段输出
    output [5:0]    op_out,
    output [5:0]    func_out,
    // 译码后的指令操作码与功能码，传递至EX阶段
    
    // PC选择输出
    output [2:0]    pc_sel_out,
    output [31:0]   id_pc4_out,
    // PC选择控制信号与当前PC+4地址直通输出
    
    // 立即数和移位量输出
    output [31:0]   imm_ext,
    output [31:0]   shamt_ext,
    // 符号/零扩展后的立即数与扩展后的移位量
    
    // PC目标地址输出
    output [31:0]   cpc_out,
    output [31:0]   bpc_out,
    output [31:0]   jpc_out,
    output [31:0]   rpc_out,
    // 异常PC、分支PC、跳转PC与寄存器PC（JR指令）
    
    // 寄存器数据输出
    output [31:0]   rs_data,
    output [31:0]   rt_data,
    output [31:0]   hi_out,
    output [31:0]   lo_out,
    output [31:0]   cp0_out,
    // RS/RT/HI/LO/CP0寄存器数据，供EX阶段运算使用
    
    // ALU控制输出
    output          a_sel,
    output [1:0]    b_sel,
    output [3:0]    alu_op,
    // ALU操作数选择信号与ALU运算控制码
    
    // 乘除法控制输出
    output          mul_ena,
    output          div_ena,
    output          mul_sign,
    output          div_sign,
    // 乘除法使能信号与符号控制信号（有符号/无符号）
    
    // 写使能输出
    output          hi_we,
    output          lo_we,
    output          rf_we,
    // HI/LO/通用寄存器写使能信号
    
    // Cutter控制输出
    output          cutter_sign,
    output          cutter_addr_sel,
    output [2:0]    cutter_sel,
    // 数据截取符号扩展控制、地址选择与截取类型选择
    
    // 数据存储器控制输出
    output          dm_ena,
    output          dm_we,
    output [1:0]    dm_wsel,
    output [1:0]    dm_rsel,
    // 数据存储器使能、写使能与读写位宽选择
    
    // 数据选择输出
    output [1:0]    hi_sel,
    output [1:0]    lo_sel,
    output [2:0]    rf_sel,
    output [4:0]    wr_addr,
    // HI/LO/通用寄存器写回数据选择与通用寄存器写地址
    
    // 调试寄存器输出
    output [31:0]   a,
    output [31:0]   b,
    output [31:0]   c,
    output [31:0]   d
    // 特定寄存器调试输出，用于观测流水线运行状态
);

    // 指令字段解码：从32位指令中提取各功能字段，为后续译码提供基础
    wire [5:0] op   = id_inst[31:26];      // 提取指令操作码（高6位）
    wire [5:0] func = id_inst[5:0];        // 提取指令功能码（低6位，R型指令有效）
    wire [4:0] rs_addr  = id_inst[25:21];  // 提取RS寄存器地址（第21-25位）
    wire [4:0] rt_addr  = id_inst[20:16];  // 提取RT寄存器地址（第16-20位）
    
    // 控制信号：用于指令译码与数据通路控制的中间信号
    wire rs_re;                // RS寄存器读使能
    wire rt_re;                // RT寄存器读使能
    wire sign_ext;             // 立即数扩展类型选择（1=符号扩展，0=零扩展）
    wire mfc0, mtc0;           // CP0寄存器读写控制（MFC0=读CP0，MTC0=写CP0）
    wire eret;                 // 异常返回指令标志
    
    // 数据前递相关：用于EX/MEM阶段到ID阶段的数据前递中间信号
    wire [31:0] ex_fwd_hi;     // EX阶段HI寄存器前递数据
    wire [31:0] mem_fwd_hi;    // MEM阶段HI寄存器前递数据
    wire [31:0] ex_fwd_lo;     // EX阶段LO寄存器前递数据
    wire [31:0] mem_fwd_lo;    // MEM阶段LO寄存器前递数据
    wire [31:0] ex_fwd_rf;     // EX阶段通用寄存器前递数据
    wire [31:0] mem_fwd_rf;    // MEM阶段通用寄存器前递数据
    
    // 扩展相关：移位量扩展选择与输出
    wire        ext5_sel;      // 5位数据扩展选择信号（选择指令移位量或RS寄存器低5位）
    wire [4:0]  ext5_out;      // 5位扩展后输出数据
    
    // 立即数和移位量扩展：生成32位有效数据，供ALU运算使用
    assign imm_ext   = {{16{sign_ext & id_inst[15]}}, id_inst[15:0]};
    // 立即数扩展：根据sign_ext选择符号扩展或零扩展，将16位立即数扩展为32位
    assign shamt_ext = {27'b0, ext5_out};
    // 移位量扩展：将5位扩展数据补27个0，生成32位移位量数据
    
    // PC计算：生成各类分支/跳转目标地址，供PC选择模块使用
    assign bpc_out = id_pc4 + {{{14{id_inst[15]}}, id_inst[15:0], 2'b00}};
    // 分支目标地址：PC+4 + 符号扩展并左移2位的立即数
    assign jpc_out = {id_pc4[31:28], id_inst[25:0], 2'b00};
    // 跳转目标地址：PC高4位 + 指令26位地址 + 末尾2个0（左移2位）
    assign rpc_out = rs_data;
    // 寄存器跳转地址：直接使用RS寄存器数据（JR/JALR指令）
    
    // 数据前递控制：数据前递使能与寄存器前递标志，以及各类前递/原始数据
    wire fwd_ena;              // 数据前递总使能信号
    wire fwd_rs, fwd_rt;       // RS/RT寄存器是否需要前递的标志
    wire [31:0] hi_fwd, lo_fwd, hi_rf, lo_rf;  // HI/LO寄存器前递数据与原始读取数据
    wire [31:0] rs_fwd, rt_fwd, rs_rf, rt_rf;  // RS/RT寄存器前递数据与原始读取数据
    
    // CP0相关：协处理器0的异常控制与寄存器信息
    wire        cp0_exc;       // CP0异常发生标志
    wire [4:0]  cp0_addr;      // CP0寄存器地址
    wire [4:0]  cp0_cause;     // CP0异常原因码
    wire [31:0] cp0_status;    // CP0状态寄存器数据
    
    // 最终输出数据选择：根据前递使能信号，选择前递数据或原始寄存器数据
    assign rs_data  = (fwd_ena && fwd_rs) ? rs_fwd : rs_rf;
    // RS寄存器输出：需要前递则选前递数据，否则选原始读取数据
    assign rt_data  = (fwd_ena && fwd_rt) ? rt_fwd : rt_rf;
    // RT寄存器输出：需要前递则选前递数据，否则选原始读取数据
    assign hi_out   = fwd_ena ? hi_fwd : hi_rf;
    // HI寄存器输出：有前递则选前递数据，否则选原始读取数据
    assign lo_out   = fwd_ena ? lo_fwd : lo_rf;
    // LO寄存器输出：有前递则选前递数据，否则选原始读取数据
    assign id_pc4_out = id_pc4;
    // PC+4地址直通输出，无逻辑处理
    assign op_out   = op;
    // 指令操作码直通输出，传递至EX阶段
    assign func_out = func;
    // 指令功能码直通输出，传递至EX阶段
    
    // 寄存器堆实例化：实现通用寄存器的读写操作，提供原始RS/RT寄存器数据
    Regfiles regfile_inst(
        clk, 
        rst, 
        wb_rf_we, 
        rs_addr, 
        rt_addr, 
        rs_re, 
        rt_re, 
        wb_wr_addr, 
        wb_rf_data, 
        rs_rf, 
        rt_rf, 
        a,
        b,
        c,
        d
    );
    
    // 5位扩展多路选择器：选择移位量来源（指令移位量或RS寄存器低5位）
    Mux2To1_5Bit mux_extend5(
        id_inst[10:6], 
        rs_data[4:0], 
        ext5_sel, 
        ext5_out
    );
    
    // EX阶段HI数据前递选择：根据EX阶段HI选择信号，选择对应的前递数据
    Mux4To1_32Bit mux_ex_fwd_hi(
        ex_rem, 
        ex_mul_hi, 
        ex_rs_data, 
        32'hz, 
        ex_hi_sel, 
        ex_fwd_hi
    );
    
    // EX阶段LO数据前递选择：根据EX阶段LO选择信号，选择对应的前递数据
    Mux4To1_32Bit mux_ex_fwd_lo(
        ex_quo, 
        ex_mul_lo, 
        ex_rs_data, 
        32'hz, 
        ex_lo_sel, 
        ex_fwd_lo
    );
    
    // EX阶段RF数据前递选择：根据EX阶段RF选择信号，选择对应的前递数据
    Mux8To1_32Bit mux_ex_fwd_rf(
        ex_lo, 
        ex_pc4, 
        ex_cnt, 
        32'hz, 
        32'hz, 
        ex_alu_out, 
        ex_hi, 
        ex_mul_lo, 
        ex_rf_sel, 
        ex_fwd_rf
    );
    
    // MEM阶段HI数据前递选择：根据MEM阶段HI选择信号，选择对应的前递数据
    Mux4To1_32Bit mux_mem_fwd_hi(
        mem_quo, 
        mem_mul_hi, 
        mem_rs_data, 
        32'hz, 
        mem_hi_sel, 
        mem_fwd_hi
    );
    
    // MEM阶段LO数据前递选择：根据MEM阶段LO选择信号，选择对应的前递数据
    Mux4To1_32Bit mux_mem_fwd_lo(
        mem_rem, 
        mem_mul_lo, 
        mem_rs_data, 
        32'hz, 
        mem_lo_sel, 
        mem_fwd_lo
    );
    
    // MEM阶段RF数据前递选择：根据MEM阶段RF选择信号，选择对应的前递数据
    Mux8To1_32Bit mux_mem_fwd_rf(
        mem_lo, 
        mem_pc4, 
        mem_cnt, 
        32'hz, 
        mem_dm_out, 
        mem_alu_out, 
        mem_hi, 
        mem_mul_lo, 
        mem_rf_sel, 
        mem_fwd_rf
    );
    
    // CP0协处理器实例化：处理异常、中断与特权指令，提供CP0寄存器数据与异常目标地址
    CP0 cp0_inst(
        clk, 
        rst, 
        mfc0, 
        mtc0, 
        id_pc4 - 32'd4, 
        cp0_addr, 
        rt_data, 
        cp0_exc, 
        eret, 
        cp0_cause,
        ena, 
        cp0_out, 
        cp0_status, 
        cpc_out
    );
    
    // HI/LO寄存器实例化：存储乘除法运算结果，提供HI/LO寄存器原始读取数据
    hi_lo hi_lo_inst(
        clk, 
        rst, 
        wb_hi_data, 
        wb_lo_data, 
        wb_hi_we, 
        wb_lo_we, 
        hi_rf, 
        lo_rf
    );
    
    // 数据前递单元实例化：解决流水线数据冒险，生成前递控制信号与前递数据
    forwarding forwarding_inst(
        .clk(clk),
        .rst(rst),
        .op(op),
        .func(func),
        .rs_rena(rs_re),
        .rt_rena(rt_re),
        .rs_addr(rs_addr),
        .rt_addr(rt_addr),
        .exe_op(ex_op),
        .exe_func(ex_func),
        .exe_hi_data(ex_fwd_hi),
        .exe_lo_data(ex_fwd_lo),
        .exe_rd_data(ex_fwd_rf),
        .exe_hi_wena(ex_hi_we),
        .exe_lo_wena(ex_lo_we),
        .exe_rd_wena(ex_rf_we),
        .exe_rd_addr(ex_wr_addr),
        .mem_hi_data(mem_fwd_hi),
        .mem_lo_data(mem_fwd_lo),
        .mem_rd_data(mem_fwd_rf),
        .mem_hi_wena(mem_hi_we),
        .mem_lo_wena(mem_lo_we),
        .mem_rd_wena(mem_rf_we),
        .mem_rd_addr(mem_wr_addr),
        .stall(stall),
        .fwd(fwd_ena),
        .is_rs_forward(fwd_rs),
        .is_rt_forward(fwd_rt),
        .rs_data(rs_fwd),
        .rt_data(rt_fwd),
        .hi_data(hi_fwd),
        .lo_data(lo_fwd)
    );
    
    // 比较单元实例化：判断分支/跳转指令是否有效，生成is_goto标志
    compare compare_inst(
        clk, 
        rst, 
        rs_data, 
        rt_data, 
        op, 
        func, 
        cp0_exc, 
        is_goto
    );
    
    // 控制器实例化：指令译码核心，生成流水线所有控制信号，协调各模块工作
    Controller controller_inst(
        .branch(is_goto),
        .status(cp0_status),
        .inst(id_inst),
        .pc_sel(pc_sel_out),
        .immed_sign(sign_ext),
        .ext5_sel(ext5_sel),
        .rs_rena(rs_re),
        .rt_rena(rt_re),
        .alu_a_sel(a_sel),
        .alu_b_sel(b_sel),
        .aluc(alu_op),
        .mul_ena(mul_ena),
        .div_ena(div_ena),
        .mul_sign(mul_sign),
        .div_sign(div_sign),
        .cutter_sign(cutter_sign),
        .cutter_addr_sel(cutter_addr_sel),
        .cutter_sel(cutter_sel),
        .dmem_ena(dm_ena),
        .dmem_wena(dm_we),
        .dmem_wsel(dm_wsel),
        .dmem_rsel(dm_rsel),
        .eret(eret),
        .cause(cp0_cause),
        .exception(cp0_exc),
        .cp0_addr(cp0_addr),
        .mfc0(mfc0),
        .mtc0(mtc0),
        .hi_wena(hi_we),
        .lo_wena(lo_we),
        .rd_wena(rf_we),
        .hi_sel(hi_sel),
        .lo_sel(lo_sel),
        .rd_sel(rf_sel),
        .rdc(wr_addr)
    );

endmodule