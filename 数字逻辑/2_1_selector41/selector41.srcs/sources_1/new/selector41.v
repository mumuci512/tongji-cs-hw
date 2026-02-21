module selector41 (
    input [3:0] iC0, iC1, iC2, iC3,  
    input iS1, iS0,                  
    output [3:0] oZ                  
);
wire [3:0] wZ0, wZ1;
    mux2_1 mux1 (.iA(iC0), .iB(iC1), .sel(iS0), .oZ(wZ0));
    mux2_1 mux2 (.iA(iC2), .iB(iC3), .sel(iS0), .oZ(wZ1));
    mux2_1 mux3 (.iA(wZ0), .iB(wZ1), .sel(iS1), .oZ(oZ));
endmodule
// 2选1选择器子模块
module mux2_1 (
    input [3:0] iA, iB, 
    input sel,          
    output [3:0] oZ     
);
assign oZ = sel ? iB : iA;
endmodule