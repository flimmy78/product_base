#!/bin/sh
#
###########################################################################
#
#              Copyright (C) Autelan Technology
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

TEMP=/var/run/apache2/dns.tmp

if [ $# -eq 2 ];then      
     if ! test -f $1; then
           sudo touch $1 
           sudo chmod 666 $1    
           echo "nameserver $2" > $1 
           exit 1
     else
     sudo chmod 666 $1      
     num=` cat $1 |grep nameserver |wc -l `        
        if [ $num -lt 3 ];then
            if [ $num -eq 0 ];then   #���ж��Ƿ�С������С���������ж��Ƿ�Ϊ�㣬
            echo "nameserver $2" > $1   #�������Ļ�������ӵ�һ��
            else
            sudo touch $TEMP
            sudo chmod 666 $TEMP
            cat $1 |grep nameserver > $TEMP
            sed "1s/.*/nameserver $2/" $TEMP > $1   #���������Ļ����޸ĵ�һ�е�������
            sudo rm $TEMP            
            exit 0
          fi
        else
        exit 2
        fi     
     fi
else
        exit 4  
fi
