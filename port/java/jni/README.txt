To generate new JNI bindings, you should do the following:


LINUX:

cd $GAMS_ROOT/port/java/src
javah -d jni com.gams.<whatever you're generating>



WINDOWS:

cd %GAMS_ROOT%/port/java/src
javah -d ../jni com.gams.<whatever you're generating>
