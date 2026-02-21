`timescale 1ns/1ns

module logic_gates_tb;
  reg iA;
  reg iB;
  wire oAnd;
  wire oOr;
  wire oNot;

  // 实例化与、或、非门模块
  logic_gates_1 uut (
    .iA(iA),
    .iB(iB),
    .oAnd(oAnd),
    .oOr(oOr),
    .oNot(oNot)
  );

  initial begin
    // 初始化输入
    iA = 0; iB = 0;
    #40 iA = 1; iB = 0;
    #40 iA = 0; iB = 1;
    #40 iA = 1; iB = 1;
    #40 iA = 0; iB = 0;
  end
endmodule
