# DO NOT EDIT
# This makefile makes sure all linkable targets are
# up-to-date with anything they link to
default:
	echo "Do not invoke directly"

# Rules to remove targets that are older than anything to which they
# link.  This forces Xcode to relink the targets from scratch.  It
# does not seem to check these dependencies itself.
PostBuild.tropicaladd_tilde.Debug:
/Users/ilarialemmo/Documents/Max\ 8/Packages/max-sdk/externals/tropicaladd~.mxo/Contents/MacOS/tropicaladd~:
	/bin/rm -f /Users/ilarialemmo/Documents/Max\ 8/Packages/max-sdk/externals/tropicaladd~.mxo/Contents/MacOS/tropicaladd~


PostBuild.tropicaladd_tilde.Release:
/Users/ilarialemmo/Documents/Max\ 8/Packages/max-sdk/externals/tropicaladd~.mxo/Contents/MacOS/tropicaladd~:
	/bin/rm -f /Users/ilarialemmo/Documents/Max\ 8/Packages/max-sdk/externals/tropicaladd~.mxo/Contents/MacOS/tropicaladd~


PostBuild.tropicaladd_tilde.MinSizeRel:
/Users/ilarialemmo/Documents/Max\ 8/Packages/max-sdk/externals/MinSizeRel/tropicaladd~.mxo/Contents/MacOS/tropicaladd~:
	/bin/rm -f /Users/ilarialemmo/Documents/Max\ 8/Packages/max-sdk/externals/MinSizeRel/tropicaladd~.mxo/Contents/MacOS/tropicaladd~


PostBuild.tropicaladd_tilde.RelWithDebInfo:
/Users/ilarialemmo/Documents/Max\ 8/Packages/max-sdk/externals/RelWithDebInfo/tropicaladd~.mxo/Contents/MacOS/tropicaladd~:
	/bin/rm -f /Users/ilarialemmo/Documents/Max\ 8/Packages/max-sdk/externals/RelWithDebInfo/tropicaladd~.mxo/Contents/MacOS/tropicaladd~




# For each target create a dummy ruleso the target does not have to exist
