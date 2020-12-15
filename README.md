# `els`

This is a simple CLI tool to view the files in a given directory. This tool is not even close to fully featured, but it works well enough as an at-a-glance tool.

	- Recursive option (to print file trees)
	- Prints symlinks
	- Color support for ANSI terminals
	- Selective file/folder exclusion
	- *`els -h` for complete options*

`els` also supports selectively ignoring certain files from being listed. In this way, it's good for at-a-glance printouts where you want to see hidden files but not necessarily your `.git` directory. You can set the files/folders which need to be excluded in `~/.els.excludes`.

### Building

This project will only build on Linux as far as I know.

```
cd build && cmake ..
make
```

The executable will be generated in a file called `els`.
