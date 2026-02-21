module encoder83_Pri (
    input [7:0] iData,     
    input iEI,             
    output reg [2:0] oData,
    output reg oEO        
);
    always @(*) begin
        if (!iEI) begin //iEIΪ0
            if(iData[7]==1'b0) begin
                oData = 3'b000;
            end
            if(iData[7]==1'b1&&iData[6]==1'b0) begin
                oData = 3'b001;
            end
            if(iData[7]==1'b1&&iData[6]==1'b1&&iData[5]==1'b0) begin
                oData = 3'b010;
            end
            if(iData[7]==1'b1&&iData[6]==1'b1&&iData[5]==1'b1&&iData[4]==1'b0) begin
                oData = 3'b011;
            end
            if(iData[7]==1'b1&&iData[6]==1'b1&&iData[5]==1'b1&&iData[4]==1'b1&&iData[3]==1'b0) begin
                oData = 3'b100;
            end
            if(iData[7]==1'b1&&iData[6]==1'b1&&iData[5]==1'b1&&iData[4]==1'b1&&iData[3]==1'b1&&iData[2]==1'b0) begin
                oData = 3'b101;
            end
            if(iData[7]==1'b1&&iData[6]==1'b1&&iData[5]==1'b1&&iData[4]==1'b1&&iData[3]==1'b1&&iData[2]==1'b1&&iData[1]==1'b0) begin
                oData = 3'b110;
            end
            if(iData[7]==1'b1&&iData[6]==1'b1&&iData[5]==1'b1&&iData[4]==1'b1&&iData[3]==1'b1&&iData[2]==1'b1&&iData[1]==1'b1&&iData[0]==1'b0) begin
                oData = 3'b111;
            end
            if(iData==8'b11111111) begin
                oData=3'b111;
            end
            oEO = (iData != 8'b11111111);
        end else begin //iEIΪ1
            oData = 3'b111;
            oEO = 1'b1;
        end
    end
endmodule
