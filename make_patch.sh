#/bin/sh
patch_file=$1
prikey_file=$2
passwd_file=$3
passwd=$4
#patch_dir=${1%.*}
patch_file_name=${patch_file##*/}
prikey_file_name=${prikey_file##*/}
passwd_file_name=${passwd_file##*/}
patch_dir=${patch_file_name%.*}

#echo "patch_file is $patch_file"
#echo "prikey_file is $prikey_file"
#echo "passwd_file is $passwd_file"
#echo "patch_file_name is $patch_file_name"
#echo "prikey_file_name is $prikey_file_name"
#echo "passwd_file_name is $passwd_file_name"
#echo "passwd is $passwd"

if [ ! $patch_file ] || [ ! -f $patch_file ] ; then
	echo "patch_file is NULL or patch file not here"
	exit -2
fi
if [ ! $prikey_file ] || [ ! -f $prikey_file ] ; then
	echo "prikey is NULL or prikey not here"
	exit -2
fi
if [ ! $passwd_file ] || [ ! -f $passwd_file ] ; then
	echo "passwd_file is NULL or passwd_file is not here"
	exit -2
fi
if [ ! $passwd ] ; then
	echo "passwd is NULL"
	exit -2
fi
	
#if [ ! -f ./passwd ] ; then
#	echo "passwd is not here"
#fi


#tar -xvjf $patch_file
#if [ $? != 0 ] ; then
#	echo "tar error"
#	exit -3
#fi
mkdir $patch_dir
if [ $? -nt 0 ] ; then
	exit -1
fi
cp $prikey_file $patch_dir

cp $patch_file $patch_dir

cp $passwd_file $patch_dir

#rm $patch_file

pushd $patch_dir

echo "$passwd" > passwd_

#temp_patch_file=$patch_file + _sec

openssl enc -a -des3 -in $patch_file_name -kfile passwd_ -out temp_patch_file

if [ $? -ne 0 ] ; then
	echo "error[1] exit"
	exit -1
fi

mv temp_patch_file $patch_file 
#openssl rsautl -encrypt -pubin -inkey $pubkey -in install -out install_  

openssl rsautl -sign -in passwd_ -inkey $prikey_file_name -passin file:$passwd_file_name > passwd_sec

if [ $? -ne 0 ] ; then
	echo "error[2] exit"
	exit -1
fi

rm $prikey_file_name
rm passwd_
rm $passwd_file_name

popd
pwd

tar -cvjf "${patch_file}s" $patch_dir

