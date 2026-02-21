module encoder83 (
    input [7:0] iData,  
    output [2:0] oData  
);
    wire [7:0] en;

    assign en[7] = iData[7];
    assign en[6] = iData[6];
    assign en[5] = iData[5];
    assign en[4] = iData[4];
    assign en[3] = iData[3];
    assign en[2] = iData[2];
    assign en[1] = iData[1];
    assign en[0] = iData[0];

    assign oData[2] = en[4] | en[5] | en[6] | en[7];
    assign oData[1] = en[2] | en[3] | en[6] | en[7];
    assign oData[0] = en[1] | en[3] | en[5] | en[7];

endmodule