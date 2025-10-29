#!/bin/sh

# Test whether the POSIX locale has encoding errors.
LC_ALL=C \
${CHECKER} ./test-mbrtoc32${EXEEXT} 1 || exit 1
LC_ALL=POSIX \
${CHECKER} ./test-mbrtoc32${EXEEXT} 1 || exit 1

exit 0
