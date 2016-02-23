#
# Copyright (C) 2011 The Android Open Source Project
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
#

# This file is executed by build/envsetup.sh, and can use anything
# defined in envsetup.sh.
#
# In particular, you can add lunch options with the add_lunch_combo
# function: add_lunch_combo generic-eng


MTK_IC_LIST='mt5880 mt5398 mt5399'
MYBRAND='mtk'
MYODM='mtk'

function android_add_ic() {
	local ic="${IC_SETTING}"
	local variant=${BUILD_VARIANT}

	if [ "x${ic}" = "x" ] ; then
	    ic=mt5399
	fi
	if [ "${variant}" = '' ]; then
		variant=userdebug
	fi

	if [ ! -e device/${MYBRAND}/${MYODM}/${ic} ] ; then 
	    (cd device/${MYBRAND}/${MYODM}/ && ln -s basic ${ic}) 
	fi 

	echo add_lunch_combo ${MYBRAND}_${ic}-${variant}
	add_lunch_combo ${MYBRAND}_${ic}-${variant}


}

if [ "${IC_SETTING}" == '' ]; then
	for my_i in ${MTK_IC_LIST}; do
		IC_SETTING=$my_i android_add_ic
	done
else

if [ "$BRAND" == "$MYBRAND" ]; then
	android_add_ic
fi
fi

