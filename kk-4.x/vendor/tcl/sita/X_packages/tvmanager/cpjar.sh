#!/bin/bash

cp ../../../../../out/target/common/obj/JAVA_LIBRARIES/com.tcl.factory_intermediates/classes-full-debug.jar ./com.tcl.factory.jar
cp ../../../../../out/target/common/obj/JAVA_LIBRARIES/com.tcl.tvmanager_intermediates/classes-full-debug.jar com.tcl.tvmanager.jar
cp ../../../../../out/target/common/obj/JAVA_LIBRARIES/com.tcl.snapshot_intermediates/classes-full-debug.jar com.tcl.snapshot.jar
cp ../../../../../out/target/product/mt5882/symbols/system/lib/libcom_tcl_tv_jni.so .

rm -rf debug
mkdir debug
cp ../../../../../out/target/product/mt5882/system/framework/com.tcl.factory.jar debug/
cp ../../../../../out/target/product/mt5882/system/framework/com.tcl.factory.odex debug/
cp ../../../../../out/target/product/mt5882/system/framework/com.tcl.tvmanager.jar debug/
cp ../../../../../out/target/product/mt5882/system/framework/com.tcl.tvmanager.odex debug/
cp ../../../../../out/target/product/mt5882/symbols/system/lib/libcom_tcl_tv_jni.so debug/
cp ../../../../../out/target/product/mt5882/system/framework/com.tcl.snapshot.jar debug/
cp ../../../../../out/target/product/mt5882/system/framework/com.tcl.snapshot.odex debug/

