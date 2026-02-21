module cutter(
    input [31:0]    data_in, 
    input [2:0]     cutter_sel,
    input           sign_ext, 
    output reg [31:0] data_out
    );
    
    always@(*) begin
        case(cutter_sel)
            // 有符号字节扩展
            3'b010: data_out <= { {24{sign_ext & data_in[7]}}, data_in[7:0] };
            // 无符号字节扩展  
            3'b011: data_out <= { 24'b0, data_in[7:0] };
            // 有符号半字扩展
            3'b001: data_out <= { {16{sign_ext & data_in[15]}}, data_in[15:0] };
            // 无符号半字扩展
            3'b100: data_out <= { 16'b0, data_in[15:0] };
            // 默认全字直接输出
            default: data_out <= data_in;
        endcase
    end

endmodule