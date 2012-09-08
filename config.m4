PHP_ARG_ENABLE(ipcalc, whether to enable ipcalc,
[  --enable-ipcalc   Enable ipcalc support])

if test "$PHP_IPCALC" = "yes"; then
  AC_DEFINE(HAVE_IPCALC, 1, [Whether you have ipcalc])
  PHP_NEW_EXTENSION(ipcalc, ipcalc.c, $ext_shared)
fi
