all:
	#ndk-build && ant debug install&&adb shell "am start -n opengl.demo/.MainActivity"; sleep 2 && adb shell "pkill opengl.demo"
	ndk-build NDK_DEBUG=1 && ant debug install&&adb shell "am start -n opengl.demo/.MainActivity";

update:
	android update project -p . -t android-10
