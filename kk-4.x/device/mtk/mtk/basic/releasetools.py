# Copyright (C) 2009 The Android Open Source Project
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#      http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.

"""Emit commands needed for Motorola devices during OTA installation
(installing the MBM, CDT, LBL and BP images)."""
import os
import common

def FullOTA_InstallEnd(info):
    print "FullOTA_InstallEnd"
    out_put_path = os.getenv('ANDROID_PREBUILT_DIR')
    loader_name =  os.getenv('MODEL') + '_secure_emmcboot.bin'

    WriteToOTAPackage(info,"/loader",out_put_path,loader_name)
    WriteToOTAPackage(info,"/uboot_env",out_put_path,"uenv.bin")	
    WriteToOTAPackage(info,"/aq",out_put_path,"AQ.bin")  
    #WriteToOTAPackage(info,"/pq",out_put_path,"pq.bin")  
    #fota changed by james peng begin    
    # append tcldata.img
    try:
      tv_database_img = info.input_zip.read("TCLTVIMG/tcldata.img")
    except KeyError:
      print "no tcldata.img in TCLTVIMG; skipping install"
    else:
      common.ZipWriteStr(info.output_zip, "TCLTVIMG/tcldata.img", tv_database_img)
      fstab = info.info_dict["fstab"]

      info.script.Print("TCL Writing database.img...")
      info.script.AppendExtra('''package_extract_file("TCLTVIMG/tcldata.img", "%s");''' %
                            (fstab["/database"].device,))

    # append tclconfig.img
    try:
      tv_config_img = info.input_zip.read("TCLTVIMG/tclconfig.img")
    except KeyError:
      print "no tclconfig.img in TCLTVIMG; skipping install"
    else:
      common.ZipWriteStr(info.output_zip, "TCLTVIMG/tclconfig.img", tv_config_img)
      fstab = info.info_dict["fstab"]

      info.script.Print("TCL Writing config.img...")
      info.script.AppendExtra('''package_extract_file("TCLTVIMG/tclconfig.img", "%s");''' %
                            (fstab["/tclconfig"].device,))

    # append tvos.img
    try:
      tv_tvos_img = info.input_zip.read("TCLTVIMG/tvos.img")
    except KeyError:
      print "no mid_base.img in TCLTVIMG; skipping install"
    else:
      common.ZipWriteStr(info.output_zip, "TCLTVIMG/tvos.img", tv_tvos_img)
      fstab = info.info_dict["fstab"]

      info.script.Print("TCL Writing tvos.img...")
      info.script.AppendExtra('''package_extract_file("TCLTVIMG/tvos.img", "%s");''' %
                            (fstab["/midbase"].device,))
    #fota changed by james peng end
    
def IncrementalOTA_InstallEnd(info):
    print "IncrementalOTA_InstallEnd"
    loader_name =  os.getenv('MODEL') + '_secure_emmcboot.bin'

    WriteToIncrementalPackage(info,"/loader",loader_name)
    WriteToIncrementalPackage(info,"/uboot_env","uenv.bin")
    WriteToIncrementalPackage(info,"/aq","AQ.bin")  
    #WriteToIncrementalPackage(info,"/pq","pq.bin")  
    #fota changed by james peng begin  
    # append tcldata.img
    if CompareImage("TCLTVIMG/tcldata.img", info.source_tmp, info.target_tmp):
      tv_database_img = info.target_zip.read("TCLTVIMG/tcldata.img")
      common.ZipWriteStr(info.output_zip, "TCLTVIMG/tcldata.img", tv_database_img)
      fstab = info.info_dict["fstab"]

      info.script.Print("TCL Writing database.img...")
      info.script.AppendExtra('''package_extract_file("TCLTVIMG/tcldata.img", "%s");''' %
                            (fstab["/database"].device,))

    # append tclconfig.img
    if CompareImage("TCLTVIMG/tclconfig.img", info.source_tmp, info.target_tmp):
      tv_config_img = info.target_zip.read("TCLTVIMG/tclconfig.img")
      common.ZipWriteStr(info.output_zip, "TCLTVIMG/tclconfig.img", tv_config_img)
      fstab = info.info_dict["fstab"]

      info.script.Print("TCL Writing tclconfig.img...")
      info.script.AppendExtra('''package_extract_file("TCLTVIMG/tclconfig.img", "%s");''' %
                            (fstab["/tclconfig"].device,))


    # append tvos.img
    if CompareImage("TCLTVIMG/tvos.img", info.source_tmp, info.target_tmp):
      tv_tvos_img = info.target_zip.read("TCLTVIMG/tvos.img")
      common.ZipWriteStr(info.output_zip, "TCLTVIMG/tvos.img", tv_tvos_img)
      fstab = info.info_dict["fstab"]

      info.script.Print("TCL Writing tvos.img...")
      info.script.AppendExtra('''package_extract_file("TCLTVIMG/tvos.img", "%s");''' %
                            (fstab["/midbase"].device,))
    #fota changed by james peng end

def WriteToOTAPackage(info, dev_name, bin_path, bin_name):
  try:
    common.ZipWriteStr(info.output_zip, bin_name,
                       open(bin_path + '/' + bin_name).read())

  except KeyError:
    print ("warning: no "+ bin_name +" in input target_files; ")

  try:
    info.script.WriteRawImage(dev_name, bin_name)
  except KeyError:
    print ("warning: "+ bin_name +" write script failed;")

def WriteToIncrementalPackage(info, dev_name, bin_name):
  try:
    file_name =  bin_name;
    target = info.target_zip.read(file_name);
    try:
      source = info.source_zip.read(file_name);
      if source == target:
        print(dev_name + " image unchanged; skipping")
      else:
        print(dev_name + " image changed; including")
        common.ZipWriteStr(info.output_zip, bin_name,target)
        info.script.WriteRawImage(dev_name, bin_name)
    except KeyError:
      print("warning: no "+ bin_name +" in source_files; just use target")
      out_put_path = os.getenv('ANDROID_PREBUILT_DIR')	  
      WriteToOTAPackage(info, dev_name,out_put_path, bin_name)
  except KeyError:
    print("warning: no "+ bin_name +" in target_files; not flashing")

#fota changed by james peng begin
def CompareImage(partition, source_dir, target_dir):
  """Compare tv images to judge if image should be update.
  """
  source_dir = os.path.join(source_dir, partition)
  target_dir = os.path.join(target_dir, partition)
  if (os.path.exists(target_dir) == False):
    print "The image (%s) is not exist!\n" % (partition,)
    return False
  if (os.path.exists(source_dir) == False):
    return True

  source_file = common.File.FromLocalFile(partition, source_dir)
  target_file = common.File.FromLocalFile(partition, target_dir)
  if (source_file.data != target_file.data):
    print "The image (%s) is different!\n" % (partition,)
    return True
  else:
    print "The image (%s) is the same!\n" % (partition,)
    return False
#fota changed by james peng end
