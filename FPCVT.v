`timescale 1ns / 1ps
//////////////////////////////////////////////////////////////////////////////////
// Company: 
// Engineer: 
// 
// Create Date:    11:16:06 10/19/2020 
// Design Name: 
// Module Name:    FPCVT 
// Project Name: 
// Target Devices: 
// Tool versions: 
// Description: 
//
// Dependencies: 
//
// Revision: 
// Revision 0.01 - File Created
// Additional Comments: 
//
//////////////////////////////////////////////////////////////////////////////////

module FPCVT(
    input wire [12:0] D, //13 bit 2's complement input vector
    output reg S,            //sign bit
    output reg [2:0] E,  //3 bit exponential
    output reg [4:0] F   //5 bit mantissa
    );

reg [11:0] mag; //the magnitude of the input bit vector
reg [4:0] mantissa; //stores temporary unrounded mantissa
reg [2:0] exponent; //stores temporary unadjusted exponent
reg [3:0] n_lzeroes;  //determines # of leading 0's in mag
reg sixth_bit;  //the "sixth bit" of the mantissa, determines rounding
reg [11:0] shifted_mag; //the shifted magnitude used to find mantissa 
integer i;

////////////////Variables declared////////////////////////////////////////////////////////////

always@(D) begin
	S = D[12]; //sign bit is always the first bit in D
end

////////////////S is set/////////////////////////////////////////////////////////////////////

always@(D or S) begin
	if (S == 0)       //if positive...
		mag = D[11:0];   //magnitude is equal to the value of D
			
	else begin       //if negative...
		if (D == 13'b1000000000000) //special case of Tmin complemented to Tmax
			mag = 12'b111111111111;		
		else          //complement and add 1
			mag = (~D[11:0] + 1'b1);
	end
			
///////////////Mag is set/////////////////////////////////////////////
		
	for (i = 0; i < 12; i=i+1) begin //iterate thru mag from LSB to MSB and record pos of last 1
		if (mag[i])
			n_lzeroes[3:0] = 11 - i;  //number of leading 0's = MSB position - current position		
	end
		
		if (n_lzeroes >= 8) //assign exponent using leading zeroes heuristic
			exponent[2:0] = 3'b000;
		else if (n_lzeroes == 0)
			exponent[2:0] = 3'b111;
		else
			exponent[2:0] = 7 - n_lzeroes[3:0];
			
/////////////exponent and n_lzeroes are set/////////////////////////////////////////////////
		
	if (exponent == 0) begin //edge case when 8+ leading 0's
		mantissa = mag[4:0];
		sixth_bit = 1'b0;
	end
	else begin
		shifted_mag = mag << n_lzeroes;  //shifts magnitude to get mantissa
		mantissa = shifted_mag[11:7];   //calculates mantissa
		sixth_bit = shifted_mag[6];    //saves unused sixth bit for rounding
	end
end
	
////////////mantissa and sixth_bit are set///////////////////////////////////////////////
	
always@(mag or mantissa or exponent or sixth_bit) begin
	if (mag > 3968) begin  //cases of FPV overflow handled first (sign bit set in beginning)
			E[2:0] = 3'b111;
			F[4:0] = 5'b11111;
	end
	else if (mantissa == 5'b11111 && exponent == 3'b111 && sixth_bit == 1'b1) begin //case of max value (rounding up would cause issues)
		F[4:0] = 5'b11111;  //highest possible floating point value
		E[2:0] = 3'b111;
	end
	else if (sixth_bit) begin  //rounding up scenario
		if (mantissa == 5'b11111) begin  //handles special case of mantissa overflow
			F[4:0] = 5'b10000;
			E[2:0] = exponent + 1'b1; //adjusts exponent since mantissa overflowed
		end
		else
			F[4:0] = mantissa + 1'b1; //adds 1 to mantissa in normal case
	end
	else begin //rounding down scenario
		F[4:0] = mantissa;  //assigns F and E to mantissa and exponent
		E[2:0] = exponent;
	end
end
endmodule		
////////////F is rounded, E adjusted/////////////////////////////////////////////////////
