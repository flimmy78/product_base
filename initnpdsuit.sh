#!/bin/bash

./initpublic.sh

pushd src
cvs checkout npdsuit_mv
popd
#[ -d npdsuit ] || mkdir npdsuit
#pushd npdsuit
#[ -d bm ] || cvs checkout bm
#[ -d mcpss ] || cvs checkout mcpss
#[ -d nam_ax7 ] || cvs checkout nam_ax7
#[ -d nbm_ax7 ] || cvs checkout nbm_ax7
#[ -d npd ] || cvs checkout npd
#popd
