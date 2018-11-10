`define SCREEN_WIDTH 176
`define SCREEN_HEIGHT 144

module DOWNSAMPLER(
	PCLK,
	HREF,
	VSYNC,
	CAMERA_IN,
	pixel_data_RGB332,
	X_ADDR,
	Y_ADDR,
	W_EN
);

input PCLK;
input HS;
input VS;
input [7:0] CAMERA_IN;

output[7:0]	pixel_data_RGB332;
output[14:0] X_ADDR;
output[14:0] Y_ADDR;
output W_EN;

//internal wires for data storage
reg[7:0] pixel_data_RGB332;
reg[14:0] X_ADDR;
reg[14:0] Y_ADDR;
reg W_EN;



//store data
// always @(posedge PCLK) begin
// 	if(high) begin
// 		full_data[7:0] = camera_data;
// 		high = 1;
// 	end
// 	else begin
// 		full_data[15:8] = camera_data;
// 		high = 0;
// 		out = {full_data[15:13], full_data[10:8], full_data[4:3]}; //downsample
// 	end
// end

// //update address
// always @(posedge VS or negedge HS or posedge PCLK) begin
// 	if(VS) begin
// 		x_addr = 0;
// 		y_addr = 0;
// 	end
// 	else begin
// 		if(HS) y_addr = y_addr+1;
// 		else if (high) x_addr = x_addr+1;
// 	end
// end //end always

//update y address
always @(negedge HREF) begin
	if((Y_ADDR >= `SCREEN_HEIGHT-1) || VSYNC) Y_ADDR = 0;
	else Y_ADDR = Y_ADDR+1;
end

//store data and update x address
always @(posedge PCLK) begin
	if(VSYNC) begin
		W_EN = 0;
		X_ADDR = 0;
		high = 0;
		pixel_data_RGB332 = 0;
	end
	else begin
		if (HREF) begin
			if(!high) begin
				W_EN = 0;
				X_ADDR = X_ADDR;
				pixel_data_RGB332[1:0] = {CAMERA_IN[4], CAMERA_IN[3]};
				high = 1;
			end
			else begin
				pixel_data_RGB332[7:5] = {CAMERA_IN[7], CAMERA_IN[6], CAMERA_IN[5]};
				pixel_data_RGB332[4:2] = {CAMERA_IN[2], CAMERA_IN[1], CAMERA_IN[0]};
				X_ADDR = X_ADDR + 1;
				high = 0;
				W_EN = 1;
			end
		end
		else begin
			W_EN = 0;
			X_ADDR = 0;
			high = 0;
			out = 0;
		end
	end
end
endmodule