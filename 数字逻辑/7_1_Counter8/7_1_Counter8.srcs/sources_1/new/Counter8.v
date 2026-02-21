// JK触发器模块
module JK_FF(
    input CLK,        // 时钟信号
    input J,          // J输入
    input K,          // K输入
    input RST_n,      // 复位信号，低电平有效
    output reg Q1,    // 输出Q1
    output reg Q2     // 输出Q2
);

    always @(posedge CLK or negedge RST_n) begin
        if (!RST_n) begin
            Q1 <= 0;  
            Q2 <= 1;  
        end else begin
            case ({J, K})
                2'b00: ;  // 保持状态
                2'b01: begin
                    Q1 <= 0; 
                    Q2 <= 1;
                end
                2'b10: begin
                    Q1 <= 1; 
                    Q2 <= 0;
                end
                2'b11: begin
                    Q1 <= ~Q1; 
                    Q2 <= ~Q2;
                end
            endcase
        end
    end

endmodule

// 七段显示管模块
module display7( 
    input [3:0] iData,  // 输入数据（四位二进制数）
    output [6:0] oData  // 输出七段显示管的控制信号
);

    wire [6:0] seg_out;

    assign seg_out[0] = (~iData[3] & ~iData[2] & ~iData[1] &  iData[0]) | 
                        (~iData[3] &  iData[2] & ~iData[1] & ~iData[0]) | 
                        ( iData[3] & ~iData[2] &  iData[1] &  iData[0]) | 
                        ( iData[3] &  iData[2] & ~iData[1] &  iData[0]);

    assign seg_out[1] = (~iData[3] &  iData[2] & ~iData[1] &  iData[0]) | 
                        ( iData[3] &  iData[2] & ~iData[0]) | 
                        ( iData[2] &  iData[1] & ~iData[0]) | 
                        ( iData[3] &  iData[1] &  iData[0]);

    assign seg_out[2] = (~iData[3] & ~iData[2] &  iData[1] & ~iData[0]) | 
                        ( iData[3] &  iData[2] &  iData[0]) | 
                        ( iData[3] &  iData[2] &  iData[1]);

    assign seg_out[3] = (~iData[3] & ~iData[2] & ~iData[1] &  iData[0]) | 
                        (~iData[3] &  iData[2] & ~iData[1] & ~iData[0]) | 
                        ( iData[2] &  iData[1] &  iData[0]) | 
                        ( iData[3] & ~iData[2] &  iData[1] & ~iData[0]);

    assign seg_out[4] = (~iData[3] &  iData[0]) | 
                        (~iData[3] &  iData[2] & ~iData[1]) | 
                        (~iData[2] & ~iData[1] &  iData[0]);

    assign seg_out[5] = (~iData[3] & ~iData[2] &  iData[0]) | 
                        (~iData[3] & ~iData[2] &  iData[1]) | 
                        (~iData[3] &  iData[1] &  iData[0]) | 
                        ( iData[3] &  iData[2] & ~iData[1] &  iData[0]);

    assign seg_out[6] = (~iData[3] & ~iData[2] & ~iData[1]) | 
                        (~iData[3] &  iData[2] &  iData[1] &  iData[0]) | 
                        ( iData[3] &  iData[2] & ~iData[1] & ~iData[0]);

    assign oData = seg_out;  // 输出七段显示管控制信号
endmodule

module Divider (
    input I_CLK,         //输入时钟信号，上升沿有效
    input rst,           //同步复位信号，低电平有效
    output reg O_CLK     //输出时钟
);
    //参数默认为20
    parameter DIV_RATIO = 100000000;
    
    reg [31:0] counter;  //32位计数器存储计数值
    
    initial begin
        O_CLK = 0;     
        counter = 0;   
    end
    
    //每次 I_CLK上升沿触发，同步复位
    always @(posedge I_CLK) begin
        if (rst) begin
            counter <= 0;
            O_CLK <= 0; 
        end else begin
            //计数器达到分频倍数的一半时
            if (counter == DIV_RATIO / 2 - 1) begin
                counter <= 0;  
                O_CLK <= ~O_CLK; 
            end else begin
                counter <= counter + 1;  
            end
        end
    end

endmodule
// 模8计数器模块
module Counter8( 
    input CLK,            // 时钟信号
    input rst_n,          // 复位信号，低电平有效
    output [2:0] oQ,      // 三位计数器输出（Q2, Q1, Q0）
    output [6:0] oDisplay // 七段显示管输出
);

    // 声明分频器模块实例
    wire O_CLK;           // 分频器输出时钟

    // 实例化分频器，假设 DIV_RATIO 默认为20
    Divider #(100000000) divider_inst (
        .I_CLK(CLK),    // 输入时钟信号
        .rst(~rst_n),    // 同步复位信号
        .O_CLK(O_CLK)   // 输出时钟信号
    );

    // 声明三个JK触发器实例
    wire Q0, Q1, Q2;  // JK触发器的输出

    // 实例化三个JK触发器，作为三位二进制计数器
    JK_FF FF0(
        .CLK(O_CLK),   // 使用分频器的输出时钟作为JK触发器的时钟
        .J(1'b1),      // J=1, K=1，JK触发器为切换模式
        .K(1'b1),
        .RST_n(rst_n),
        .Q1(Q0),
        .Q2()           // 不使用Q2
    );

    JK_FF FF1(
        .CLK(O_CLK),   // 级联，FF1的时钟输入为FF0的Q输出
        .J(Q0),
        .K(Q0),
        .RST_n(rst_n),
        .Q1(Q1),
        .Q2()           // 不使用Q2
    );

    wire Q3;
    and(Q3, Q0, Q1);

    JK_FF FF2(
        .CLK(O_CLK),   // 级联，FF2的时钟输入为FF1的Q输出
        .J(Q3),
        .K(Q3),
        .RST_n(rst_n),
        .Q1(Q2),
        .Q2()           // 不使用Q2
    );

    // 将Q1, Q2, Q3作为计数器的输出
    assign oQ = {Q2, Q1, Q0};  

    // 4位中间变量，用于七段显示管的输入
    wire [3:0] oQ_extended;  // 4位中间变量

    // 将3位计数器扩展为4位，MSB设为0
    assign oQ_extended = {1'b0, Q2, Q1, Q0};  // 最高位为0，扩展为4位
    
    // 调用七段显示管模块，将计数器的值转换为七段显示输出
    display7 display7_inst (
        .iData(oQ_extended),   // 输入数据为计数器的输出
        .oData(oDisplay)       // 七段显示管输出
    );

endmodule

