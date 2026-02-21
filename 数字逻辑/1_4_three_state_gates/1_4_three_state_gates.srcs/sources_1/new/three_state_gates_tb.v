`timescale 1ns/1ns

module three_state_gates_tb;
  reg iA;
  reg iEna;
  wire oTriState;

  // 实例化三态门模块
  three_state_gates uut (
    .iA(iA),
    .iEna(iEna),
    .oTri(oTriState)
  );

  initial begin
    // 初始化输入信号
    iA = 0;
    #40 iA = 1;
    #40 iA = 0;
    #40 iA = 1;
  end

  initial begin
    // 初始化使能信号
    iEna = 1;
    #20 iEna = 0;
    #40 iEna = 1;
    #20 iEna = 0;
  end
endmodule
