module tb_de_selector14();
    reg iC;
    reg iS1, iS0;
    wire oZ0, oZ1, oZ2, oZ3;

    // 实例化被测试的模块
    de_selector14 uut (
        .iC(iC),
        .iS1(iS1),
        .iS0(iS0),
        .oZ0(oZ0),
        .oZ1(oZ1),
        .oZ2(oZ2),
        .oZ3(oZ3)
    );

    initial begin
        // 测试用例1：iC=0, iS1=0, iS0=0
        iC = 0; iS1 = 0; iS0 = 0;
        #10;
        $display("iC=%b, iS1=%b, iS0=%b -> oZ0=%b, oZ1=%b, oZ2=%b, oZ3=%b", iC, iS1, iS0, oZ0, oZ1, oZ2, oZ3);

        // 测试用例2：iC=0, iS1=0, iS0=1
        iC = 0; iS1 = 0; iS0 = 1;
        #10;
        $display("iC=%b, iS1=%b, iS0=%b -> oZ0=%b, oZ1=%b, oZ2=%b, oZ3=%b", iC, iS1, iS0, oZ0, oZ1, oZ2, oZ3);

        // 测试用例3：iC=0, iS1=1, iS0=0
        iC = 0; iS1 = 1; iS0 = 0;
        #10;
        $display("iC=%b, iS1=%b, iS0=%b -> oZ0=%b, oZ1=%b, oZ2=%b, oZ3=%b", iC, iS1, iS0, oZ0, oZ1, oZ2, oZ3);

        // 测试用例4：iC=0, iS1=1, iS0=1
        iC = 0; iS1 = 1; iS0 = 1;
        #10;
        $display("iC=%b, iS1=%b, iS0=%b -> oZ0=%b, oZ1=%b, oZ2=%b, oZ3=%b", iC, iS1, iS0, oZ0, oZ1, oZ2, oZ3);

        // 结束仿真
        #10 $finish;
    end
endmodule
