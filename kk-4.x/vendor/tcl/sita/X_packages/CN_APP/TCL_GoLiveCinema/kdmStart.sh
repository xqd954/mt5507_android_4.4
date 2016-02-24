#!/system/bin/sh
#** kdmStart
#** Change by heguijiss@163.com @ 2015/07/02.

shadow=/data/kdmServer
source=/system/bin/kdmServer
u_path=/data/kdmConfig/upgrade

upgrade()
{
	echo "Upgrade kdmServer"
	if [ -f $u_path/update.sh ];
	then
		chmod 0755 $u_path/update.sh
		sh $u_path/update.sh
		rm -r $u_path
	fi
}

ready()
{
	if [ -f $shadow ];
	then
		rm -f $shadow
	fi

	echo "Ready kdmServer"
	if [ -f $source ];
	then
		cp $source $shadow
		chmod 0755 $shadow
	else
		echo "$n_path/$source not exist!"
	fi
}

daemon()
{
	while true
	do
		upgrade
		ready
		sleep 0.3
		$shadow
	done
}

daemon

#~End~