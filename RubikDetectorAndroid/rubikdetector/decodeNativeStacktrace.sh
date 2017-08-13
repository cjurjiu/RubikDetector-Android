rm -v -f ./nativeStacktraces/crashDumpRaw.txt
rm -v -f ./nativeStacktraces/crashDumpDecoded.txt
adb shell logcat -d -s DEBUG:E > ./nativeStacktraces/crashDumpRaw.txt
ndk-stack -sym ./build/intermediates/cmake/debug/obj/armeabi-v7a/ -dump ./nativeStacktraces/crashDumpRaw.txt > ./nativeStacktraces/crashDumpDecoded.txt