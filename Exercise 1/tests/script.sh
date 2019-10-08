#!/bin/bash

#
#   brief: Print in terminal and/or external file the error messages based on user input flags
#
error(){
    # Print error messages in terminal if quiet mode is not enabled
    if [ -f ${QUIET} ] ; then echo -n $@ ; fi ;
    # Print error messages in file if it is given
    if [ "${ERRORS_FILE}" != "" ] ; then echo -n $@ >> ${ERRORS_FILE} ; fi ;
}


#
#   brief: Check if input file exists and if it is valid
#          (has correct syntax)
#
#   param: $1 -> The name of the file (actually its path)
# 
#   return: The content of the file and status code:
#           0 if it is OK,
#           10 if file is empty,
#           11 if something is wrong with the format of the file at any point,
#           12 if file is too big,
#           13 if there is not read permission,
#           14 if file does not exist
#
check(){
    if [ ${DEBUG} ] ; then echo -n "\tFunction: check(), --START--" ; fi ;

    local result=""
    local inputFile=$1

    #File exists, is readable and is not empty
    if [ -f ${inputFile} ] && [ -r ${inputFile}] && [ -s ${inputFile}]; then
        echo "File OK" #TODO format check

    elif [ !-f ${inputFile} ] ; then
        echo "Function: check(), "
    fi

    echo ${result}
    return 0
}