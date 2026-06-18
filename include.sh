#!/usr/bin/env bash
MOD_TWISTED_ROOT="$( cd "$( dirname "${BASH_SOURCE[0]}" )/" && pwd )"

source $MOD_TWISTED_ROOT"/conf/conf.sh.dist"

if [ -f $MOD_TWISTED_ROOT"/conf/conf.sh" ]; then
    source $MOD_TWISTED_ROOT"/conf/conf.sh"
fi
