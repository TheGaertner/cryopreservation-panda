#Arguments: $1 -> 1: build | 2: run | 3: build & run

DIRECTORY="$( cd "$( dirname "${BASH_SOURCE[0]}" )" >/dev/null 2>&1 && pwd )"


yaml() {
    python3 -c "import yaml;print(yaml.load(open('$1'))$2)"
}

HOSTNAME=$(yaml ${DIRECTORY}/config.yaml "['nuc_hostname']")
USERNAME=$(yaml ${DIRECTORY}/config.yaml "['ssh_username']")
PASSWORD=$(yaml ${DIRECTORY}/config.yaml "['ssh_password']")


if [ "$1" != "1" ]
then
	function cleanup {
	printf "\nCancel script!\n"
	sshpass -p $PASSWORD  ssh $USERNAME@$HOSTNAME ' \
		var=$(pidof ./panda-controller);\
		kill ${var};\
		pkill -f gst-launch-1.0'  
	}

	trap cleanup EXIT
fi

	


if [ "$1" = "1" ] || [ "$1" = "3" ] 
then
	sshpass -p $PASSWORD rsync -a ${DIRECTORY}/panda-controller --update --exclude="*.user" --exclude="build"  $USERNAME@$HOSTNAME:.
	sshpass -p $PASSWORD rsync ${DIRECTORY}/config.yaml --update  $USERNAME@$HOSTNAME:panda-controller  
	sshpass -p $PASSWORD  ssh $USERNAME@$HOSTNAME 'cd panda-controller; chmod u+r+x external_build.sh ; ./external_build.sh;'
fi

if [ "$1" = "2" ] || [ "$1" = "3" ] 
then
	sshpass -p $PASSWORD  ssh $USERNAME@$HOSTNAME ' \
		cd panda-controller/build; \
		echo -e "\nStart Project!!!\n"; \
		./panda-controller ; \
		echo -e "\nProject Finished!!!\n"'
fi
	


