#!/usr/bin/perl
# Copyright Statement:
#
# This software/firmware and related documentation ("MediaTek Software") are
# protected under relevant copyright laws. The information contained herein
# is confidential and proprietary to MediaTek Inc. and/or its licensors.
# Without the prior written permission of MediaTek inc. and/or its licensors,
# any reproduction, modification, use or disclosure of MediaTek Software,
# and information contained herein, in whole or in part, shall be strictly prohibited.
#
# MediaTek Inc. (C) 2010. All rights reserved.
#
# BY OPENING THIS FILE, RECEIVER HEREBY UNEQUIVOCALLY ACKNOWLEDGES AND AGREES
# THAT THE SOFTWARE/FIRMWARE AND ITS DOCUMENTATIONS ("MEDIATEK SOFTWARE")
# RECEIVED FROM MEDIATEK AND/OR ITS REPRESENTATIVES ARE PROVIDED TO RECEIVER ON
# AN "AS-IS" BASIS ONLY. MEDIATEK EXPRESSLY DISCLAIMS ANY AND ALL WARRANTIES,
# EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED WARRANTIES OF
# MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE OR NONINFRINGEMENT.
# NEITHER DOES MEDIATEK PROVIDE ANY WARRANTY WHATSOEVER WITH RESPECT TO THE
# SOFTWARE OF ANY THIRD PARTY WHICH MAY BE USED BY, INCORPORATED IN, OR
# SUPPLIED WITH THE MEDIATEK SOFTWARE, AND RECEIVER AGREES TO LOOK ONLY TO SUCH
# THIRD PARTY FOR ANY WARRANTY CLAIM RELATING THERETO. RECEIVER EXPRESSLY ACKNOWLEDGES
# THAT IT IS RECEIVER'S SOLE RESPONSIBILITY TO OBTAIN FROM ANY THIRD PARTY ALL PROPER LICENSES
# CONTAINED IN MEDIATEK SOFTWARE. MEDIATEK SHALL ALSO NOT BE RESPONSIBLE FOR ANY MEDIATEK
# SOFTWARE RELEASES MADE TO RECEIVER'S SPECIFICATION OR TO CONFORM TO A PARTICULAR
# STANDARD OR OPEN FORUM. RECEIVER'S SOLE AND EXCLUSIVE REMEDY AND MEDIATEK'S ENTIRE AND
# CUMULATIVE LIABILITY WITH RESPECT TO THE MEDIATEK SOFTWARE RELEASED HEREUNDER WILL BE,
# AT MEDIATEK'S OPTION, TO REVISE OR REPLACE THE MEDIATEK SOFTWARE AT ISSUE,
# OR REFUND ANY SOFTWARE LICENSE FEES OR SERVICE CHARGE PAID BY RECEIVER TO
# MEDIATEK FOR SUCH MEDIATEK SOFTWARE AT ISSUE.
#
# The following software/firmware and/or related documentation ("MediaTek Software")
# have been modified by MediaTek Inc. All revisions are subject to any receiver's
# applicable license agreements with MediaTek Inc.


# Programmer: Shengkai Lin (2009-08-05)

main();

sub
main
{
  ###############################################################
  # Read ini values
  ###############################################################
  
  my $dbfolder = "database";
  my $parsedb = "database/parse_db.c";
  my $pridb = "database/msglog_db/parse.db";
  my $ps_trace_file_list = "settings/ps_trace_file_list.txt";
  my $pstrace_db_path = "database/pstrace_db";
  my $ps_trace_h_catcher_path = "database_win32/ps_trace.h";
  my $catcher_ps_db_path = "../../database/BTCatacherDB";
  my $catcher_db_folder = "database_win32";
  my $Preprocessor_path;
  my $optionPath = "";
  my $compile_options = "";
  my $pc_cnf = "database/Pc_Cnf";
  my $tgt_cnf;
  my $version = "1.0";
  my $verno = "W0949";
  my $CodegenCompiler;
  
  #del the existed db files
  if (-e "$catcher_ps_db_path")
  {
  	print("$catcher_ps_db_path exist\n");
 		unlink $catcher_ps_db_path;
	}
  
  if (!defined($IS_AUTOGEN) and $#ARGV != 0)
  {
    print "version = $version\n";
    print "Usage: BTCodegen.pl [mode]\n";
    print "       mode == arm or vc\n";
    print "       E.g: BTCodegen.pl vc (Generate Catcher database for VC compilers)\n";
    exit(1);  
  }

  if(defined($IS_AUTOGEN))
  {
  	$CodegenCompiler = $CGEN_COMPILER;
  	$cgen = "$CGEN_PATH";
  	$compile_options = $GLOBAL_OPTIONS;
  	#$optionPath = "$BT_CODEGEN_PATH/";
	  #prefix working path in all path variables
	  #$dbfolder = $BT_CODEGEN_PATH."/".$dbfolder;
	  #$parsedb = $BT_CODEGEN_PATH."/".$parsedb;
	  #$pridb = $BT_CODEGEN_PATH."/".$pridb;
    #$ps_trace_file_list = $BT_CODEGEN_PATH."/".$ps_trace_file_list;
    #$pstrace_db_path = $BT_CODEGEN_PATH."/".$pstrace_db_path;
    #$ps_trace_h_catcher_path = $BT_CODEGEN_PATH."/".$ps_trace_h_catcher_path;
    #$catcher_ps_db_path = $BT_CODEGEN_PATH."/".$catcher_ps_db_path;
		#$catcher_db_folder = $BT_CODEGEN_PATH."/".$catcher_db_folder;
 	}
 	else
  {
  	$CodegenCompiler = $ARGV[0];
  	$CodegenCompiler =~ tr/[A-Z]/[a-z]/;
  	$cgen = "tools/Cgen";
  	die "$cgen does NOT exist!\n" if (!-e $cgen);
	}

  if ($CodegenCompiler eq "arm")
  {
    $optionPath = "Settings/ARMCC_Options.ini"; #Store the compile option
    $Preprocessor_path = "armcc";
    $parse_base_db_cmd = "$Preprocessor_path -E -DGEN_FOR_CPARSER -DGEN_FOR_PC $compile_options --via $optionPath -o $pridb $parsedb";      
    $tgt_cnf = "database/Tgt_Cnf";
  }
  elsif ($CodegenCompiler eq "gcc")
  {
    $optionPath = "settings/GCC_Options.ini"; #Store the compile option
    $Preprocessor_path = "gcc";
    #parsing options
    open OPTIONFILE, "<", "$optionPath" or die $!;
    my @gcc_options;
    while(<OPTIONFILE>)
    {
			$_ =~ s/^\s+//; #remove leading spaces
			$_ =~ s/\s+$//; #remove trailing spaces
			if($_ ne '')
			{
         push(@gcc_options, $_);
			}
    }
    close(OPTIONFILE);
    $compile_options .= join(" ", @gcc_options);

    $parse_base_db_cmd = "$Preprocessor_path -E -DGEN_FOR_CPARSER -DGEN_FOR_PC $compile_options -o $pridb $parsedb";
    $tgt_cnf = "database/Tgt_Cnf";
  }
  elsif ($CodegenCompiler eq "vc")
  {
    #register VC path
    registerVCPath(); 
    
    my $is_cl = system("cl > nul 2>&1");        
    die "Could NOT execute \"cl\"!\nPlease help to add into PATH environment!\n" if ($is_cl != 0);   
    
    #set the option file list
    $optionPath = "Settings/VC_Options.ini"; #Store the compile option
    $Preprocessor_path = "cl.exe";
    $parse_base_db_cmd = "$Preprocessor_path -E -DGEN_FOR_CPARSER -DGEN_FOR_PC $compile_options \@$optionPath $parsedb > $pridb";
    $tgt_cnf = "database/Pc_Cnf";
  }
  else
  {
    print "Unsupported mode: $CodegenCompiler\n";
    exit(1);
  }

	if(defined($IS_AUTOGEN))
	{
		$pc_cnf = $CGEN_PC_CNF_PATH;
		$tgt_cnf = $CGEN_TGT_CNF_PATH;
	}

  system("mkdir $catcher_db_folder") if (!-d $catcher_db_folder);
  system("mkdir $dbfolder") if (!-d $dbfolder);
  system("mkdir $dbfolder/msglog_db") if (!-d "$dbfolder/msglog_db");
  
  #Preprocess the C header files
  print "$parse_base_db_cmd\n";
  $res = system("$parse_base_db_cmd");
  die "$parse_base_db_cmd failed ==> $res\n" if ($res != 0);  
  
  #Generate the baseDatabase  
  $res = system("$cgen -c $pridb $tgt_cnf $pc_cnf $dbfolder/BPGUInfo $dbfolder/enumFile MoDIS $verno");
  print "$cgen -c $pridb $tgt_cnf $pc_cnf $dbfolder/BPGUInfo $dbfolder/enumFile MoDIS $verno\n";
  die "$cgen -c $pridb $tgt_cnf $pc_cnf $dbfolder/BPGUInfo $dbfolder/enumFile MoDIS $verno ==> $res\n" if ($res != 0);

  #read the file list
  my @TraceListAry;
  my $i;
  parsePSTraceList($ps_trace_file_list,\@TraceListAry);
  
  if ($#TraceListAry < 0)
  {
    die "There is not trace files\n";       
  }
  
  foreach $trc (@TraceListAry) {
    next if (($trc eq "/") || ($trc eq ""));
    #print "Check \"$trc\"\n";
    #sleep 1;
    next if ($trc !~ /\/(\w+)\.h/);
    next if ($1 =~ /^l1/i);    
    #sleep 1;
    if ($CodegenCompiler eq "arm")
    {
      $res = system("$Preprocessor_path -E -DGEN_FOR_PC --via $optionPath -o $pstrace_db_path/${1}.ptr $trc");
      die "$Preprocessor_path -E -DGEN_FOR_PC --via $optionPath -o $pstrace_db_path/${1}.ptr $trc failed ==> $res\n" if ($res != 0);
    }
    if ($CodegenCompiler eq "gcc")
    {
      print "$Preprocessor_path -E -DGEN_FOR_PC $compile_options -o $pstrace_db_path/${1}.ptr $trc\n";
      $res = system("$Preprocessor_path -E -DGEN_FOR_PC $compile_options -o $pstrace_db_path/${1}.ptr $trc");
      die "$Preprocessor_path -E -DGEN_FOR_PC $compile_options -o $pstrace_db_path/${1}.ptr $trc failed ==> $res\n" if ($res != 0);
    }
    elsif ($CodegenCompiler eq "vc")
    {
      print "$Preprocessor_path -E \@$optionPath -DGEN_FOR_PC $trc > $pstrace_db_path\\${1}.ptr\n";
      $res = system("$Preprocessor_path -E \@$optionPath -DGEN_FOR_PC $trc > $pstrace_db_path\\${1}.ptr");
      die "$Preprocessor_path -E \@$optionPath -DGEN_FOR_PC $trc > $pstrace_db_path\\${1}.ptr failed ==> $res\n" if ($res != 0);    
    }
  }  

  #generate PS trace database
  print "$cgen -ps $catcher_ps_db_path $dbfolder/BPGUInfo $pstrace_db_path $ps_trace_h_catcher_path\n";
  $res = system("$cgen -ps $catcher_ps_db_path $dbfolder/BPGUInfo $pstrace_db_path $ps_trace_h_catcher_path");  
  die "$cgen -ps $catcher_ps_db_path $dbfolder/BPGUInfo $pstrace_db_path $ps_trace_h_catcher_path failed ==> $res\n" if ($res != 0);
}

sub registerVCPath
{
  $cl = 'cl';
  $is_msdev = 0;
  $is_cl = 0;
  $VSCommonDir = "C:\\Program Files\\Microsoft Visual Studio\\Common";
  $MSDevDir = "C:\\Program Files\\Microsoft Visual Studio\\Common\\MSDev98";
  $MSVCDir = "C:\\Program Files\\Microsoft Visual Studio\\VC98";
  $VcOsDir = "WIN95";
  if ((-d $VSCommonDir) && (-d $MSDevDir) && (-d $MSVCDir)) {
    if ($ENV{"OS"} eq "Windows_NT") {
      $VcOsDir = "WINNT";
      $ENV{"PATH"} = "${MSDevDir}\\BIN;${MSVCDir}\\BIN;${VSCommonDir}\\TOOLS\\${VcOsDir};${VSCommonDir}\\TOOLS;" . $ENV{"PATH"};
    } else {
      $ENV{"PATH"} = "${MSDevDir}\\BIN;${MSVCDir}\\BIN;${VSCommonDir}\\TOOLS\\${VcOsDir};${VSCommonDir}\\TOOLS;" .$ENV{"windir"}. "\\SYSTEM;" . $ENV{"PATH"};
    }
    $ENV{"INCLUDE"} = "${MSVCDir}\\ATL\\INCLUDE;${MSVCDir}\\INCLUDE;${MSVCDir}\\MFC\\INCLUDE;". $ENV{"INCLUDE"};
    $ENV{"LIB"} = "${MSVCDir}\\LIB;${MSVCDir}\\MFC\\LIB;". $ENV{"LIB"};
  } else {
    print "$VSCommonDir does NOT exist!\n" if (!-d $VSCommonDir);
    print "$MSDevDir does NOT exist!\n" if (!-d $MSDevDir);
    print "$MSVCDir does NOT exist!\n" if (!-d $MSVCDir);
  }
  $is_msdev = system("$msdev /? > nul 2>&1");
  $is_cl = system("$cl > nul 2>&1");  
    
}

#This function is intended to parse the TMD List file and convert them into a array
sub parsePSTraceList
{ #input parameters: $_[0]: input file, $_[1]: TraceListAry
  my $preFixStr = "";
  
  my $inputFileName = $_[0];
  local(*TraceListAry) = $_[1];
  
  #open File to read
  open(inFHD, "< $inputFileName ") || die "$inputFileName $!\n";

  while (my $line=<inFHD>) {

    chomp($line);
    #print "$line\n";

    #check if this line starts with #            
    my $pos1 = index($line, "#");    
    my $content = $line;
    
    if ($pos1 != -1)
    {
      $content = substr $line, 0, $pos1;      
    }       
    
    #remove all the non-iteral [^a-zA-Z0-9_]
    $content =~ s/\s//g;	
    
    #print "$content X\n";
    
    if ($content ne "")
    {
      push @TraceListAry, $preFixStr.$content;
    }
        
  }
  
  #close File
  close(inFHD);
    
}

1;
