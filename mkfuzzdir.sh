# need to make the dirs below before run `run.sh`.

if [ ! -d ./bin/ ]; then
    mkdir ./bin/
fi

if [ ! -d /tmp/.fuzzlab/ ]; then
    mkdir /tmp/.fuzzlab/
fi

if [ ! -d /tmp/.fuzzlab/bin/ ]; then
    mkdir /tmp/.fuzzlab/bin/
fi

if [ ! -d /tmp/.fuzzlab/testcases/ ]; then
    mkdir /tmp/.fuzzlab/testcases/
fi

