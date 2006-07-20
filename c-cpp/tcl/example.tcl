# Example tcl script

# set mytclvar for executable
set mytclvar "what else should i put in these things\n"

# execute mycout
mycout "This is Tcl Calling mycout ();"

proc mytclproc {args} {
	mycout "This is from mytclproc(): " $args
	return 1
}

mycout $mycvar

# end
