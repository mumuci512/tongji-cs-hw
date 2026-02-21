module pipeIR(
    input               clk,
    input               rst,
    input               stall,
    input               branch,
    input [31:0]        if_pc4,
    input [31:0]        if_inst,
    output [31:0]       id_pc4,
    output [31:0]       id_inst
);

    // 中间信号：寄存器写使能与待锁存的PC+4、指令数据
    wire        wen;
    wire [31:0] pc4_d;
    wire [31:0] inst_d;

    // 寄存器写使能控制：流水线未暂停或发生分支时，允许写入新数据
    assign wen     = (~stall) || branch;
    // 分支发生时清零PC+4，否则传递IF阶段的PC+4地址
    assign pc4_d   = branch ? 32'b0 : if_pc4;
    // 分支发生时清零指令，否则传递IF阶段读取的指令
    assign inst_d = branch ? 32'b0 : if_inst;

    // PC+4寄存器实例化：锁存IF阶段的PC+4地址，传递至ID阶段
    register pc4_reg(
        .clk      (clk),
        .rst      (rst),
        .wena     (wen),
        .data_in (pc4_d),
        .data_out(id_pc4)
    );

    // 指令寄存器实例化：锁存IF阶段读取的指令，传递至ID阶段进行译码
    register ir_reg(
        .clk      (clk),
        .rst      (rst),
        .wena     (wen),
        .data_in (inst_d),
        .data_out(id_inst)
    );

endmodule