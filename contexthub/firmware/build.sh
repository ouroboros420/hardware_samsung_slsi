#!/bin/bash

if [[ "$CROSS_COMPILE" == "" ]]
then
  source ./toolchain-setup.sh
fi

VARIANT_BASE="variant"
OUTPUT_BASE="out/nanohub"
SENSOR_CONF_FILE="sensor_conf"

#COMMIT_LOG=$(git log -5 --abbrev=8 --pretty=format:"%h %<(30,trunc)%s" | tr '\n' ';' | tr '"' ' ')
COMMIT_LOG=$(git log -5 --abbrev=8 --pretty=format:"%h %s" | cut -c -39 | tr '\n' ';' | tr '"' ' ')
BUILDER=$(whoami | cut -c -18)
ret=$?

echo "###############################"
echo NANOHUB_FIRMWARE_PATH=$NANOHUB_FIRMWARE_PATH
echo "###############################"
if [[ "$NANOHUB_FIRMWARE_PATH" != "" && ! -d "$NANOHUB_FIRMWARE_PATH" ]]
then
  echo
  echo  "  ERROR : $NANOHUB_FIRMWARE_PATH does not exist"
  echo
  exit
fi

if [ $ret -ne 0 ]
then
  COMMIT_LOG="THERE_IS_NO_COMMIT_INFORMATION"
fi

if [ $# -eq 0 ]
then
  echo
  echo "   usage : $0 variant_name <option>"
  echo "   option : all / clean / user defined options"
  echo "   ex :"
  echo "       $0 smdk9610"
  echo "       $0 smdk9610 all"
  echo "       $0 smdk9610 clean"
  echo "       $0 smdk9610 ACC=BMI160"
  echo "       $0 smdk9610 1,3,5 ALS=RPR0521"
  exit
fi

VARIANT=$1

if [ ! -d "$VARIANT_BASE/$VARIANT" ]
then
  echo
  echo  "  ERROR : $VARIANT_BASE/$VARIANT does not exist"
  echo
  exit
fi

if [ -e $VARIANT_BASE/$VARIANT/$SENSOR_CONF_FILE ]
then
    source $VARIANT_BASE/$VARIANT/$SENSOR_CONF_FILE

    MULTI_COMB_SENSOR_COUNT=${#MULTI_COMB_OPTION_NAME[@]}
    MULTI_COMB_OPTION_COUNT=${#MULTI_COMB_OPTION[@]}
    let "MULTI_COMB_COUNT=$MULTI_COMB_OPTION_COUNT/$MULTI_COMB_SENSOR_COUNT"
fi

if [ $# -eq 1 ]
then
  echo ===========================================================
  echo make -C $VARIANT_BASE/$VARIANT $OTHER_OPTIONS
  echo ===========================================================
  make -C $VARIANT_BASE/$VARIANT $OTHER_OPTIONS
  if [ "$NANOHUB_FIRMWARE_PATH" != "" ]; then
    cp $OUTPUT_BASE/$VARIANT/os.checked.bin $NANOHUB_FIRMWARE_PATH/
    cp $OUTPUT_BASE/$VARIANT/os.checked.elf $NANOHUB_FIRMWARE_PATH/
    cp $OUTPUT_BASE/$VARIANT/os.sym.bin $NANOHUB_FIRMWARE_PATH/
    cp $OUTPUT_BASE/$VARIANT/bl.unchecked.bin $NANOHUB_FIRMWARE_PATH/
    cp $OUTPUT_BASE/$VARIANT/bl.unchecked.elf $NANOHUB_FIRMWARE_PATH/
  fi
  exit
fi

if [ "$2" == "clean" ]
then
    make -C $VARIANT_BASE/$VARIANT clean
    exit
fi

if [ "$2" == "all" ]
then
  echo "USE_MULTI_SENSOR_COMBINATION = $USE_MULTI_SENSOR_COMBINATION"
    if [[ $USE_MULTI_SENSOR_COMBINATION -eq 1 ]]
    then
        for ((i = 0 ; i < $MULTI_COMB_COUNT ; i++))
        do
            PARAM=""
            for ((j = 0 ; j < $MULTI_COMB_SENSOR_COUNT ; j++))
            do
                let "OFFSET=$j * $MULTI_COMB_COUNT"
                PARAM="$PARAM ${MULTI_COMB_OPTION_NAME[j]}=${MULTI_COMB_OPTION[i+$OFFSET]}";
            done

            for ((j = 3 ; j <= $# ; j++))
            do
                PARAM="$PARAM ${!j}"
            done
            echo ===========================================================
            echo make -C $VARIANT_BASE/$VARIANT $PARAM $OTHER_OPTIONS
            echo ===========================================================
            if [ -e $OUTPUT_BASE/$VARIANT/os.unchecked.elf ]
            then
              echo
              rm -f $OUTPUT_BASE/$VARIANT/os.unchecked.elf
            fi
            make -C $VARIANT_BASE/$VARIANT $PARAM $OTHER_OPTIONS COMMIT_LOG="$COMMIT_LOG" BUILDER="$BUILDER"
            mv $OUTPUT_BASE/$VARIANT/os.checked.bin $OUTPUT_BASE/$VARIANT/os.checked_$i.bin
            mv $OUTPUT_BASE/$VARIANT/os.checked.elf $OUTPUT_BASE/$VARIANT/os.checked_$i.elf
            mv $OUTPUT_BASE/$VARIANT/os.sym.bin $OUTPUT_BASE/$VARIANT/os.sym_$i.bin
            if [ "$NANOHUB_FIRMWARE_PATH" != "" ]; then
              cp $OUTPUT_BASE/$VARIANT/os.checked_$i.bin $NANOHUB_FIRMWARE_PATH/
              cp $OUTPUT_BASE/$VARIANT/os.checked_$i.elf $NANOHUB_FIRMWARE_PATH/
              cp $OUTPUT_BASE/$VARIANT/os.sym_$i.bin $NANOHUB_FIRMWARE_PATH/
              cp $OUTPUT_BASE/$VARIANT/bl.unchecked.bin $NANOHUB_FIRMWARE_PATH/
              cp $OUTPUT_BASE/$VARIANT/bl.unchecked.elf $NANOHUB_FIRMWARE_PATH/
            fi
        done
        ls -l $OUTPUT_BASE/$VARIANT
    else
        for ((j = 3 ; j <= $# ; j++))
        do
            PARAM="$PARAM ${!j}"
        done
        echo ===========================================================
        echo make -C $VARIANT_BASE/$VARIANT $PARAM $OTHER_OPTIONS
        echo ===========================================================
        make -C $VARIANT_BASE/$VARIANT $PARAM $OTHER_OPTIONS COMMIT_LOG="$COMMIT_LOG" BUILDER="$BUILDER"
        if [ "$NANOHUB_FIRMWARE_PATH" != "" ]; then
          cp $OUTPUT_BASE/$VARIANT/os.checked.bin $NANOHUB_FIRMWARE_PATH/
          cp $OUTPUT_BASE/$VARIANT/os.checked.elf $NANOHUB_FIRMWARE_PATH/
          cp $OUTPUT_BASE/$VARIANT/os.sym.bin $NANOHUB_FIRMWARE_PATH/
          cp $OUTPUT_BASE/$VARIANT/bl.unchecked.bin $NANOHUB_FIRMWARE_PATH/
          cp $OUTPUT_BASE/$VARIANT/bl.unchecked.elf $NANOHUB_FIRMWARE_PATH/
        fi
        ls -l $OUTPUT_BASE/$VARIANT
    fi

    exit
fi

if [[ $USE_MULTI_SENSOR_COMBINATION -eq 1 ]]
then
    SELECT_SENSOR_CONFIG_NUM_FLAG=1

    declare -a imagenum

    temp_num=$(echo $2 | tr "," "\n");

    i=0
    for num in $temp_num
    do
            if [ "$num" -eq "$num" ] 2> /dev/null
            then
                imagenum[$i]=$num
                let "i=$i + 1"
            else
                SELECT_SENSOR_CONFIG_NUM_FLAG=0
            break
          fi
    done
fi

echo SELECT_SENSOR_CONFIG_NUM_FLAG = $SELECT_SENSOR_CONFIG_NUM_FLAG
if [[ $SELECT_SENSOR_CONFIG_NUM_FLAG -eq 1 ]]
then
    for ((i = 0 ; i < ${#imagenum[@]} ; i++))
    do
        if [ "${imagenum[$i]}" -ge "$MULTI_COMB_COUNT" ]
        then
            echo
            echo "ERROR : Sensor Combination ${imagenum[$i]} does not exist!!!"
            echo "        MAX : $MULTI_COMB_COUNT"
            echo
            continue
        fi
        PARAM=""
        for ((j = 0 ; j < $MULTI_COMB_SENSOR_COUNT ; j++))
        do
            let "OFFSET=$j * $MULTI_COMB_COUNT"
            PARAM="$PARAM ${MULTI_COMB_OPTION_NAME[$j]}=${MULTI_COMB_OPTION[${imagenum[$i]}+$OFFSET]}";
        done

        for ((j = 3 ; j <= $# ; j++))
        do
            PARAM="$PARAM ${!j}"
        done
        PARAM="$PARAM $OTHER_OPTIONS"

        echo ===========================================================
        echo make -C $VARIANT_BASE/$VARIANT $PARAM
        echo ===========================================================
        if [ -e $OUTPUT_BASE/$VARIANT/os.unchecked.elf ]
        then
          echo
          rm -f $OUTPUT_BASE/$VARIANT/os.unchecked.elf
        fi
        make -C $VARIANT_BASE/$VARIANT $PARAM COMMIT_LOG="$COMMIT_LOG" BUILDER="$BUILDER"
        mv $OUTPUT_BASE/$VARIANT/os.checked.bin $OUTPUT_BASE/$VARIANT/os.checked_${imagenum[$i]}.bin
        mv $OUTPUT_BASE/$VARIANT/os.checked.elf $OUTPUT_BASE/$VARIANT/os.checked_${imagenum[$i]}.elf
        mv $OUTPUT_BASE/$VARIANT/os.sym.bin $OUTPUT_BASE/$VARIANT/os.sym_${imagenum[$i]}.bin
        if [ "$NANOHUB_FIRMWARE_PATH" != "" ]; then
          cp $OUTPUT_BASE/$VARIANT/os.checked_${imagenum[$i]}.bin $NANOHUB_FIRMWARE_PATH/
          cp $OUTPUT_BASE/$VARIANT/os.checked_${imagenum[$i]}.elf $NANOHUB_FIRMWARE_PATH/
          cp $OUTPUT_BASE/$VARIANT/os.sym_${imagenum[$i]}.bin $NANOHUB_FIRMWARE_PATH/
          cp $OUTPUT_BASE/$VARIANT/bl.unchecked.bin $NANOHUB_FIRMWARE_PATH/
          cp $OUTPUT_BASE/$VARIANT/bl.unchecked.elf $NANOHUB_FIRMWARE_PATH/
        fi
    done
    ls -l $OUTPUT_BASE/$VARIANT
else
    for ((j = 2 ; j <= $# ; j++))
    do
        PARAM="$PARAM ${!j}"
    done
    PARAM="$PARAM $OTHER_OPTIONS"
    echo ===========================================================
    echo make -C $VARIANT_BASE/$VARIANT $PARAM
    echo ===========================================================
    if [ -e $OUTPUT_BASE/$VARIANT/os.unchecked.elf ]
    then
        echo
        rm -f $OUTPUT_BASE/$VARIANT/os.unchecked.elf
    fi
    make -C $VARIANT_BASE/$VARIANT $PARAM COMMIT_LOG="$COMMIT_LOG" BUILDER="$BUILDER"
    if [ "$NANOHUB_FIRMWARE_PATH" != "" ]; then
      cp $OUTPUT_BASE/$VARIANT/os.checked.bin $NANOHUB_FIRMWARE_PATH/
      cp $OUTPUT_BASE/$VARIANT/os.checked.elf $NANOHUB_FIRMWARE_PATH/
      cp $OUTPUT_BASE/$VARIANT/os.sym.bin $NANOHUB_FIRMWARE_PATH/
      cp $OUTPUT_BASE/$VARIANT/bl.unchecked.bin $NANOHUB_FIRMWARE_PATH/
      cp $OUTPUT_BASE/$VARIANT/bl.unchecked.elf $NANOHUB_FIRMWARE_PATH/
    fi
    ls -l $OUTPUT_BASE/$VARIANT
fi

exit
