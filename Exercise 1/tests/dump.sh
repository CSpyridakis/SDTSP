#!/bin/bash

        #TODO
        #GNU-style long options support
        # -)
        #     case "${OPTARG}" in 
        #         # check)      CHECK_FILE= ; exit $(check ${CHECK_FILE})               ;;  #Check input file
        #         debug)      DEBUG=${TRUE}               ;;  #Debug mode on
        #         error)      ERRORS_FILE=${OPTARG#*=}    ;;  #File for error messages
        #         help)       usage ; exit 0              ;;  #Help menu
        #         input)      INPUT_FILE=${OPTARG#*=}     ;;  #Input file
        #         ouput)      OUTPUT_FILE=${OPTARG#*=}    ;;  #Output file
        #         quiet)      QUIET=${TRUE}               ;;  #Quiet mode
        #         \?)         echo "ddddddddd" ; exit 0              ;;  #Unknown
        #         *)          echo "wwwwwwww" ; exit 0              ;;  #Unknown
        #     esac
        # ;;

declare -a inputFiles
declare -a outputFiles
declare -a errorFiles