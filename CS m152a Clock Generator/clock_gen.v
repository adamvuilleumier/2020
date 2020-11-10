`timescale 1ns / 1ps
//////////////////////////////////////////////////////////////////////////////////
// Company: 
// Engineer: 
// 
// Create Date:    20:31:10 11/06/2020 
// Design Name: 
// Module Name:    verification_task 
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

module clock_gen(
	input clk_in,
	input rst,
	output clk_div_2,
	output clk_div_4,
	output clk_div_8,
	output clk_div_16,
	output clk_div_28,
	output clk_div_5,
	output [7:0] toggle_counter
	);
clock_div_two task_one(
	.clk_in (clk_in),
	.rst (rst),
	.clk_div_2(clk_div_2),
	.clk_div_4(clk_div_4),
	.clk_div_8(clk_div_8),
	.clk_div_16(clk_div_16)
	);
clock_div_twenty_eight task_two(
	.clk_in (clk_in),
	.rst (rst),
	.clk_div_28 (clk_div_28)
	);
clock_div_five task_three(
	.clk_in (clk_in),
	.rst (rst),
	.clk_div_5 (clk_div_5)
	);
clock_strobe task_four(
	.clk_in (clk_in),
	.rst (rst),
	.toggle_counter (toggle_counter)
	);
endmodule

module clock_div_two(
	input clk_in,
	input rst,
	output clk_div_2,
	output clk_div_4,
	output clk_div_8,
	output clk_div_16
	);
	
	reg [3:0] a;
	always@(posedge clk_in) begin
		if (rst)
			a <= 4'b0000;
		else
			a <= a + 1'b1;
	end
	
	assign clk_div_2 = a[0];
	assign clk_div_4 = a[1];
	assign clk_div_8 = a[2];
	assign clk_div_16 = a[3];
endmodule 

module clock_div_twenty_eight(
	input clk_in,
	input rst,
	output clk_div_28
	);
	
	reg clk_div_28_reg = 1'b0;
	reg [3:0] b;
	
	assign clk_div_28 = clk_div_28_reg;
	
	always@(posedge clk_in) begin
		if (rst)
			b <= 4'b0000;
		else if (b == 4'b1101) begin
			b <= 4'b0000;
			if (clk_div_28_reg == 0)
				clk_div_28_reg <= 1;
			else
				clk_div_28_reg <= 0;
		end
		else
			b <= b + 1'b1;
	end
endmodule 

module clock_div_five(
	input clk_in,
	input rst,
	output clk_div_5
	);
	
	reg [2:0] pos_count, neg_count;
 
	always @(posedge clk_in) begin
		if (rst)
			pos_count <= 0;
		else if (pos_count == 3'b100) 
			pos_count <= 0;
		else 
			pos_count <= pos_count + 1'b1;
	end
	 
	 always @(negedge clk_in) begin
		 if (rst)
			neg_count <=0;
		 else  if (neg_count == 3'b100) 
			neg_count <= 0;
		 else 
			neg_count <= neg_count + 1'b1; 
	 end
	 
	 assign clk_div_5 = ((pos_count > 3'b010) | (neg_count > 3'b010));

endmodule

module clock_strobe(
	input clk_in,
	input rst,
	output [7:0] toggle_counter
	);
	
	reg [31:0] c;
	reg [7:0] toggle_reg;
	
	always@(posedge clk_in) begin
		if (rst) begin
			toggle_reg <= 8'b0;
			c <= 1;
		end
		else begin
			if (c % 4 == 0)
				toggle_reg <= toggle_reg - 3'b101;
			else
				toggle_reg <= toggle_reg + 2'b10;
			c <= c + 1'b1;
		end
	end
	
	assign toggle_counter = toggle_reg;
	
endmodule 
