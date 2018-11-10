`define SCREEN_WIDTH 176
`define SCREEN_HEIGHT 144

///////* DON'T CHANGE THIS PART *///////
module DE0_NANO(
	CLOCK_50,
	GPIO_0_D,
	GPIO_1_D,
	KEY
);

//=======================================================
//  PARAMETER declarations
//=======================================================
localparam RED = 8'b111_000_00;
localparam GREEN = 8'b000_111_00;
localparam BLUE = 8'b000_000_11;

//=======================================================
//  PORT declarations
//=======================================================

//////////// CLOCK - DON'T NEED TO CHANGE THIS //////////
input 		          		CLOCK_50;

//////////// GPIO_0, GPIO_0 connect to GPIO Default //////////
output 		    [33:0]		GPIO_0_D;
//////////// GPIO_0, GPIO_1 connect to GPIO Default //////////
input 		    [33:0]		GPIO_1_D;
input 		     [1:0]		KEY;

///// PIXEL DATA /////
reg [7:0]	pixel_data_RGB332 = 8'd0;

///// READ/WRITE ADDRESS /////
reg [14:0] X_ADDR;
reg [14:0] Y_ADDR;
wire [14:0] WRITE_ADDRESS;
reg [14:0] READ_ADDRESS; 

assign WRITE_ADDRESS = X_ADDR + Y_ADDR*(`SCREEN_WIDTH);

///// VGA INPUTS/OUTPUTS /////
wire 		VGA_RESET;
wire [7:0]	VGA_COLOR_IN;
wire [9:0]	VGA_PIXEL_X;
wire [9:0]	VGA_PIXEL_Y;
wire [7:0]	MEM_OUTPUT;
wire		VGA_VSYNC_NEG;
wire		VGA_HSYNC_NEG;
reg			VGA_READ_MEM_EN;

assign GPIO_0_D[5] = VGA_VSYNC_NEG;
assign VGA_RESET = ~KEY[0];

///// I/O for Img Proc /////
wire [7:0] RESULT;

//CHANGE GPIO HERE TODO ASSIGN GPIO PINS
wire[7:0] CAMERA_IN;
assign CAMERA_IN = {GPIO_1_D[22], GPIO_1_D[20], GPIO_1_D[18], 
					GPIO_1_D[16], GPIO_1_D[14], GPIO_1_D[24], 
					GPIO_1_D[10], GPIO_1_D[8]};
wire HREF = GPIO_1_D[2];
wire VSYNC = GPIO_1_D[4];
wire PCLK = GPIO_1_D[6];


/* WRITE ENABLE */
reg W_EN;

///////* CREATE ANY LOCAL WIRES YOU NEED FOR YOUR PLL *///////
wire CLK24;
wire CLK25;
wire CLK50;
assign GPIO_0_D[1] = CLK24;

///////* INSTANTIATE YOUR PLL HERE *///////
T26PLL	T26PLL_inst (
	.inclk0 ( CLOCK_50 ),
	.c0 ( CLK24 ),
	.c1 ( CLK25 ),
	.c2 ( CLK50 )
);

///////* M9K Module *///////
Dual_Port_RAM_M9K mem(
	.input_data(pixel_data_RGB332),
	.w_addr(WRITE_ADDRESS),
	.r_addr(READ_ADDRESS),
	.w_en(W_EN),
	.clk_W(CLK50),
	.clk_R(CLK25), 
	.output_data(MEM_OUTPUT)
);

///////* VGA Module *///////
VGA_DRIVER driver (
	.RESET(VGA_RESET),
	.CLOCK(CLK25),
	.PIXEL_COLOR_IN(VGA_READ_MEM_EN ? MEM_OUTPUT : BLUE),
	.PIXEL_X(VGA_PIXEL_X),
	.PIXEL_Y(VGA_PIXEL_Y),
	.PIXEL_COLOR_OUT({GPIO_0_D[9],GPIO_0_D[11],GPIO_0_D[13],GPIO_0_D[15],GPIO_0_D[17],GPIO_0_D[19],GPIO_0_D[21],GPIO_0_D[23]}),
    .H_SYNC_NEG(GPIO_0_D[7]),
    .V_SYNC_NEG(VGA_VSYNC_NEG)
);

///////* Image Processor *///////
IMAGE_PROCESSOR proc(
	.PIXEL_IN(MEM_OUTPUT),
	.CLK(CLK25),
	.VGA_PIXEL_X(VGA_PIXEL_X),
	.VGA_PIXEL_Y(VGA_PIXEL_Y),
	.VGA_VSYNC_NEG(VGA_VSYNC_NEG),
	.RESULT(RESULT)
);


///////* Update Read Address *///////
always @ (VGA_PIXEL_X, VGA_PIXEL_Y) begin
		READ_ADDRESS = (VGA_PIXEL_X + VGA_PIXEL_Y*`SCREEN_WIDTH);
		if(VGA_PIXEL_X>(`SCREEN_WIDTH-1) || VGA_PIXEL_Y>(`SCREEN_HEIGHT-1))begin
				VGA_READ_MEM_EN = 1'b0;
		end
		else begin
			VGA_READ_MEM_EN = 1'b1;

			/* test pattern*/
			if (VGA_PIXEL_X==VGA_PIXEL_Y) pixel_data_RGB332 = RED;
			else  pixel_data_RGB332 = GREEN;
				
		end
end

//DOWNSAMPLER Module
reg high;
initial high = 0;

always @(posedge VSYNC or negedge HREF) begin
	if((VSYNC) Y_ADDR = 0;
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
			pixel_data_RGB332 = 0;
		end
	end
end

	
endmodule 