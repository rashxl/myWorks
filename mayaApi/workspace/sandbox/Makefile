build:
	#compiler to .o
	g++ -I/a-vfx/stuff/programs/linux/Maya2017/include/ -std=c++11 -fPIC -fpermissive -c *.cpp
	#link .o to .so
	g++ -shared -L/a-vfx/stuff/programs/linux/Maya2017/lib/ -lOpenMaya -lOpenMayaRender  -lOpenMayaUI -lFoundation -lOpenMayaAnim -lGL -o librash.so *.o -Wl,-z,defs
	#rename so
	mv librash.so rash.so
clear:
	rm -f *.o *.so

clean:
	rm -f *.o