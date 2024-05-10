
# Log Analysis Tool

Prerequisites :
Python 3.7 with Tkinter Package

This Directory contain script for analysis of log result.
Log Analysis Tool wil take test logs as input and display results in tree format. It will show the tests accroding to the APIs.

## How to execute:
```
python log_analysis_tool.py
```

By running the script GUI will be open.

## How to use GUI Tool:
1. When GUI is opened on top left side Menu option will come.

2. From Menu there will be 3 submenu options.
	Select Log File: For selecting single test suite log file.
	Select Log Folder: For selecting folder for multiple test suite files.
	Exit: For exiting from GUI Tool.
   Note: While selecting make sure you have only log files in selected folder and not more than one log file for one test suite.

3. The test suite tree node will be generated. User can expand and collapse the nodes.

4. By double clicking on TEST node, it will show detail description of that test on right hand side of window.

5. By right clicking on any node, menu will be open. This menu will contain three options.
	Summary: Display summary result of the node on the right hand side window.
	Expand: For expanding all nodes for all level from current expanding level.
	Collapse: For collapsing all nodes for all level from current collapsing level.
   Note: The node which has not any leaf nodes will not perform any of right click menu functionality.


*Copyright (c) 2021, Arm Limited and Contributors. All rights reserved.*
