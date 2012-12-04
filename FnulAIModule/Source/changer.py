# This script will parse the current directory recursively and find any .h and .cpp files.
# In any file it finds, it will look for #include "anyfile.h" strings, find the file referenced and replace
# the whole #include statement with #include <Path/From/Current/Directory/anyfile.h>

# This will not work if there are files with the same name in the hierarchy. Then the first file w/ a given name in a depth-first search will be used.
import re
import os
import os.path
import string

# Returns the full path to a specific file or none if no such file
def FindFile(filename):
	for root, dirs, files in os.walk("."):
		for f in files:
			if f == filename:
				return os.path.join(root, f)
	return None

# Find #include directives and manipulate them
def ChangeFile(filename, root):
	file = open(filename, "r+")
	lines = file.readlines()
	file.close()
	
	# Find include directives and rewrite them
	newlines = []
	for line in lines:
		if line.startswith('#include "'):
			lastquote = string.rfind(line, '"')
			includename = line[10 : lastquote]
			
			print lastquote
			
			fullpath = FindFile(includename)
			if (fullpath == None):
				line = line[:-1] + " // Relative path not found\n"
				print "Relative path not found in file", filename
			else:
				line = "#include <%s>\n" % fullpath[2:]
		newlines.append(line)
		
	# Write the new, modified, lines back
	file = open(filename, "w+")
	file.writelines(newlines)
	file.close()

# Traverse the current directory recursively
for root, dirs, files in os.walk("."):
	print "Directory", root
	print "Directories: ", dirs
	print "Files: ", files
	print "\n"
	
	for filename in files:
		if filename.endswith(".cpp") or filename.endswith(".h"):
			# parse file
			filename = os.path.join(root, filename)
			print filename
			
			ChangeFile(filename, root)
			
	print "\n"

