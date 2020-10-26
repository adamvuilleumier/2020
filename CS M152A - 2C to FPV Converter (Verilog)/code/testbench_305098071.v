`timescale 1ns / 1ps

////////////////////////////////////////////////////////////////////////////////
// Company: 
// Engineer:
//
// Create Date:   18:55:37 10/20/2020
// Design Name:   FPCVT
// Module Name:   C:/Users/Adam/project1_35L/testbench_305098071.v
// Project Name:  project1_35L
// Target Device:  
// Tool versions:  
// Description: 
//
// Verilog Test Fixture created by ISE for module: FPCVT
//
// Dependencies:
// 
// Revision:
// Revision 0.01 - File Created
// Additional Comments:
// 
////////////////////////////////////////////////////////////////////////////////

module testbench_305098071;

	// Inputs
	reg [12:0] D;

	// Outputs
	wire S;
	wire [2:0] E;
	wire [4:0] F;

	// Instantiate the Unit Under Test (UUT)
	FPCVT uut (
		.D(D), 
		.S(S), 
		.E(E), 
		.F(F)
	);

	initial begin
		
		D = 13'b0000000111010;   //basic case (easy 5 bit mantissa)
		#100;

		D = 13'b0111110000001; //case of FPV overflow (> 3968)
		#100;
		
		D = 13'b1000000000000; //case of FPV overflow (< -3968)
		#100;
		
		D = 13'b0000011111101; //case of rounding oversaturation of mantissa 
		//FPV rounding error: module gives 256 even though binary rep = 253
		#100;
		
		D = 13'b1110001111110; //case of rounding oversaturation and negative
		#100;
		
		D = 13'b0000000000010; //case of >8 leading 0's
		#100;
		
		D = 13'b1111111111100; //case of no leading 0's (small negative)
		#100;
		
		D = 13'b0000000000000; //case when input = 0
		#100;
	
	end
      
endmodule

