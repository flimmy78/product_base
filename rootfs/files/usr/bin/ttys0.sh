#!/bin/sh
#opyright (C) Autelan Technology
#
#This software file is owned and distributed by Autelan Technology 
#
############################################################################
#THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND 
#ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED 
#WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE 
#DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR 
#ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES 
#(INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; 
#LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON 
#ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT 
#(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS 
#SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
##############################################################################
#
# eag_init
#
# CREATOR:
# autelan.software.xxx. team
# 
# DESCRIPTION: 
#     
#############################################################################

TTYS0PWDFILE="/etc/ttyS0pwd"
TTYS0USRFILE="/etc/ttyS0usr"

echo -e  "\n\nPress ENTER key to connect via ttyS0.\n"
read
if [ -e $TTYS0PWDFILE ] ; then

	if [ -e $TTYS0USRFILE ] ; then
		printf "Login:"
		read username
		iusername=$(cat $TTYS0USRFILE )
		if [ "$username" != "$iusername" ] ; then
	  		sleep 3
			echo -e "\nWrong console Username."
			exit -1
		fi
	fi
        printf "Password:"
        stty -echo
        read passwd1
        stty echo
        ipasswd=$(echo $passwd1 | openssl md5)
	opasswd=$(cat "$TTYS0PWDFILE")
       	if [ "$ipasswd" != "$opasswd" ] ; then
               	sleep 3 
                echo -e "\nWrong console password."
                exit -1
        fi 


fi
cat /etc/passwd | grep ^admin > /dev/null
if [ $? -eq 0 ] ; then
	su admin --shell /opt/bin/vtysh --login
else
	sh
fi



