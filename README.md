# `els`

This is a simple CLI tool to view the files in a given directory. This tool is not even close to fully featured, but it works well enough as an at-a-glance tool.

	- Recursive option (to print file trees)
	- Prints symlinks
	- Color support for ANSI terminals
	- Selective file/folder exclusion

`els` also supports selectively ignoring certain files from being listed. In this way, it's good for at-a-glance printouts where you want to see hidden files but not necessarily your `.git` directory. You can set the files/folders which need to be excluded in `~/.els.excludes`.
