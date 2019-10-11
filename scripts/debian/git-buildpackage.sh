#!/bin/bash


GIT_BRANCH=$BRANCH_NAME
CODENAME=`lsb_release -da  | grep Codename |  awk  '{print $2}'`
DISTRIB=`lsb_release -da  | grep Distributor |  awk  '{print $3}'`

if [ "$GIT_BRANCH" == "develop" ]; then

	if [ "$CODENAME" == "stretch" ]; then
		CODENAME="unstable"
	elif [ "$CODENAME" == "bionic" ]; then
		  CODENAME="bionic-backports"
	fi  
  
elif [ "$GIT_BRANCH" == "master" ]; then

	if [ "$CODENAME" == "stretch" ]; then
		CODENAME="stable"
	fi

        if [ "$CODENAME" == "bionic" ]; then
            # We just want to stable LLA package.
            CODENAME="stable"
        fi
fi

echo "GIT_BRANCH:"${BRANCH_NAME}
echo "GIT_COMMIT:"${GIT_COMMIT}
echo "CODENAME:"${CODENAME}
echo "DISTRIB:"${DISTRIB}

#Remove it when docker will be recreate
sudo apt-get update
sudo apt-get install -y curl dpkg-sig

#install liblogicalaccess
echo "deb http://debian.islog.com/ ${CODENAME} main" | sudo tee -a /etc/apt/sources.list.d/liblogicalaccess.list
sudo curl http://debian.islog.com/debian-islog-com-keyring.key | sudo apt-key add -
sudo apt-get update
sudo apt-get install -y liblogicalaccess liblogicalaccess-dev liblogicalaccess-dbg libnfc-dev libnfc-bin


cd /home/jenkins
git clone https://github.com/islog/liblogicalaccess-libnfc-debian.git
cd liblogicalaccess-libnfc-debian

git checkout -b upstream
git remote add liblogicalaccess-libnfc https://github.com/islog/liblogicalaccess-libnfc
git pull --no-edit -X theirs  liblogicalaccess-libnfc ${GIT_BRANCH}

VERSION=`cat pom.xml  | grep "<version>" -m1 | cut -d">" -f2 | cut -d"<" -f 1 |  cut -d"-" -f 1`.`date '+%Y%m%d'`
VERSION_DEBIAN=$VERSION~${CODENAME}
echo "VERSION=$VERSION"
echo "VERSION_DEBIAN=$VERSION_DEBIAN"

git tag upstream/$VERSION -f

git checkout master
git rebase upstream


#check if master and already have the hash dont push otherwise -> do
DEBFULLNAME=ISLOG DEBEMAIL=support@islog.com  debchange -v $VERSION_DEBIAN --distribution $CODENAME "${GIT_BRANCH}-${GIT_COMMIT}"

export DEB_BUILD_OPTIONS="parallel=3"
gbp buildpackage --git-ignore-new --git-upstream-tree=upstream/$VERSION
