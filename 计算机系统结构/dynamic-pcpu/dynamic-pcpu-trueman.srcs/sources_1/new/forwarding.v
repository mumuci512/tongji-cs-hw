`include "mips_def.vh"
`timescale 1ns / 1ps

// 数据转发模块：解决MIPS流水线中的数据冒险问题
// 支持EXE/MEM阶段向ID阶段的数据转发，同时处理load-use冒险的流水线暂停
module forwarding(
    input               clk,
    input               rst,
    input [5:0]         op,             // 当前ID阶段指令的操作码
    input [5:0]         func,           // 当前ID阶段指令的功能码
    input               rs_rena,        // RS寄存器读使能（标记是否需要读取RS寄存器）
    input               rt_rena,        // RT寄存器读使能（标记是否需要读取RT寄存器）
    input [4:0]         rs_addr,        // RS寄存器地址（待读取的源寄存器1地址）
    input [4:0]         rt_addr,        // RT寄存器地址（待读取的源寄存器2地址）

    // EXE阶段信息：传递当前执行阶段的寄存器写回信息
    input [5:0]         exe_op,         // EXE阶段指令的操作码
    input [5:0]         exe_func,       // EXE阶段指令的功能码
    input [31:0]        exe_hi_data,    // EXE阶段HI寄存器待写回数据
    input [31:0]        exe_lo_data,    // EXE阶段LO寄存器待写回数据
    input [31:0]        exe_rd_data,    // EXE阶段通用寄存器待写回数据
    input               exe_hi_wena,    // EXE阶段HI寄存器写使能（标记是否要写HI寄存器）
    input               exe_lo_wena,    // EXE阶段LO寄存器写使能（标记是否要写LO寄存器）
    input               exe_rd_wena,    // EXE阶段通用寄存器写使能（标记是否要写通用寄存器）
    input [4:0]         exe_rd_addr,    // EXE阶段通用寄存器写地址（待写入的目标寄存器地址）

    // MEM阶段信息：传递当前访存阶段的寄存器写回信息
    input [31:0]        mem_hi_data,    // MEM阶段HI寄存器待写回数据
    input [31:0]        mem_lo_data,    // MEM阶段LO寄存器待写回数据
    input [31:0]        mem_rd_data,    // MEM阶段通用寄存器待写回数据
    input               mem_hi_wena,    // MEM阶段HI寄存器写使能
    input               mem_lo_wena,    // MEM阶段LO寄存器写使能
    input               mem_rd_wena,    // MEM阶段通用寄存器写使能
    input [4:0]         mem_rd_addr,    // MEM阶段通用寄存器写地址

    // 转发控制输出：提供数据转发与流水线控制信号及转发数据
    output reg          stall,          // 流水线暂停信号（用于解决load-use冒险）
    output reg          fwd,            // 转发使能信号（标记当前是否存在数据转发）
    output reg          is_rs_forward,  // RS寄存器转发标记（标记RS寄存器是否需要转发）
    output reg          is_rt_forward,  // RT寄存器转发标记（标记RT寄存器是否需要转发）
    output reg [31:0]   rs_data,        // RS寄存器转发数据（转发给ID阶段的RS寄存器数据）
    output reg [31:0]   rt_data,        // RT寄存器转发数据（转发给ID阶段的RT寄存器数据）
    output reg [31:0]   hi_data,        // HI寄存器转发数据（转发给ID阶段的HI寄存器数据）
    output reg [31:0]   lo_data         // LO寄存器转发数据（转发给ID阶段的LO寄存器数据）
    );

    // 转发逻辑处理：负时钟沿触发，避免与流水线寄存器冲突
    always@(negedge clk or posedge rst) 
    begin
        if(rst) 
        begin
            // 复位状态：所有控制信号置0，转发数据清零
            stall           <= 1'b0;
            rs_data         <= 32'b0;
            rt_data         <= 32'b0;
            hi_data         <= 32'b0;
            lo_data         <= 32'b0;
            fwd             <= 1'b0;
            is_rs_forward   <= 1'b0;
            is_rt_forward   <= 1'b0;
        end 
        else if(stall) 
        begin
            // 解决load-use冒险：暂停一个周期后取消暂停，从MEM阶段获取有效数据
            stall <= 1'b0;
            if(is_rs_forward) 
                rs_data <= mem_rd_data;  // RS寄存器需要转发，加载MEM阶段的写回数据
            else if(is_rt_forward)
                rt_data <= mem_rd_data;  // RT寄存器需要转发，加载MEM阶段的写回数据
        end 
        else 
        begin
            // 非暂停状态：先初始化转发标记与使能信号
            fwd             = 0;
            is_rs_forward   = 0;
            is_rt_forward   = 0;
            
            // 处理MFHI/MFLO指令的转发：专门针对HI/LO寄存器的数据转发
            if(op == `OP_MFHI && func == `FUNC_MFHI)  // 当前是MFHI指令（读取HI寄存器）
            begin
                if(exe_hi_wena)  // 优先转发EXE阶段的HI写回数据（数据最新）
                begin
                    hi_data     <= exe_hi_data;
                    fwd         <= 1'b1;
                end 
                else if(mem_hi_wena)  // 其次转发MEM阶段的HI写回数据
                begin
                    hi_data     <= mem_hi_data;
                    fwd         <= 1'b1;
                end
            end 
            else if(op == `OP_MFLO && func == `FUNC_MFLO)  // 当前是MFLO指令（读取LO寄存器）
            begin
                if(exe_lo_wena)  // 优先转发EXE阶段的LO写回数据
                begin
                    lo_data     <= exe_lo_data;
                    fwd         <= 1'b1;
                end 
                else if(mem_lo_wena)  // 其次转发MEM阶段的LO写回数据
                begin
                    lo_data     <= mem_lo_data;
                    fwd         <= 1'b1;
                end
            end 
            else 
            begin
                // 通用寄存器RS的数据转发判断：优先EXE阶段，再MEM阶段
                if(exe_rd_wena && rs_rena && exe_rd_addr == rs_addr)  // EXE阶段有对应写回且RS需要读取
                begin
                    // 判断EXE阶段是否为load指令（load指令数据未就绪，需暂停）
                    if(exe_op == `OP_LW || exe_op == `OP_LH || exe_op == `OP_LHU || 
                       exe_op == `OP_LB || exe_op == `OP_LBU) 
                    begin
                        is_rs_forward   <= 1'b1;  // 标记RS需要转发
                        stall           <= 1'b1;  // 触发流水线暂停，解决load-use冒险
                        fwd             <= 1'b1;
                    end
                    else  // 非load指令，直接转发EXE阶段的写回数据
                    begin
                        is_rs_forward   <= 1'b1;
                        rs_data         <= exe_rd_data;
                        fwd             <= 1'b1;
                    end
                end
                else if(mem_rd_wena && rs_rena && mem_rd_addr == rs_addr)  // MEM阶段有对应写回
                begin
                    is_rs_forward   <= 1'b1;
                    rs_data         <= mem_rd_data;  // 转发MEM阶段的写回数据
                    fwd             <= 1'b1;
                end
                
                // 通用寄存器RT的数据转发判断：逻辑与RS寄存器一致
                if(exe_rd_wena && rt_rena && exe_rd_addr == rt_addr)  // EXE阶段有对应写回且RT需要读取
                begin
                    // 判断EXE阶段是否为load指令，若是则触发暂停
                    if(exe_op == `OP_LW || exe_op == `OP_LH || exe_op == `OP_LHU || 
                       exe_op == `OP_LB || exe_op == `OP_LBU) 
                    begin
                        is_rt_forward   <= 1'b1;
                        stall           <= 1'b1;
                        fwd             <= 1'b1;
                    end 
                    else  // 非load指令，直接转发EXE阶段写回数据
                    begin
                        is_rt_forward   <= 1'b1;
                        rt_data         <= exe_rd_data;
                        fwd             <= 1'b1;
                    end
                end 
                else if(mem_rd_wena && rt_rena && mem_rd_addr == rt_addr)  // MEM阶段有对应写回
                begin
                    is_rt_forward   <= 1'b1;
                    rt_data         <= mem_rd_data;  // 转发MEM阶段写回数据
                    fwd             <= 1'b1;
                end
            end
        end
	end      

endmodule