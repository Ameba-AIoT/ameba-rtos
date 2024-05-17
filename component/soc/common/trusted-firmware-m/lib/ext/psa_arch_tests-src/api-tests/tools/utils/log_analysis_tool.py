#! /usr/bin/env python
#/** @file
# * Copyright (c) 2021, Arm Limited or its affiliates. All rights reserved.
# * SPDX-License-Identifier : Apache-2.0
# *
# * Licensed under the Apache License, Version 2.0 (the "License");
# * you may not use this file except in compliance with the License.
# * You may obtain a copy of the License at
# *
# *  http://www.apache.org/licenses/LICENSE-2.0
# *
# * Unless required by applicable law or agreed to in writing, software
# * distributed under the License is distributed on an "AS IS" BASIS,
# * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# * See the License for the specific language governing permissions and
# * limitations under the License.
#**/

import os, sys, re

gtoplevel = None
filename=''
filename_dir = []
testsuite_list = []
f_temp=[]
testsuite_temp=[]


try:
    import Tkinter as tk
    from Tkinter import tkFileDialog as fd
    from Tkinter import *
except ImportError:
    import tkinter as tk
    from tkinter import filedialog as fd
    from tkinter import *

try:
    import ttk
    py3 = False
except ImportError:
    import tkinter.ttk as ttk
    py3 = True

try:
    from tkinter import messagebox
except:
    import tkMessageBox as messagebox

# Browse the single testsuite log file
def browseLogFile():
    clearText()
    clearTree()
    filename_dir.clear()
    testsuite_list.clear()
    f_temp.clear()
    global filename
    global gtoplevel
    try:
        ftypes = [('all types', '*.*')]
        filename = fd.askopenfilename(initialdir="/", title="Select A File", filetypes=ftypes)
        if filename == "":
            clearText()
            gtoplevel.Scrolledtext1.insert("end", "\nPlease Select File\n")
            return;
        filename_dir.append(filename)
        findTestSuite(filename)

    except Exception as e:
        gtoplevel.Scrolledtext1.insert("end", "EXCEPTION : " + str(e) + "\n")
        pass

# Browse multiple testsuite log files in single folder
def browseLogFolder():
    global filename_dir
    global gtoplevel
    testsuite_list.clear()
    clearText()
    clearTree()
    filename_dir.clear()
    f_temp.clear()
    testsuite_temp.clear()


    try:
        dirname = fd.askdirectory()
        if dirname == "":
            clearText()
            gtoplevel.Scrolledtext1.insert("end", "\nPlease Select Directory\n")
            return;
        for path in os.listdir(dirname):
            full_path = os.path.join(dirname, path)
            if os.path.isfile(full_path):
                filename_dir.append(full_path)
        for file in filename_dir:
            findTestSuite(file)
        for f1 in f_temp:
            filename_dir.remove(f1)

        if testsuite_temp:
            testsuite_temp1 = set(testsuite_temp)
            messagebox.showwarning("Warning","{} Test Suite file is already exists. \nAll the duplicate test suite will not be added.".format(testsuite_temp1))
            testsuite_temp1.clear()

    except Exception as e:
        gtoplevel.Scrolledtext1.insert("end", "EXCEPTION : " + str(e) + "\n")
        pass

# Find testsuite from file.
def findTestSuite(filename):
    ut = False
    global gtoplevel
    f = open(filename, 'r+')
    try:

        for line in f:
            if re.search(r'PSA\sArchitecture\sTest\sSuite\s-\sVersion\s(.*?)\s',line):
                testsuite_version = re.search(r'PSA\sArchitecture\sTest\sSuite\s-\sVersion\s(.*?)\s',line).group(1)
                gtoplevel.Scrolledtreeview1.heading("#0", text="PSA Arch Test Suite "+testsuite_version)
                break
        for line in f:
            if re.search(r'^Running..\s(.*?)\sSuite$', line):
                testsuite = re.search("Running..\s(.*?)\sSuite", line).group(1)
                if testsuite in testsuite_list:
                    f_temp.append(filename)
                    testsuite_temp.append(testsuite)
                else:
                    testsuite_list.append(testsuite)

                    for line in f:
                        if re.search(r'^TEST:\s[0-9]+\s\|\sDESCRIPTION:\sTesting\s.*\s.*\sAPIs\s\|\sUT:\s.*\n', line):
                            ut = True
                            break
                    if testsuite == "":
                        gtoplevel.Scrolledtext1.insert("end","Please Select Proper File")
                    if ut is True:
                        if testsuite == "Storage":
                            storageDataExtraction(filename, testsuite)
                        else:
                            testsuiteDataExtraction(filename, testsuite)
                    else:
                        if testsuite=="Attestation":
                            attestationDataExtraction(filename, testsuite)
                        elif testsuite=="Storage":
                            storageDataExtraction(filename, testsuite)
                        elif testsuite=="Crypto":
                            cryptoDataExtraction(filename, testsuite)
                break

    except Exception as e:
        gtoplevel.Scrolledtext1.insert("end", "EXCEPTION : " + str(e) + "\n")
        pass


# Tests extraction from file. It will create list of tests.
def testsExtraction(fname):
    f = open(fname, 'r+')
    test = None
    tests = []
    is_test = False
    for line in f:
        if not is_test and line.startswith('TEST:'):
            is_test = True
            test = []

        elif is_test and line.startswith('TEST RESULT:'):
            test.append(line)
            is_test = False
            tests.append(''.join(test))
            test = None

        if is_test:
            test.append(line)
    return tests


# Crypto testsuite data extaction
def cryptoDataExtraction(fname, testsuite):
    global gtoplevel
    tests = testsExtraction(fname)
    testsuite = testsuite.lower()
    firstlevel = []
    secondlevel = []
    thirdlevel = []

    gtoplevel.Scrolledtreeview1.insert(parent='', index='end', iid=testsuite, text=testsuite)

    for i in tests:
        if re.search(r'^TEST:\s201',i):
            gtoplevel.Scrolledtreeview1.insert(parent=testsuite, index='end', iid="basic", text="Basic")
            gtoplevel.Scrolledtreeview1.insert(parent="basic", index='end', iid="psa_crypto_init", text="psa_crypto_init")
            test_num = re.search(r'(.*?)\sDESCRIPTION:', i).group(1)
            test_result = re.search(r'TEST\sRESULT:\s(.*?)\s', i).group(1)
            b = test_num + " " + test_result
            b_temp = b.replace(" ","")
            gtoplevel.Scrolledtreeview1.insert(parent="psa_crypto_init", index='end', iid=b_temp, text=b)

        if re.search(rf'^TEST:\s[0-9]+\s\|\sDESCRIPTION:\sTesting\s{testsuite}\s(.*?)\sAPIs', i):
            x = re.search(rf'^TEST:\s[0-9]+\s\|\sDESCRIPTION:\sTesting\s{testsuite}\s(.*?)\sAPIs', i).group(1)
            firstlevel.append(x) if x not in firstlevel else firstlevel

    if not firstlevel:
        messagebox.showerror("Error", "Test Suite File must be in required format.")
        clearTree()
        return

    for record in firstlevel:
        r_temp = record.replace(" ", "")
        gtoplevel.Scrolledtreeview1.insert(parent=testsuite, index='end', iid=r_temp, text=record)

    for i in firstlevel:
        secondlevel.clear()
        for j in tests:
            if re.search(rf'^TEST:\s[0-9]+\s\|\sDESCRIPTION:\sTesting\s{testsuite}\s{i}\sAPIs\n', j):
                if re.search(r'\[Check\s1\]\sTest\s(.*?)\s.*\n', j):
                    y = re.search(r'\[Check\s1\]\sTest\s(.*?)\s.*\n', j).group(1)
                    secondlevel.append(y) if y not in secondlevel else secondlevel

        for r in secondlevel:
            i_temp = i.replace(" ", "")
            gtoplevel.Scrolledtreeview1.insert(parent=i_temp, index='end', iid=r, text=r)

        for k in secondlevel:
            thirdlevel.clear()
            for m in tests:
                if re.search(rf'^TEST:\s[0-9]+\s\|\sDESCRIPTION:\sTesting\s{testsuite}\s{i}\sAPIs\n', m):
                    if re.search(rf'\[Check\s1\]\sTest\s{k}\s.*\n', m):
                        test_num = re.search(r'(.*?)\sDESCRIPTION:', m).group(1)
                        test_result = re.search(r'TEST\sRESULT:\s(.*?)\s', m).group(1)
                        b = test_num + " " + test_result
                        thirdlevel.append(b) if b not in thirdlevel else thirdlevel

            for rec in thirdlevel:
                rec_temp = rec.replace(" ", "")
                gtoplevel.Scrolledtreeview1.insert(parent=k, index='end', iid=rec_temp, text=rec)

    if not secondlevel or not thirdlevel:
        messagebox.showerror("Error", "Test Suite File must be in required format.")
        clearTree()


# Attestation testsuite data extraction
def attestationDataExtraction(fname, testsuite):
    global gtoplevel
    tests = testsExtraction(fname)
    testsuite = testsuite.lower()
    attest = []

    gtoplevel.Scrolledtreeview1.insert(parent='', index='end', iid=testsuite, text=testsuite)
    testsuite_new="initial attestation"
    for i in tests:
        if re.search(rf'^TEST:\s[0-9]+\s\|\sDESCRIPTION:\sTesting\s{testsuite_new}\sAPIs', i):
            if re.search(r'\[Check\s1\]\sTest\s(.*?)\s.*\n', i):
                y = re.search(r'\[Check\s1\]\sTest\s(.*?)\s.*\n', i).group(1)
                attest.append(y) if y not in attest else attest

    for r in attest:
        gtoplevel.Scrolledtreeview1.insert(parent=testsuite, index='end', iid=r, text=r)


    for j in attest:
        for t in tests:
            if re.search(rf'\[Check\s1\]\sTest\s{j}\s.*\n', t):
                test_num = re.search(r'(.*?)\sDESCRIPTION:', t).group(1)
                test_result = re.search(r'TEST\sRESULT:\s(.*?)\s', t).group(1)
                b = test_num + " " + test_result
                gtoplevel.Scrolledtreeview1.insert(parent=j, index='end', iid=b, text=b)

# Storage testsuite data extraction
def storageDataExtraction(fname, testsuite):
    global gtoplevel
    tests = testsExtraction(fname)
    testsuite = testsuite.lower()

    gtoplevel.Scrolledtreeview1.insert(parent='', index='end', iid=testsuite, text=testsuite)
    gtoplevel.Scrolledtreeview1.insert(parent=testsuite, index='end', iid="ITS", text="ITS")
    gtoplevel.Scrolledtreeview1.insert(parent=testsuite, index='end', iid="PS", text="PS")
    for i in tests:
        test_num = re.search(r'(.*?)\sDESCRIPTION:', i).group(1)
        test_result = re.search(r'TEST\sRESULT:\s(.*?)\s', i).group(1)
        b = test_num + " " + test_result
        b_temp = b.replace(" ", "")
        if re.search(r'\[Info\]\sExecuting\sITS\stests\n', i):
            b_temp1 = "its_"+b_temp
            gtoplevel.Scrolledtreeview1.insert(parent="ITS", index='end', iid=b_temp1, text=b)

        if re.search(r'\[Info\]\sExecuting\sPS\stests\n', i):
            b_temp2 = "ps_" + b_temp
            gtoplevel.Scrolledtreeview1.insert(parent="PS", index='end', iid=b_temp2, text=b)

    its_child = gtoplevel.Scrolledtreeview1.get_children("ITS")
    if not its_child:
        gtoplevel.Scrolledtreeview1.delete("ITS")
    ps_child = gtoplevel.Scrolledtreeview1.get_children("PS")
    if not ps_child:
        gtoplevel.Scrolledtreeview1.delete("PS")


# Generic testsuite data extraction
def testsuiteDataExtraction(fname, testsuite):
    global gtoplevel
    tests = testsExtraction(fname)
    firstlevel = []
    secondlevel = []
    thirdlevel = []
    testsuite = testsuite.lower()

    gtoplevel.Scrolledtreeview1.insert(parent='', index='end', iid=testsuite, text=testsuite)

    for i in tests:
        if re.search(rf'^TEST:\s[0-9]+\s\|\sDESCRIPTION:\sTesting\s{testsuite}\s(.*?)\sAPIs', i):
            x = re.search(rf'^TEST:\s[0-9]+\s\|\sDESCRIPTION:\sTesting\s{testsuite}\s(.*?)\sAPIs', i).group(1)
            firstlevel.append(x) if x not in firstlevel else firstlevel

    if not firstlevel:
        messagebox.showerror("Error","Test Suite File must be in required format.")
        clearTree()
        return

    for record in firstlevel:
        r_temp = record.replace(" ", "")
        gtoplevel.Scrolledtreeview1.insert(parent=testsuite, index='end', iid=r_temp, text=record)

    for i in firstlevel:
        secondlevel.clear()
        for j in tests:
            if re.search(rf'^TEST:\s[0-9]+\s\|\sDESCRIPTION:\sTesting\s{testsuite}\s{i}\sAPIs\s\|\sUT:\s(.*?)\n', j):
                y = re.search(rf'^TEST:\s[0-9]+\s\|\sDESCRIPTION:\sTesting\s{testsuite}\s{i}\sAPIs\s\|\sUT:\s(.*?)\n', j).group(1)
                secondlevel.append(y) if y not in secondlevel else secondlevel

        for r in secondlevel:
            i_temp = i.replace(" ","")
            gtoplevel.Scrolledtreeview1.insert(parent=i_temp, index='end', iid=r, text=r)

        for k in secondlevel:
            thirdlevel.clear()
            for m in tests:
                if re.search(rf'^TEST:\s[0-9]+\s\|\sDESCRIPTION:\sTesting\s{testsuite}\s{i}\sAPIs\s\|\sUT:\s{k}\n', m):
                    test_num = re.search(r'(.*?)\sDESCRIPTION:',m).group(1)
                    test_result = re.search(r'TEST\sRESULT:\s(.*?)\s',m).group(1)
                    b = test_num+" "+test_result
                    thirdlevel.append(b) if b not in thirdlevel else thirdlevel

            for rec in thirdlevel:
                rec_temp=rec.replace(" ","")
                gtoplevel.Scrolledtreeview1.insert(parent=k, index='end', iid=rec_temp, text=rec)

    if not secondlevel or not thirdlevel:
        messagebox.showerror("Error", "Test Suite File must be in required format.")
        clearTree()


# Get the summary result of selected tree node
def summary_result():
    global gtoplevel
    passed = 0
    failed = 0
    skipped = 0
    sim_error = 0
    clearText()
    selected_item = gtoplevel.Scrolledtreeview1.selection()[0]

    if selected_item == "storage" :
        firstlevel = gtoplevel.Scrolledtreeview1.get_children(selected_item)
        for child in firstlevel:
            passed = 0
            failed = 0
            skipped = 0
            sim_error = 0
            secondlevel = gtoplevel.Scrolledtreeview1.get_children(child)
            if not secondlevel:
                result = gtoplevel.Scrolledtreeview1.item(child)['text']
                if re.search(r'TEST:\s[0-9]+\s\|\s(.*?)', result):
                    r = re.search(r'TEST:\s[0-9]+\s\|\s(.*?)$', result).group(1)
                    if r == "PASSED":
                        passed = passed + 1
                    if r == "FAILED":
                        failed = failed + 1
                    if r == "SKIPPED":
                        skipped = skipped + 1
                    if r == "SIM ERROR":
                        sim_error = sim_error + 1

            else:
                for c in secondlevel:
                    thirdlevel = gtoplevel.Scrolledtreeview1.get_children(c)

                    if not thirdlevel:
                        result = gtoplevel.Scrolledtreeview1.item(c)['text']
                        if re.search(r'TEST:\s[0-9]+\s\|\s(.*?)', result):
                            r = re.search(r'TEST:\s[0-9]+\s\|\s(.*?)$', result).group(1)
                            if r == "PASSED":
                                passed = passed + 1
                            if r == "FAILED":
                                failed = failed + 1
                            if r == "SKIPPED":
                                skipped = skipped + 1
                            if r == "SIM ERROR":
                                sim_error = sim_error + 1

            total_test = passed + failed + skipped + sim_error
            data1 = "************ "+child+" Summary Report ************\nTOTAL TESTS   :" + str(
                total_test) + "\nTOTAL PASSED    :" + str(
                passed) + "\nTOTAL SIM ERROR :" + str(sim_error) + "\nTOTAL FAILED   :" + str(
                failed) + "\nTOTAL SKIPPED   :" + str(skipped) + "\n***********************************************\n"
            gtoplevel.Scrolledtext1.insert(END, data1)
    else:
        firstlevel = gtoplevel.Scrolledtreeview1.get_children(selected_item)

        if not firstlevel:
            gtoplevel.Scrolledtext1.insert(END,"This node has not any leaf nodes.")
        else:
            for child in firstlevel:
                secondlevel = gtoplevel.Scrolledtreeview1.get_children(child)
                if not secondlevel:
                    result = gtoplevel.Scrolledtreeview1.item(child)['text']
                    if re.search(r'TEST:\s[0-9]+\s\|\s(.*?)', result):
                        r = re.search(r'TEST:\s[0-9]+\s\|\s(.*?)$', result).group(1)
                        if r == "PASSED":
                            passed = passed + 1
                        if r == "FAILED":
                            failed = failed + 1
                        if r == "SKIPPED":
                            skipped = skipped + 1
                        if r == "SIM ERROR":
                            sim_error = sim_error + 1
                else:
                    for c in secondlevel:
                        thirdlevel = gtoplevel.Scrolledtreeview1.get_children(c)

                        if not thirdlevel:
                            result = gtoplevel.Scrolledtreeview1.item(c)['text']
                            if re.search(r'TEST:\s[0-9]+\s\|\s(.*?)', result):
                                r = re.search(r'TEST:\s[0-9]+\s\|\s(.*?)$', result).group(1)
                                if r == "PASSED":
                                    passed = passed + 1
                                if r == "FAILED":
                                    failed = failed + 1
                                if r == "SKIPPED":
                                    skipped = skipped + 1
                                if r == "SIM ERROR":
                                    sim_error = sim_error + 1
                        else:
                            for c1 in thirdlevel:
                                fourthlevel = gtoplevel.Scrolledtreeview1.get_children(c1)
                                if not fourthlevel:
                                    result = gtoplevel.Scrolledtreeview1.item(c1)['text']
                                    if re.search(r'TEST:\s[0-9]+\s\|\s(.*?)', result):
                                        r = re.search(r'TEST:\s[0-9]+\s\|\s(.*?)$', result).group(1)
                                        if r == "PASSED":
                                            passed = passed + 1
                                        if r == "FAILED":
                                            failed = failed + 1
                                        if r == "SKIPPED":
                                            skipped = skipped + 1
                                        if r == "SIM ERROR":
                                            sim_error = sim_error + 1

            total_test = passed + failed + skipped + sim_error
            data1 = "************ Summary Report ************\nTOTAL TESTS   :" + str(total_test) + "\nTOTAL PASSED    :" + str(
                passed) + "\nTOTAL SIM ERROR :" + str(sim_error) + "\nTOTAL FAILED   :" + str(
                failed) + "\nTOTAL SKIPPED   :" + str(skipped) + "\n******************************************"
            gtoplevel.Scrolledtext1.insert(END, data1)


# On Right click of tree item it will display summary result of that tree node
def onRightClick(e):
    global gtoplevel
    try:
        e.widget.focus()
        item = gtoplevel.Scrolledtreeview1.identify('item', e.x, e.y)
        rmenu = Menu(None, tearoff=0, takefocus=0)
        rmenu.add_command(label="Summary", command=summary_result)
        rmenu.add_command(label="Expand", command=lambda: open_tree(item))
        rmenu.add_command(label="Collapse", command=lambda: close_tree(item))
        rmenu.tk_popup(e.x_root + 40, e.y_root + 10, entry="0")
    except TclError:
        print(' - rClick menu, something wrong')
        pass

    return "break"

# On clicking double click on tree item for test it will fetch the test data from file and insert in text widget
def onDoubleClick(a):
    global gtoplevel
    global filename
    global filename_dir

    clearText()
    selected_item = gtoplevel.Scrolledtreeview1.selection()[0]
    cur_item = gtoplevel.Scrolledtreeview1.item(selected_item)['text']


    if re.search(r'^TEST:', cur_item):
        for file in filename_dir:
            data = showTest(file,cur_item)
            try:
                if data:
                    gtoplevel.Scrolledtext1.insert(END, data)
                    pass
            except Exception as e:
                print ('error : '+e)


# Get specific selected test from file
def showTest(fname,selected_item):
    tests = testsExtraction(fname)
    x = selected_item.split('|')
    for i in tests:
        if re.search(rf'{x[0]}',i):
            return i


# Clear Tree Widget Data
def clearTree():
    global gtoplevel
    for i in gtoplevel.Scrolledtreeview1.get_children():
        gtoplevel.Scrolledtreeview1.delete(i)


# Clear Text Widget Data
def clearText():
    global gtoplevel
    gtoplevel.Scrolledtext1.delete("1.0", "end")

# Collapse tree nodes
def close_tree(parent):
    global gtoplevel
    gtoplevel.Scrolledtreeview1.item(parent, open=False)
    for child in gtoplevel.Scrolledtreeview1.get_children(parent):
        close_tree(child)

# Expand tree nodes
def open_tree(parent):
    global gtoplevel
    gtoplevel.Scrolledtreeview1.item(parent, open=True)
    for child in gtoplevel.Scrolledtreeview1.get_children(parent):
        open_tree(child)


def vp_start_gui():
    '''Starting point when module is the main routine.'''
    global gtoplevel
    global val, w, root
    root = tk.Tk()
    top = Toplevel1 (root)

    if platform.system() == 'Linux':
        root.attributes('-zoomed', True)
    elif platform.system() == 'Windows':
        root.state('zoomed')
    gtoplevel = top
    gui_init(root, top)
    root.mainloop()

w = None
def create_Toplevel1(rt, *args, **kwargs):
    '''Starting point when module is imported by another module.
       Correct form of call: 'create_Toplevel1(root, *args, **kwargs)' .'''
    global w, w_win, root
    #rt = root
    root = rt
    w = tk.Toplevel (root)
    top = Toplevel1 (w)
    gui_init(w, top, *args, **kwargs)
    return (w, top)

def destroy_Toplevel1():
    global w
    w.destroy()
    w = None

def gui_init(top, gui, *args, **kwargs):
    global w, top_level, root
    w = gui
    top_level = top
    root = top

def destroy_window():
    # Function which closes the window.
    global top_level
    top_level.destroy()
    top_level = None

class Toplevel1:
    def __init__(self, top=None):
        '''This class configures and populates the toplevel window.
           top is the toplevel containing window.'''
        _bgcolor = '#d9d9d9'  # X11 color: 'gray85'
        _fgcolor = '#000000'  # X11 color: 'black'
        _compcolor = '#d9d9d9' # X11 color: 'gray85'
        _ana1color = '#d9d9d9' # X11 color: 'gray85'
        _ana2color = '#ececec' # Closest X11 color: 'gray92'
        self.style = ttk.Style()
        if sys.platform == "win32":
            self.style.theme_use('winnative')
        self.style.configure('.',background="white")
        self.style.configure('.',foreground=_fgcolor)
        self.style.configure('.',font="TkDefaultFont")
        self.style.map('.',background=
            [('selected', _compcolor), ('active',_ana2color)])

        self.style.configure('Treeview', font="TkDefaultFont")
        top.geometry("1250x500")
        top.minsize(176, 1)
        top.maxsize(1924, 1050)
        top.resizable(1, 1)

        top.title("PSA Test Suite Log Analysis Tool")
        top.configure(background="white")
        top.configure(highlightbackground="#d9d9d9")
        top.configure(highlightcolor="black")
        top.columnconfigure(0, weight=1)
        top.columnconfigure(1, weight=2)
        top.rowconfigure(0, weight=1)

        self.Scrolledtext1 = ScrolledText(top, borderwidth=1)
        self.Scrolledtext1.grid(row = 0, column = 1, sticky="NSEW")
        self.Scrolledtext1.configure(background="white")
        self.Scrolledtext1.configure(font="TkTextFont")
        self.Scrolledtext1.configure(foreground="black")
        self.Scrolledtext1.configure(highlightbackground="#d9d9d9")
        self.Scrolledtext1.configure(highlightcolor="black")
        self.Scrolledtext1.configure(insertbackground="black")
        self.Scrolledtext1.configure(insertborderwidth="3")
        self.Scrolledtext1.configure(selectbackground="#c4c4c4")
        self.Scrolledtext1.configure(selectforeground="black")
        self.Scrolledtext1.configure(wrap="none")

        self.Scrolledtreeview1 = ScrolledTreeView(top)
        self.Scrolledtreeview1.grid(row=0, column=0, sticky="NSEW")
        self.Scrolledtreeview1.heading("#0",text="Test Suite")
        self.Scrolledtreeview1.heading("#0",anchor="center")
        self.Scrolledtreeview1.column("#0", width="10")
        self.Scrolledtreeview1.column("#0",minwidth="250")
        self.Scrolledtreeview1.column("#0",anchor="w")
        self.Scrolledtreeview1.bind('<Double-Button-1>', onDoubleClick)
        self.Scrolledtreeview1.bind('<Button-3>', onRightClick)

        self.Menu1 = tk.Menu(top)
        top.configure(menu=self.Menu1)
        self.filemenu=tk.Menu(top)
        self.Menu1.add_cascade(menu=self.filemenu, label="Menu")
        self.filemenu.add_command(label="Select Log File", command=browseLogFile)
        self.filemenu.add_command(label="Select Log Folder", command=browseLogFolder)
        self.filemenu.add_command(label="Exit", command=top.quit)


# The following code is added to facilitate the Scrolled widgets you specified.
class AutoScroll(object):
    '''Configure the scrollbars for a widget.'''
    def __init__(self, master):
        #  Rozen. Added the try-except clauses so that this class
        #  could be used for scrolled entry widget for which vertical
        #  scrolling is not supported. 5/7/14.
        try:
            vsb = ttk.Scrollbar(master, orient='vertical', command=self.yview)
        except:
            pass
        hsb = ttk.Scrollbar(master, orient='horizontal', command=self.xview)
        try:
            self.configure(yscrollcommand=self._autoscroll(vsb))
        except:
            pass
        self.configure(xscrollcommand=self._autoscroll(hsb))
        self.grid(column=0, row=0, sticky='nsew')
        try:
            vsb.grid(column=1, row=0, sticky='ns')
        except:
            pass
        hsb.grid(column=0, row=1, sticky='ew')
        master.grid_columnconfigure(0, weight=1)
        master.grid_rowconfigure(0, weight=1)
        # Copy geometry methods of master  (taken from ScrolledText.py)
        if py3:
            methods = tk.Pack.__dict__.keys() | tk.Grid.__dict__.keys() \
                  | tk.Place.__dict__.keys()
        else:
            methods = tk.Pack.__dict__.keys() + tk.Grid.__dict__.keys() \
                  + tk.Place.__dict__.keys()
        for meth in methods:
            if meth[0] != '_' and meth not in ('config', 'configure'):
                setattr(self, meth, getattr(master, meth))

    @staticmethod
    def _autoscroll(sbar):
        '''Hide and show scrollbar as needed.'''
        def wrapped(first, last):
            first, last = float(first), float(last)
            if first <= 0 and last >= 1:
                sbar.grid_remove()
            else:
                sbar.grid()
            sbar.set(first, last)
        return wrapped

    def __str__(self):
        return str(self.master)

def _create_container(func):
    '''Creates a ttk Frame with a given master, and use this new frame to
    place the scrollbars and the widget.'''
    def wrapped(cls, master, **kw):
        container = ttk.Frame(master)
        container.bind('<Enter>', lambda e: _bound_to_mousewheel(e, container))
        container.bind('<Leave>', lambda e: _unbound_to_mousewheel(e, container))
        return func(cls, container, **kw)
    return wrapped

class ScrolledText(AutoScroll, tk.Text):
    '''A standard Tkinter Text widget with scrollbars that will
    automatically show/hide as needed.'''
    @_create_container
    def __init__(self, master, **kw):
        tk.Text.__init__(self, master, **kw)
        AutoScroll.__init__(self, master)

class ScrolledTreeView(AutoScroll, ttk.Treeview):
    '''A standard ttk Treeview widget with scrollbars that will
    automatically show/hide as needed.'''
    @_create_container
    def __init__(self, master, **kw):
        ttk.Treeview.__init__(self, master, **kw)
        AutoScroll.__init__(self, master)


import platform
def _bound_to_mousewheel(event, widget):
    child = widget.winfo_children()[0]
    if platform.system() == 'Windows' or platform.system() == 'Darwin':
        child.bind_all('<MouseWheel>', lambda e: _on_mousewheel(e, child))
        child.bind_all('<Shift-MouseWheel>', lambda e: _on_shiftmouse(e, child))
    else:
        child.bind_all('<Button-4>', lambda e: _on_mousewheel(e, child))
        child.bind_all('<Button-5>', lambda e: _on_mousewheel(e, child))
        child.bind_all('<Shift-Button-4>', lambda e: _on_shiftmouse(e, child))
        child.bind_all('<Shift-Button-5>', lambda e: _on_shiftmouse(e, child))

def _unbound_to_mousewheel(event, widget):
    if platform.system() == 'Windows' or platform.system() == 'Darwin':
        widget.unbind_all('<MouseWheel>')
        widget.unbind_all('<Shift-MouseWheel>')
    else:
        widget.unbind_all('<Button-4>')
        widget.unbind_all('<Button-5>')
        widget.unbind_all('<Shift-Button-4>')
        widget.unbind_all('<Shift-Button-5>')

def _on_mousewheel(event, widget):
    if platform.system() == 'Windows':
        widget.yview_scroll(-1*int(event.delta/120),'units')
    elif platform.system() == 'Darwin':
        widget.yview_scroll(-1*int(event.delta),'units')
    else:
        if event.num == 4:
            widget.yview_scroll(-1, 'units')
        elif event.num == 5:
            widget.yview_scroll(1, 'units')

def _on_shiftmouse(event, widget):
    if platform.system() == 'Windows':
        widget.xview_scroll(-1*int(event.delta/120), 'units')
    elif platform.system() == 'Darwin':
        widget.xview_scroll(-1*int(event.delta), 'units')
    else:
        if event.num == 4:
            widget.xview_scroll(-1, 'units')
        elif event.num == 5:
            widget.xview_scroll(1, 'units')

if __name__ == '__main__':
    vp_start_gui()
