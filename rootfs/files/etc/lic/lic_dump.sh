#!/bin/bash

if [ ! $# -eq 1 ] ; then
echo "Usage: lic_dump.sh LICTYPE"
echo "	LICTYPE 0 to dump all types of  license."
exit 1;
fi

LICTYPE=$1

dump_one_lic()
{
LICT=$1
if [ ${LICT} -eq 1 ] ; then
LICTS=''
else
LICTS=$LICT
fi

if [ -r /mnt/lic/maxwtpcount${LICTS}.lic ] ; then
	echo "Type ${LICT}: "`cat /mnt/lic/maxwtpcount${LICTS}.lic`
	return 0;
else
	return 1;
fi
}

echo "Currently Installed license:"
if [ ${LICTYPE} -eq 0 ] ; then
	for i in 1 2 3 4 5 6
	do
	dump_one_lic $i
	done
else
	dump_one_lic ${LICTYPE}
fi
echo "End of installed license."

exit 0
