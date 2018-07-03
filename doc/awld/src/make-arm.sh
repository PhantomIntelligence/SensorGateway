ARMDIR=../bin
export CROSS_COMPILE=arm-xilinx-linux-gnueabi-
#this line for 14.2 environment
source /opt/Xilinx/14.2/ISE_DS/settings64.sh &> /dev/null
source /opt/Xilinx/14.2/SDK/settings64.sh &> /dev/null
#source /opt/windriver/wrlinux-small/8.0-xilinx-zynq/environment-setup-armv7a-vfp-neon-wrs-linux-gnueabi
#select this line for 16.4 envrionment (on Eric-Surface)
#source /opt/Xilinx/SDK/2016.3/settings64.sh &> /dev/null

make clean
make $*
${CROSS_COMPILE}strip awld
if [ ! -d $ARMDIR ]
then
	mkdir -p $ARMDIR
fi
if [ -d $ARMDIR ]
then
	mv awld $ARMDIR
	rm *.o
fi
