module CP0( 
input clk, 
input rst, 
input mfc0, // CPU指令Mfc0 
input mtc0, // CPU指令Mtc0 
input [31:0]pc, 
input [4:0] Rd, // 指定Cp0寄存器
input [31:0] wdata, // 数据从GP寄存器到CP0寄存器
input exception, 
input eret, // 指令ERET (Exception Return) 
input [4:0]cause, 
input intr,  // 外设中断请求
output [31:0] rdata, //数据从 CP0寄存器到GP寄存器
output [31:0] status, 
output reg timer_int,  // 定时产生中断，定时器的中断输出，现在不做了
output [31:0]exc_addr // 异常起始地址
);

parameter Status = 4'd12,       //STATUS寄存器是12号寄存器
          Cause  = 4'd13,       //CAUSE寄存器是13号寄存器
          EPC    = 4'd14;       //EPC是14号寄存器

reg [31:0] cp0_reg [0:31];

assign status = cp0_reg[Status];

// 定时器中断，暂未实现
always @(*) timer_int = 1'b0;

// 内部信号
wire syscall_ex = (cause == 5'b01000);  // syscall异常编码
wire break_ex   = (cause == 5'b01001);  // break异常编码
wire teq_ex     = (cause == 5'b01101);  // teq异常编码

// 计算异常是否被屏蔽
wire syscall_enable = cp0_reg[Status][8] && syscall_ex; // IM位（屏蔽位）为1，表未被屏蔽，即允许中断
wire break_enable   = cp0_reg[Status][9] && break_ex;
wire teq_enable     = cp0_reg[Status][10] && teq_ex;
wire exception_enable = syscall_enable || break_enable || teq_enable;

// 异常或中断发生
wire ex_occur = (exception && exception_enable && cp0_reg[Status][0]) || 
                (intr && cp0_reg[Status][0]); // 异常或中断发生，IE为1表示允许中断，0表示不允许中断
wire [4:0] actual_cause = (intr && cp0_reg[Status][0]) ? 5'b00000 : cause; // excCode为0表示外部中断
assign exc_addr = ex_occur ? 32'h4 : 32'b0;

// rdata是唯一的数据输出，优先处理eret（输出epc），其次mfc0
assign rdata = (eret) ? cp0_reg[EPC] : 
               (mfc0) ? cp0_reg[Rd] : 
               32'b0;

// 异常处理
always @(negedge clk or posedge rst) begin
    //integer i;
    
    if (rst) begin
        //for (i = 0; i < 32; i = i + 1) begin
            //cp0_reg[i] <= 32'b0;
        //end
         cp0_reg [0] <=0 ;
         cp0_reg [1] <=0 ;
         cp0_reg [2] <=0 ;
         cp0_reg [3] <=0 ;
         cp0_reg [4] <=0 ;
         cp0_reg [5] <=0 ;
         cp0_reg [6] <=0 ;
         cp0_reg [7] <=0 ;
         cp0_reg [8] <=0 ;
         cp0_reg [9] <=0 ;
         cp0_reg [10] <=0 ;
         cp0_reg [11] <=0 ;
         cp0_reg [12] <=0 ;
         cp0_reg [13] <=0 ;
         cp0_reg [14] <=0 ;
         cp0_reg [15] <=0 ;
         cp0_reg [16] <=0 ;
         cp0_reg [17] <=0 ;
         cp0_reg [18] <=0 ;
         cp0_reg [19] <=0 ;
         cp0_reg [20] <=0 ;
         cp0_reg [21] <=0 ;
         cp0_reg [22] <=0 ;
         cp0_reg [23] <=0 ;
         cp0_reg [24] <=0 ;
         cp0_reg [25] <=0 ;
         cp0_reg [26] <=0 ;
         cp0_reg [27] <=0 ;
         cp0_reg [28] <=0 ;
         cp0_reg [29] <=0 ;
         cp0_reg [30] <=0 ;
         cp0_reg [31] <=0 ; 
    end
    else begin
        if (mtc0) begin
            cp0_reg[Rd] <= wdata;
        end
        if (ex_occur) begin
            cp0_reg[EPC] <= pc;            // 保存PC
            cp0_reg[Cause][6:2] <= actual_cause;  // 记录异常类型
            // 设置IP[0]位表示中断挂起
            //if (intr && cp0_reg[Status][0]) begin
              //cp0_reg[Cause][8] <= 1'b1;  // 设置中断挂起位
            //end
            cp0_reg[Status] <= cp0_reg[Status]  << 5; // 关中断
        end
        else if (eret) begin
            cp0_reg[Status] <= cp0_reg[Status] >> 5; // 开中断（恢复）
        end        
        // 更新中断挂起状态 (IP位)
        //if (!ex_occur) begin
            //cp0_reg[Cause][8] <= intr;
        //end
    end
end

endmodule