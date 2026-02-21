module pipeIF(
    input   [31:0]  pc,
    input   [31:0]  cpc,
    input   [31:0]  bpc,
    input   [31:0]  rpc,
    input   [31:0]  jpc,
    // 各类PC地址输入：当前PC、异常PC、分支PC、寄存器PC、跳转PC
    input   [2:0]   pc_sel,
    // PC选择控制信号，用于选择下一个PC地址来源
    output  [31:0]  npc,
    output  [31:0]  pc4,
    output  [31:0]  inst
    // 输出信号：下一个PC地址、当前PC+4地址、读取的指令
);

    // PC地址递增：生成当前PC+4地址，供顺序执行指令使用
    assign pc4 = pc + 32'd4;

    // 指令存储器实例化：根据当前PC地址读取指令，仅使用PC的[12:2]位（指令按字对齐）
    imem imem_inst(pc[12:2], inst);

    // 8选1多路选择器实例化：根据pc_sel信号选择下一个PC地址（npc）的来源
    Mux8To1_32Bit mux_npc(
        jpc,
        rpc,
        pc4,
        32'h00400004,
        bpc,
        cpc,
        32'bz,
        32'bz,
        pc_sel,
        npc
    );

endmodule