#!/bin/bash
./sigusr_recv 
$RECVPID=`ps -A | grep sigusr_recv | awk '{printf $1}'`
echo $RECVPID
