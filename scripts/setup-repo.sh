#!/bin/bash

YOCTO_DIR=${1?"Usage: ./scripts/setup-repo.sh [YOCTO_DIR]"}

# Save teledyne directory location
DEVDIR=$PWD

# Create Yocto directory in case it does not exist
mkdir -p $YOCTO_DIR
cd $YOCTO_DIR

# Create local auxiliar repository to use repo tool
mkdir -p manifest && \
    cp ${DEVDIR}/teledyne-manifest.xml manifest && \
    cd manifest && \
    git init && \
    git add teledyne-manifest.xml && \
    git config user.name "teledyne" && \
    git config user.email "<>" && \
    git commit -m "teledyne local manifest"

# Install repo tool if is not already installed
if ! [ -x "$(command -v repo)" ]
then
    echo "Installing repo"
    mkdir -p ~/.bin && \
        PATH="${HOME}/.bin:${PATH}" && \
        curl https://storage.googleapis.com/git-repo-downloads/repo-1 > ~/.bin/repo && \
        chmod a+x ~/.bin/repo
else
    echo "Using existent installation of repo"
fi

# Check for Git user and email and create a temporal one if not set
need_clean=no
if [ -f ~/.gitconfig ]
then
    echo "Git configuration file found"
    ID_EXIST=`grep '\[user\]' ~/.gitconfig`
fi
if [ "x$ID_EXIST" == "x" ]
then
    echo "Creating temporal Git ID"
    echo -e "[user] \n\temail = teledyne\n\tname = teledyne" >> ~/.gitconfig
    need_clean=yes
else
    echo "Using existent Git ID settings"
fi

# Download required repos
cd $YOCTO_DIR
python3 ~/.bin/repo init -u manifest -m teledyne-manifest.xml && \
repo sync
# Create symlink to meta-teledyne to update YOCTO_DIR when updating repo
ln -s $DEVDIR/meta-teledyne $YOCTO_DIR/meta-teledyne

# Delete local auxiliar repository
rm -rf manifest

# Clean repo user ID settings
if [ $need_clean == "yes" ]
then
    gawk -i inplace '!/user/' ~/.gitconfig
    gawk -i inplace '!/name =/' ~/.gitconfig
    gawk -i inplace '!/email =/' ~/.gitconfig
fi
